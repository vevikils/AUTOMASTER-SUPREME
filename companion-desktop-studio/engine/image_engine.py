"""Image Generation Engine for Companion Studio
Optimized for NVIDIA GeForce RTX 4060 (8GB VRAM) using Diffusers, PyTorch FP16, and SDPA.
Supports both Hugging Face repos and local .safetensors checkpoints.
"""

import os
import gc
import time
import json
import logging
import threading
from typing import Dict, Any, Optional, Callable
from PIL import Image

logger = logging.getLogger("CompanionImageEngine")

DEFAULT_MODEL = "SG161222/Realistic_Vision_V5.1_noVAE"


class ImageEngine:
    def __init__(self, models_dir: str, outputs_dir: str):
        self.models_dir = os.path.abspath(models_dir)
        self.outputs_dir = os.path.abspath(outputs_dir)
        os.makedirs(self.outputs_dir, exist_ok=True)

        self.current_pipeline = None
        self.current_model_id = None
        self.is_generating = False
        self.last_progress_data = None
        self.lock = threading.Lock()

        self._configure_optimizations()

    def _configure_optimizations(self):
        try:
            import torch
            if torch.cuda.is_available():
                torch.backends.cuda.matmul.allow_tf32 = True
                torch.backends.cudnn.allow_tf32 = True
                torch.backends.cudnn.benchmark = True
                logger.info("Optimizaciones TF32 y cuDNN benchmark activas en RTX 4060.")
        except Exception as e:
            logger.warning(f"Error configurando optimizaciones CUDA: {e}")

    def free_memory(self):
        try:
            import torch
            gc.collect()
            if torch.cuda.is_available():
                torch.cuda.empty_cache()
                torch.cuda.ipc_collect()
        except Exception:
            pass

    def load_model(self, model_id_or_path: str = DEFAULT_MODEL, status_callback: Optional[Callable] = None):
        """Loads a model (HF repo or local .safetensors) into GPU with RTX 4060 optimizations."""
        import torch
        from diffusers import AutoPipelineForText2Image, StableDiffusionPipeline

        with self.lock:
            if self.current_pipeline is not None and self.current_model_id == model_id_or_path:
                logger.info(f"Reutilizando modelo ya cargado: {model_id_or_path}")
                return self.current_pipeline

            if status_callback:
                status_callback({
                    "status": "cargando_modelo",
                    "progress_percent": 10.0,
                    "message": f"Cargando modelo {os.path.basename(model_id_or_path)} en RTX 4060..."
                })

            # Unload prior pipeline to keep 8GB VRAM completely free
            if self.current_pipeline is not None:
                del self.current_pipeline
                self.current_pipeline = None
                self.free_memory()

            dtype = torch.float16
            pipe = None

            # Check if it's a local .safetensors file
            if os.path.isfile(model_id_or_path) and model_id_or_path.endswith((".safetensors", ".ckpt")):
                logger.info(f"Cargando checkpoint individual .safetensors: {model_id_or_path}")
                pipe = StableDiffusionPipeline.from_single_file(
                    model_id_or_path,
                    torch_dtype=dtype,
                    use_safetensors=True
                )
            else:
                # Check if it's stored in local models_dir
                folder_name = model_id_or_path.replace("/", "--")
                local_dir = os.path.join(self.models_dir, folder_name)
                load_target = local_dir if os.path.exists(local_dir) else model_id_or_path

                logger.info(f"Cargando modelo Diffusers desde {load_target}...")
                pipe = AutoPipelineForText2Image.from_pretrained(
                    load_target,
                    torch_dtype=dtype,
                    safety_checker=None
                )

            # Move to RTX 4060
            pipe.to("cuda")

            # Enable memory optimizations
            if hasattr(pipe, "enable_attention_slicing"):
                pipe.enable_attention_slicing(slice_size="auto")
            if hasattr(pipe, "enable_vae_tiling"):
                try:
                    pipe.enable_vae_tiling()
                except Exception:
                    pass

            self.current_pipeline = pipe
            self.current_model_id = model_id_or_path

            if status_callback:
                status_callback({
                    "status": "modelo_listo",
                    "progress_percent": 100.0,
                    "message": "Modelo listo para generar en la GPU."
                })

            return pipe

    def generate_companion_image(
        self,
        prompt: str,
        negative_prompt: str = "ugly, deformed, disfigured, poor details, bad anatomy, bad eyes, extra limbs, low quality, blurry, cartoon, 3d render",
        persona_id: str = "default",
        model_id: str = DEFAULT_MODEL,
        width: int = 512,
        height: int = 768,
        num_inference_steps: int = 22,
        guidance_scale: float = 6.5,
        seed: Optional[int] = None,
        progress_callback: Optional[Callable] = None
    ) -> Dict[str, Any]:
        """Runs fast portrait/selfie generation on RTX 4060 and returns image info."""
        import torch

        with self.lock:
            self.is_generating = True

        start_time = time.time()
        try:
            pipe = self.load_model(model_id, status_callback=progress_callback)

            # Seed
            if seed is None or seed == -1:
                seed = int(torch.randint(0, 2**32 - 1, (1,)).item())
            generator = torch.Generator(device="cuda").manual_seed(seed)

            # Callback for UI progress
            def _step_callback(p, step, timestep, callback_kwargs):
                progress_pct = round(((step + 1) / num_inference_steps) * 100, 1)
                vram_gb = 0.0
                try:
                    vram_gb = round(torch.cuda.memory_allocated() / (1024**3), 2)
                except Exception:
                    pass

                prog_data = {
                    "status": "generando",
                    "step": step + 1,
                    "total_steps": num_inference_steps,
                    "progress_percent": progress_pct,
                    "vram_gb": vram_gb,
                    "message": f"Renderizando selfie ({step + 1}/{num_inference_steps})..."
                }
                self.last_progress_data = prog_data
                if progress_callback:
                    progress_callback(prog_data)
                return callback_kwargs

            if progress_callback:
                self.last_progress_data = {
                    "status": "iniciando",
                    "step": 0,
                    "total_steps": num_inference_steps,
                    "progress_percent": 5.0,
                    "message": "Iniciando renderizado en núcleos Tensor..."
                }
                progress_callback(self.last_progress_data)

            # Run diffusion
            logger.info(f"Generando imagen ({width}x{height}, {num_inference_steps} steps) para {persona_id}...")
            result = pipe(
                prompt=prompt,
                negative_prompt=negative_prompt,
                width=width,
                height=height,
                num_inference_steps=num_inference_steps,
                guidance_scale=guidance_scale,
                generator=generator,
                callback_on_step_end=_step_callback
            )

            image = result.images[0]

            # Save in persona's folder
            persona_out_dir = os.path.join(self.outputs_dir, persona_id)
            os.makedirs(persona_out_dir, exist_ok=True)

            timestamp = int(time.time())
            filename = f"selfie_{timestamp}.jpg"
            filepath = os.path.join(persona_out_dir, filename)
            image.save(filepath, quality=95)

            elapsed = round(time.time() - start_time, 2)
            logger.info(f"Imagen generada en {elapsed}s: {filepath}")

            meta = {
                "persona_id": persona_id,
                "filename": filename,
                "filepath": filepath,
                "url": f"/outputs/{persona_id}/{filename}",
                "prompt": prompt,
                "negative_prompt": negative_prompt,
                "width": width,
                "height": height,
                "steps": num_inference_steps,
                "guidance_scale": guidance_scale,
                "seed": seed,
                "model_id": model_id,
                "render_time_s": elapsed,
                "created_at": time.strftime("%Y-%m-%d %H:%M:%S")
            }

            meta_file = filepath.replace(".jpg", ".json")
            with open(meta_file, "w", encoding="utf-8") as f:
                json.dump(meta, f, indent=2, ensure_ascii=False)

            if progress_callback:
                comp_data = {
                    "status": "completado",
                    "progress_percent": 100.0,
                    "image_url": meta["url"],
                    "metadata": meta,
                    "message": f"¡Foto generada en {elapsed}s!"
                }
                self.last_progress_data = comp_data
                progress_callback(comp_data)

            return meta

        except Exception as e:
            logger.error(f"Error generando imagen: {e}", exc_info=True)
            if progress_callback:
                progress_callback({
                    "status": "error",
                    "message": f"Error: {e}"
                })
            raise e
        finally:
            with self.lock:
                self.is_generating = False
            self.free_memory()
