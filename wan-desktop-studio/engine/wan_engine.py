"""Wan 2.1 Inference Engine optimized for NVIDIA RTX 4060 (8GB VRAM)
Provides Text-to-Video and Image-to-Video with CPU offloading and VAE tiling.
"""

import os
import gc
import time
import json
import logging
import threading
from typing import Dict, Any, Optional, Callable, List
from PIL import Image

logger = logging.getLogger("WanEngine")

class WanEngine:
    def __init__(self, models_dir: str, outputs_dir: str, model_hub=None):
        self.models_dir = os.path.abspath(models_dir)
        self.outputs_dir = os.path.abspath(outputs_dir)
        self.model_hub = model_hub
        os.makedirs(self.outputs_dir, exist_ok=True)

        self.current_pipeline = None
        self.current_model_id = None
        self.current_pipeline_type = None  # "t2v" or "i2v"
        self.is_generating = False
        self.last_progress_data = None
        self.last_result = None
        self.last_error = None
        self.lock = threading.Lock()

        # Optimize PyTorch CUDA global settings
        self._configure_cuda_optimizations()

    def _configure_cuda_optimizations(self):
        try:
            import os
            import torch
            os.environ["PYTORCH_CUDA_ALLOC_CONF"] = "expandable_segments:True"
            if torch.cuda.is_available():
                torch.backends.cuda.matmul.allow_tf32 = True
                torch.backends.cudnn.allow_tf32 = True
                torch.backends.cudnn.benchmark = True
                logger.info("TensorFloat-32 (TF32), cuDNN benchmark y expandable_segments activados para RTX 4060.")
        except Exception as e:
            logger.warning(f"No se pudieron configurar optimizaciones globales de CUDA: {e}")

    def free_memory(self):
        """Aggressively clear GPU VRAM cache and collect garbage."""
        try:
            import torch
            gc.collect()
            if torch.cuda.is_available():
                torch.cuda.empty_cache()
                torch.cuda.ipc_collect()
                logger.info("Caché de VRAM limpiada.")
        except Exception as e:
            logger.debug(f"Error liberando memoria: {e}")

    def unload_current_model(self):
        with self.lock:
            if self.current_pipeline is not None:
                del self.current_pipeline
                self.current_pipeline = None
                self.current_model_id = None
                self.current_pipeline_type = None
                self.free_memory()
                logger.info("Modelo descargado de memoria.")

    def load_pipeline(
        self,
        model_id: str = "Wan-AI/Wan2.1-T2V-1.3B-Diffusers",
        pipeline_type: str = "t2v",
        quantization: Optional[str] = None, # None, "4bit", "8bit"
        status_callback: Optional[Callable] = None
    ):
        """Load WanPipeline or WanImageToVideoPipeline with RTX 4060 optimizations."""
        import torch

        if self.current_pipeline is not None and self.current_model_id == model_id and self.current_pipeline_type == pipeline_type:
            logger.info(f"Modelo {model_id} ({pipeline_type}) ya está cargado.")
            return self.current_pipeline

        self.unload_current_model()

        # -------------------------------------------------------------
        # STEP 1: PRE-FLIGHT CHECK - DOWNLOAD IF NOT INSTALLED
        # -------------------------------------------------------------
        clean_name = model_id.replace("/", "--")
        local_dir = os.path.join(self.models_dir, clean_name)

        model_is_installed = False
        if self.model_hub:
            model_is_installed = self.model_hub.is_model_installed(model_id)
        else:
            model_is_installed = os.path.exists(local_dir) and (
                os.path.exists(os.path.join(local_dir, "model_index.json")) or
                any(f.endswith(".safetensors") for f in os.listdir(local_dir))
            )

        if not model_is_installed:
            logger.info(f"Modelo {model_id} no detectado localmente. Iniciando descarga con seguimiento en vivo a {local_dir}...")
            if status_callback:
                status_callback({
                    "status": "descargando_modelo",
                    "progress_percent": 0.0,
                    "downloaded_mb": 0,
                    "total_mb": 12800,
                    "speed_mb_s": 0,
                    "eta_seconds": 300,
                    "message": f"Iniciando descarga del modelo {model_id} en disco D:..."
                })

            if self.model_hub:
                self.model_hub.download_huggingface_repo_with_progress(
                    repo_id=model_id,
                    progress_callback=status_callback
                )
            else:
                from huggingface_hub import snapshot_download
                snapshot_download(repo_id=model_id, local_dir=local_dir, resume_download=True)

        # -------------------------------------------------------------
        # STEP 2: LOAD MODEL LOCALLY WITH 8GB OPTIMIZATIONS
        # -------------------------------------------------------------
        stop_ticker = threading.Event()
        start_load_time = time.time()

        def _loading_ticker():
            while not stop_ticker.is_set():
                elapsed = int(time.time() - start_load_time)
                pct = round(min(7.8, 3.0 + (elapsed / 120.0) * 4.8), 1)
                if status_callback:
                    status_callback({
                        "status": "cargando_modelo",
                        "progress_percent": pct,
                        "elapsed_seconds": elapsed,
                        "message": f"Cargando pesos de {model_id} en memoria RAM... ({elapsed}s transcurridos, no cerrar)"
                    })
                stop_ticker.wait(3.0)

        ticker_thread = threading.Thread(target=_loading_ticker, daemon=True)
        ticker_thread.start()

        pretrained_path = local_dir if os.path.exists(local_dir) else model_id
        dtype = torch.bfloat16 if torch.cuda.is_bf16_supported() else torch.float16
        logger.info(f"Cargando {pretrained_path} usando dtype={dtype}...")

        try:
            # Quantization config if selected
            transformer = None
            if quantization in ["4bit", "8bit"]:
                try:
                    from diffusers import BitsAndBytesConfig
                    from diffusers.models import WanTransformer3DModel
                    load_in_4bit = (quantization == "4bit")
                    bnb_config = BitsAndBytesConfig(
                        load_in_4bit=load_in_4bit,
                        load_in_8bit=not load_in_4bit,
                        bnb_4bit_compute_dtype=dtype
                    )
                    logger.info(f"Aplicando cuantización BitsAndBytes {quantization} al Transformer...")
                    transformer = WanTransformer3DModel.from_pretrained(
                        pretrained_path,
                        subfolder="transformer",
                        quantization_config=bnb_config,
                        torch_dtype=dtype
                    )
                except Exception as e:
                    logger.warning(f"No se pudo aplicar cuantización BitsAndBytes: {e}. Continuando en {dtype} nativo.")

            # Check if loading a distilled / single-file model without model_index.json
            distilled_file = None
            if os.path.exists(local_dir):
                for f in os.listdir(local_dir):
                    if f.endswith(".safetensors"):
                        distilled_file = os.path.join(local_dir, f)
                        break

            if distilled_file and not os.path.exists(os.path.join(local_dir, "model_index.json")):
                logger.info(f"Cargando modelo destilado/single-file desde {distilled_file}...")
                base_model_path = os.path.join(self.models_dir, "Wan-AI--Wan2.1-T2V-1.3B-Diffusers")
                if not os.path.exists(base_model_path):
                    base_model_path = "Wan-AI/Wan2.1-T2V-1.3B-Diffusers"

                from diffusers.models import WanTransformer3DModel
                transformer = WanTransformer3DModel.from_single_file(distilled_file, torch_dtype=dtype)
                
                if pipeline_type == "i2v":
                    from diffusers import WanImageToVideoPipeline
                    pipe = WanImageToVideoPipeline.from_pretrained(
                        base_model_path,
                        transformer=transformer,
                        torch_dtype=dtype
                    )
                else:
                    from diffusers import WanPipeline
                    pipe = WanPipeline.from_pretrained(
                        base_model_path,
                        transformer=transformer,
                        torch_dtype=dtype
                    )
            else:
                # Instantiate pipeline according to type
                if pipeline_type == "i2v":
                    from diffusers import WanImageToVideoPipeline
                    if transformer is not None:
                        pipe = WanImageToVideoPipeline.from_pretrained(
                            pretrained_path,
                            transformer=transformer,
                            torch_dtype=dtype
                        )
                    else:
                        pipe = WanImageToVideoPipeline.from_pretrained(
                            pretrained_path,
                            torch_dtype=dtype
                        )
                else:
                    from diffusers import WanPipeline
                    if transformer is not None:
                        pipe = WanPipeline.from_pretrained(
                            pretrained_path,
                            transformer=transformer,
                            torch_dtype=dtype
                        )
                    else:
                        pipe = WanPipeline.from_pretrained(
                            pretrained_path,
                            torch_dtype=dtype
                        )

            # -------------------------------------------------------------
            # CRITICAL RTX 4060 (8GB VRAM) HIGH-SPEED ARCHITECTURE
            # -------------------------------------------------------------
            is_14b_or_distilled = ("14b" in model_id.lower()) or ("distill" in model_id.lower()) or ("lightx2v" in model_id.lower()) or (distilled_file is not None)
            
            if is_14b_or_distilled:
                logger.info("Modelo 14B / Distill detectado: activando Sequential CPU Offload para soportar 14B en los 8GB VRAM de la RTX 4060 sin OOM.")
                pipe.enable_sequential_cpu_offload()
                if hasattr(pipe, "vae") and pipe.vae is not None:
                    if hasattr(pipe.vae, "enable_tiling"):
                        pipe.vae.enable_tiling()
                        logger.info("VAE Tiling activado exitosamente.")
                    if hasattr(pipe.vae, "enable_slicing"):
                        pipe.vae.enable_slicing()
                        logger.info("VAE Slicing activado exitosamente.")
            else:
                # For 1.3B model (5.28 GB transformer fits directly into 8GB VRAM):
                logger.info("Modelo 1.3B detectado: T5-XXL -> CPU RAM (48GB), Transformer (5.2GB) -> CUDA (RTX 4060), VAE -> CUDA")
                if hasattr(pipe, "text_encoder") and pipe.text_encoder is not None:
                    pipe.text_encoder.to("cpu")
                if hasattr(pipe, "transformer") and pipe.transformer is not None:
                    pipe.transformer.to("cuda")
                if hasattr(pipe, "vae") and pipe.vae is not None:
                    pipe.vae.to("cuda")
                    if hasattr(pipe.vae, "enable_tiling"):
                        pipe.vae.enable_tiling()
                        logger.info("VAE Tiling activado exitosamente.")
                    if hasattr(pipe.vae, "enable_slicing"):
                        pipe.vae.enable_slicing()
                        logger.info("VAE Slicing activado exitosamente.")
        finally:
            stop_ticker.set()

        self.current_pipeline = pipe
        self.current_model_id = model_id
        self.current_pipeline_type = pipeline_type

        if status_callback:
            status_callback({"status": "modelo_listo", "progress_percent": 8.0, "message": f"Modelo {model_id} listo en memoria."})

        return pipe

    def generate_video(
        self,
        prompt: str,
        negative_prompt: str = "",
        image_path: Optional[str] = None,
        model_id: str = "Wan-AI/Wan2.1-T2V-1.3B-Diffusers",
        pipeline_type: str = "t2v",
        height: int = 480,
        width: int = 832,
        num_frames: int = 49,
        fps: int = 16,
        num_inference_steps: int = 25,
        guidance_scale: float = 5.0,
        seed: Optional[int] = None,
        quantization: Optional[str] = None,
        progress_callback: Optional[Callable] = None
    ) -> Dict[str, Any]:
        """Run video generation loop with real-time step streaming and MP4 output."""
        import torch
        import imageio

        with self.lock:
            self.is_generating = True
            self.last_result = None
            self.last_error = None

        start_time = time.time()
        try:
            # Load or reuse pipeline (will download with live progress if needed!)
            pipe = self.load_pipeline(
                model_id=model_id,
                pipeline_type=pipeline_type,
                quantization=quantization,
                status_callback=progress_callback
            )

            # If distilled 4-step model, configure optimized 4-step schedule and guidance
            if "distill" in model_id.lower() or "lightx2v" in model_id.lower():
                if num_inference_steps > 8:
                    num_inference_steps = 4
                if guidance_scale > 2.0:
                    guidance_scale = 1.0
                logger.info(f"Modelo Turbo Distilled detectado: configurando pasos={num_inference_steps}, guidance={guidance_scale}")

            # Generator / Seed
            if seed is None or seed == -1:
                seed = int(torch.randint(0, 2**32 - 1, (1,)).item())
            generator = torch.Generator(device="cpu").manual_seed(seed)

            # Encode prompt on CPU (Intel i7-12700KF with 48GB RAM uses 0 VRAM and prevents PCIe thrashing!)
            if progress_callback:
                prog_data = {
                    "status": "codificando_prompt",
                    "step": 0,
                    "total_steps": num_inference_steps,
                    "progress_percent": 6.0,
                    "eta_seconds": int(num_inference_steps * 4.2),
                    "vram_gb": 0.0,
                    "message": "Codificando prompt semántico con T5-XXL en procesador Intel i7..."
                }
                self.last_progress_data = prog_data
                progress_callback(prog_data)

            if ("14b" in model_id.lower()) or ("distill" in model_id.lower()) or ("lightx2v" in model_id.lower()):
                logger.info("Codificando prompt semántico con T5-XXL (Sequential Offload)...")
                prompt_embeds, negative_prompt_embeds = pipe.encode_prompt(
                    prompt=prompt,
                    negative_prompt=negative_prompt if negative_prompt else None,
                )
            else:
                logger.info("Codificando prompt semántico con T5-XXL en CPU...")
                prompt_embeds, negative_prompt_embeds = pipe.encode_prompt(
                    prompt=prompt,
                    negative_prompt=negative_prompt if negative_prompt else None,
                    device=torch.device("cpu"),
                )
            prompt_embeds = prompt_embeds.to("cuda")
            if negative_prompt_embeds is not None:
                negative_prompt_embeds = negative_prompt_embeds.to("cuda")

            # Step callback for real-time progress
            step_times = []
            last_step_time = time.time()

            def _step_callback(p, step, timestep, callback_kwargs):
                nonlocal last_step_time
                now = time.time()
                dt = now - last_step_time
                last_step_time = now
                step_times.append(dt)

                avg_step_time = sum(step_times[-5:]) / len(step_times[-5:]) if step_times else dt
                remaining_steps = max(0, num_inference_steps - (step + 1))
                eta_s = int(remaining_steps * avg_step_time)
                # Scale cleanly from 10% to 95%
                progress_pct = round(10.0 + ((step + 1) / num_inference_steps) * 85.0, 1)

                vram_gb = 0.0
                try:
                    if torch.cuda.is_available():
                        vram_gb = round(torch.cuda.memory_allocated() / (1024**3), 2)
                except Exception:
                    pass

                prog_data = {
                    "status": "generando",
                    "step": step + 1,
                    "total_steps": num_inference_steps,
                    "progress_percent": progress_pct,
                    "step_time_s": round(dt, 2),
                    "eta_seconds": eta_s,
                    "vram_gb": vram_gb,
                    "message": f"Renderizando paso {step + 1} de {num_inference_steps} ({progress_pct}%)... ({dt:.1f}s/it)"
                }
                self.last_progress_data = prog_data
                if progress_callback:
                    progress_callback(prog_data)
                return callback_kwargs

            self.last_progress_data = {
                "status": "generando",
                "step": 0,
                "total_steps": num_inference_steps,
                "progress_percent": 10.0,
                "eta_seconds": int(num_inference_steps * 4.2),
                "message": f"Iniciando difusión en RTX 4060 (0/{num_inference_steps} pasos)..."
            }
            if progress_callback:
                progress_callback(self.last_progress_data)

            # Prepare inputs with pre-encoded embeddings
            kwargs = {
                "prompt_embeds": prompt_embeds,
                "negative_prompt_embeds": negative_prompt_embeds,
                "height": height,
                "width": width,
                "num_frames": num_frames,
                "num_inference_steps": num_inference_steps,
                "guidance_scale": guidance_scale,
                "generator": generator,
                "callback_on_step_end": _step_callback,
            }

            if pipeline_type == "i2v" and image_path and os.path.exists(image_path):
                input_image = Image.open(image_path).convert("RGB")
                input_image = input_image.resize((width, height), Image.Resampling.LANCZOS)
                kwargs["image"] = input_image

            # Execute generation
            logger.info(f"Comenzando difusión acelerada en RTX 4060 ({width}x{height}, {num_frames} frames, {num_inference_steps} steps)...")
            output = pipe(**kwargs)

            # Extract frames
            frames = output.frames[0]

            if progress_callback:
                dec_data = {
                    "status": "codificando_video",
                    "step": num_inference_steps,
                    "total_steps": num_inference_steps,
                    "progress_percent": 98.0,
                    "message": "Decodificando latentes con VAE y codificando MP4..."
                }
                self.last_progress_data = dec_data
                progress_callback(dec_data)

            # Save MP4 video
            timestamp = int(time.time())
            filename = f"wan_video_{timestamp}.mp4"
            filepath = os.path.join(self.outputs_dir, filename)

            # Write MP4 with imageio
            writer = imageio.get_writer(
                filepath,
                fps=fps,
                codec="libx264",
                quality=9,
                macro_block_size=1
            )
            for frame in frames:
                import numpy as np
                if isinstance(frame, Image.Image):
                    frame_np = np.array(frame)
                else:
                    frame_np = frame
                writer.append_data(frame_np)
            writer.close()

            total_elapsed = round(time.time() - start_time, 2)
            logger.info(f"Vídeo generado exitosamente en {total_elapsed}s: {filepath}")

            meta = {
                "filename": filename,
                "filepath": filepath,
                "prompt": prompt,
                "negative_prompt": negative_prompt,
                "model_id": model_id,
                "pipeline_type": pipeline_type,
                "width": width,
                "height": height,
                "num_frames": num_frames,
                "fps": fps,
                "duration_seconds": round(num_frames / fps, 2),
                "num_inference_steps": num_inference_steps,
                "guidance_scale": guidance_scale,
                "seed": seed,
                "total_render_time_s": total_elapsed,
                "created_at": time.strftime("%Y-%m-%d %H:%M:%S"),
            }
            meta_path = os.path.join(self.outputs_dir, f"wan_video_{timestamp}.json")
            with open(meta_path, "w", encoding="utf-8") as f:
                json.dump(meta, f, indent=2, ensure_ascii=False)

            comp_data = {
                "status": "completado",
                "progress_percent": 100.0,
                "video_url": f"/outputs/{filename}",
                "metadata": meta,
                "message": f"¡Vídeo generado con éxito en {total_elapsed}s!"
            }
            self.last_progress_data = comp_data
            self.last_result = meta
            if progress_callback:
                progress_callback(comp_data)

            return meta

        except Exception as e:
            logger.error(f"Error durante la generación de vídeo: {e}", exc_info=True)
            err_data = {
                "status": "error",
                "error_message": str(e),
                "message": f"Error: {e}"
            }
            self.last_progress_data = err_data
            self.last_error = str(e)
            if progress_callback:
                progress_callback(err_data)
            raise e
        finally:
            with self.lock:
                self.is_generating = False
            self.free_memory()
