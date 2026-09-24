"""Model Hub and Downloader for Hugging Face and Civitai
Manages download, discovery, verification, and live progress streaming of Wan 2.1 models.
"""

import os
import json
import time
import threading
import requests
import logging
from typing import Dict, Any, List, Optional, Callable
from urllib.parse import urlparse

logger = logging.getLogger("ModelHub")

CURATED_MODELS = [
    {
        "id": "Wan-AI/Wan2.1-T2V-1.3B-Diffusers",
        "name": "Wan 2.1 T2V 1.3B (Oficial)",
        "source": "huggingface",
        "type": "t2v",
        "description": "Modelo Text-to-Video ligero de 1.3B parámetros. Rápido pero con menor fidelidad anatómica.",
        "size_approx": "12.8 GB (Transformer + VAE + Text Encoder)",
        "recommended": False,
        "resolution_default": "832x480",
        "steps_default": 25,
        "is_distilled": False,
    },
    {
        "id": "lightx2v/Wan2.1-Distill-Models",
        "name": "Wan 2.1 Turbo Distilled (4 Pasos - 14B Cinema)",
        "source": "huggingface",
        "type": "t2v",
        "description": "Modelo 14B ultrarrápido destilado a solo 4 pasos. Calidad cinematográfica fotorrealista sin deformaciones.",
        "size_approx": "14.5 GB (FP8 / INT8)",
        "recommended": True,
        "resolution_default": "832x480",
        "steps_default": 4,
        "is_distilled": True,
    },
    {
        "id": "Wan-AI/Wan2.1-I2V-14B-480P-Diffusers",
        "name": "Wan 2.1 I2V 14B 480P",
        "source": "huggingface",
        "type": "i2v",
        "description": "Modelo Image-to-Video de alta fidelidad cinematográfica a 480p. Animación realista de imágenes estáticas.",
        "size_approx": "28 GB (ejecutable con CPU Offloading en 48GB RAM)",
        "recommended": False,
        "resolution_default": "832x480",
        "steps_default": 30,
        "is_distilled": False,
    },
    {
        "id": "Wan-AI/Wan2.1-T2V-14B-Diffusers",
        "name": "Wan 2.1 T2V 14B (Fidelidad Extrema)",
        "source": "huggingface",
        "type": "t2v",
        "description": "Modelo Text-to-Video de 14 Billones de parámetros para calidad ultra-detallada cinematográfica.",
        "size_approx": "28 GB",
        "recommended": False,
        "resolution_default": "832x480",
        "steps_default": 30,
        "is_distilled": False,
    }
]

class DownloadTracker:
    def __init__(self, task_id: str, name: str):
        self.task_id = task_id
        self.name = name
        self.status = "iniciando"  # iniciando, descargando, completado, error, cancelado
        self.total_bytes = 0
        self.downloaded_bytes = 0
        self.speed_mb_s = 0.0
        self.progress_percent = 0.0
        self.eta_seconds = 0
        self.current_file = ""
        self.error_message: Optional[str] = None
        self.start_time = time.time()
        self.last_update_time = time.time()
        self.last_bytes = 0
        self.cancel_requested = False

    def update(self, downloaded: int, total: int, current_file: str = ""):
        now = time.time()
        self.downloaded_bytes = downloaded
        self.total_bytes = total
        self.current_file = current_file
        if total > 0:
            self.progress_percent = round((downloaded / total) * 100, 1)

        dt = now - self.last_update_time
        if dt >= 0.5:
            delta_b = downloaded - self.last_bytes
            speed_bps = delta_b / dt
            self.speed_mb_s = round(speed_bps / (1024 * 1024), 2)
            self.last_update_time = now
            self.last_bytes = downloaded

            if speed_bps > 0 and total > downloaded:
                self.eta_seconds = int((total - downloaded) / speed_bps)
            else:
                self.eta_seconds = 0

    def to_dict(self) -> Dict[str, Any]:
        return {
            "task_id": self.task_id,
            "name": self.name,
            "status": self.status,
            "total_mb": round(self.total_bytes / (1024 * 1024), 1) if self.total_bytes else 0,
            "downloaded_mb": round(self.downloaded_bytes / (1024 * 1024), 1),
            "progress_percent": self.progress_percent,
            "speed_mb_s": self.speed_mb_s,
            "eta_seconds": self.eta_seconds,
            "current_file": self.current_file,
            "error_message": self.error_message,
        }

class ModelHub:
    def __init__(self, models_dir: str):
        self.models_dir = os.path.abspath(models_dir)
        os.makedirs(self.models_dir, exist_ok=True)
        self.active_downloads: Dict[str, DownloadTracker] = {}
        self.lock = threading.Lock()

    def set_models_dir(self, new_dir: str):
        with self.lock:
            self.models_dir = os.path.abspath(new_dir)
            os.makedirs(self.models_dir, exist_ok=True)

    def get_local_model_path(self, model_id: str) -> str:
        clean_name = model_id.replace("/", "--")
        return os.path.join(self.models_dir, clean_name)

    def is_model_installed(self, model_id: str) -> bool:
        """Check if essential model weights exist locally in models_dir or HF cache."""
        local_path = self.get_local_model_path(model_id)
        if os.path.exists(local_path):
            index_file = os.path.join(local_path, "model_index.json")
            vae_file = os.path.join(local_path, "vae", "diffusion_pytorch_model.safetensors")
            # If model_index.json exists and either vae or transformer exists, it's installed
            if os.path.exists(index_file) and (os.path.exists(vae_file) or os.path.exists(os.path.join(local_path, "transformer"))):
                return True
            # For single-file or distilled models without model_index.json:
            try:
                for f in os.listdir(local_path):
                    if f.endswith(".safetensors"):
                        sz = os.path.getsize(os.path.join(local_path, f))
                        # Completed model file >= 12 GB
                        if sz >= 12 * 1024 * 1024 * 1024:
                            return True
            except Exception:
                pass

        # Check huggingface cache
        hf_home = os.environ.get("HF_HOME", os.path.expanduser("~/.cache/huggingface/hub"))
        clean_name = model_id.replace("/", "--")
        repo_dir = os.path.join(hf_home, f"models--{clean_name}")
        if os.path.exists(repo_dir):
            snapshots_dir = os.path.join(repo_dir, "snapshots")
            if os.path.exists(snapshots_dir) and os.listdir(snapshots_dir):
                return True
        return False

    def list_models(self) -> Dict[str, Any]:
        """Return curated models plus any locally discovered models."""
        result = []
        for m in CURATED_MODELS:
            item = dict(m)
            item["installed"] = self.is_model_installed(m["id"])
            item["local_path"] = self.get_local_model_path(m["id"])
            if m["id"] in self.active_downloads:
                item["download_info"] = self.active_downloads[m["id"]].to_dict()
            else:
                item["download_info"] = None
            result.append(item)
        
        # Scan custom files in models_dir
        try:
            custom_files = []
            for root, _, files in os.walk(self.models_dir):
                for f in files:
                    if f.endswith((".safetensors", ".ckpt", ".gguf", ".pt")):
                        full_p = os.path.join(root, f)
                        sz_mb = round(os.path.getsize(full_p) / (1024 * 1024), 1)
                        custom_files.append({
                            "name": f,
                            "path": full_p,
                            "size_mb": sz_mb,
                            "is_lora": "lora" in f.lower(),
                        })
            return {"curated": result, "custom_files": custom_files, "models_dir": self.models_dir}
        except Exception as e:
            logger.error(f"Error escaneando archivos locales: {e}")
            return {"curated": result, "custom_files": [], "models_dir": self.models_dir}

    def download_huggingface_repo_with_progress(
        self,
        repo_id: str,
        progress_callback: Optional[Callable[[Dict[str, Any]], None]] = None
    ) -> str:
        """Download model files directly with live chunked progress tracking into models_dir."""
        target_dir = self.get_local_model_path(repo_id)
        os.makedirs(target_dir, exist_ok=True)

        from huggingface_hub import HfApi
        api = HfApi()

        logger.info(f"Obteniendo metadatos de archivos para {repo_id}...")
        info = api.model_info(repo_id, files_metadata=True)

        # Filter out documentation/asset images to save bandwidth & time
        files_to_download = []
        for s in info.siblings:
            rfn = s.rfilename
            # Ignore assets, examples, and git internals
            if rfn.startswith(("assets/", "examples/", ".git")) or rfn.endswith((".png", ".jpg", ".jpeg", ".md", ".gitattributes")):
                continue
            files_to_download.append((rfn, s.size or 0))

        # SPECIAL HANDLING for multi-model repos like LightX2V Distill Models:
        # Download ONLY the recommended 4-step 14B model (14.5 GB) instead of the entire 200+ GB multi-model repo!
        if "lightx2v" in repo_id.lower() or "distill" in repo_id.lower():
            recommended_model_file = "wan2.1_t2v_14b_scaled_fp8_e4m3_lightx2v_4step_comfyui.safetensors"
            filtered = [f for f in files_to_download if f[0] == recommended_model_file or f[0] == "config.json"]
            if filtered:
                files_to_download = filtered
                logger.info(f"Repositorio multi-modelo detectado: filtrando a {recommended_model_file} (14.5 GB) para evitar descargar 200+ GB.")

        total_bytes = sum(size for _, size in files_to_download)
        logger.info(f"Total de archivos a descargar: {len(files_to_download)} ({round(total_bytes / (1024**3), 2)} GB)")

        # Calculate already downloaded bytes (for instant resume support)
        downloaded_bytes = 0
        for rfn, size in files_to_download:
            dest_file = os.path.join(target_dir, rfn.replace("/", os.sep))
            if os.path.exists(dest_file):
                local_sz = os.path.getsize(dest_file)
                if local_sz == size:
                    downloaded_bytes += size
                elif local_sz > size:
                    # corrupted, remove
                    try:
                        os.remove(dest_file)
                    except Exception:
                        pass

        last_update_time = time.time()
        last_bytes = downloaded_bytes
        speed_mb_s = 0.0

        for rfn, size in files_to_download:
            dest_file = os.path.join(target_dir, rfn.replace("/", os.sep))
            file_name = os.path.basename(rfn)
            os.makedirs(os.path.dirname(dest_file), exist_ok=True)

            # If already completely downloaded, skip
            if os.path.exists(dest_file) and os.path.getsize(dest_file) == size:
                continue

            file_url = f"https://huggingface.co/{repo_id}/resolve/main/{rfn}"
            file_success = False
            max_retries = 5
            for attempt in range(max_retries):
                try:
                    resume_header = {}
                    mode = "wb"
                    existing_bytes = 0
                    if os.path.exists(dest_file):
                        existing_bytes = os.path.getsize(dest_file)
                        if 0 < existing_bytes < size:
                            resume_header = {"Range": f"bytes={existing_bytes}-"}
                            mode = "ab"

                    with requests.get(file_url, headers=resume_header, stream=True, timeout=(15, 60)) as r:
                        # 416 means Range Not Satisfiable (file already fully downloaded)
                        if r.status_code == 416 or (os.path.exists(dest_file) and os.path.getsize(dest_file) >= size):
                            file_success = True
                            break
                        r.raise_for_status()
                        with open(dest_file, mode) as f:
                            for chunk in r.iter_content(chunk_size=1024 * 1024):  # 1 MB chunks
                                if chunk:
                                    f.write(chunk)
                                    downloaded_bytes += len(chunk)

                                    now = time.time()
                                    dt = now - last_update_time
                                    if dt >= 0.5:
                                        delta_b = downloaded_bytes - last_bytes
                                        speed_mb_s = round((delta_b / dt) / (1024 * 1024), 2)
                                        last_update_time = now
                                        last_bytes = downloaded_bytes

                                        remaining_b = max(0, total_bytes - downloaded_bytes)
                                        eta_s = int(remaining_b / (speed_mb_s * 1024 * 1024)) if speed_mb_s > 0 else 0
                                        pct = round((downloaded_bytes / total_bytes) * 100, 1) if total_bytes > 0 else 0

                                        if progress_callback:
                                            progress_callback({
                                                "status": "descargando_modelo",
                                                "current_file": file_name,
                                                "downloaded_bytes": downloaded_bytes,
                                                "total_bytes": total_bytes,
                                                "downloaded_mb": round(downloaded_bytes / (1024 * 1024), 1),
                                                "total_mb": round(total_bytes / (1024 * 1024), 1),
                                                "progress_percent": pct,
                                                "speed_mb_s": speed_mb_s,
                                                "eta_seconds": eta_s,
                                                "message": f"Descargando {file_name} ({round(downloaded_bytes / (1024**3), 2)} / {round(total_bytes / (1024**3), 2)} GB a {speed_mb_s} MB/s)..."
                                            })
                    file_success = True
                    break
                except Exception as ex:
                    logger.warning(f"Intento {attempt+1}/{max_retries} fallido para {file_name} ({ex}). Reconectando en 3s...")
                    time.sleep(3)

            if not file_success:
                raise IOError(f"No se pudo completar la descarga de {file_name} tras {max_retries} reintentos.")

        if progress_callback:
            progress_callback({
                "status": "descargando_modelo",
                "progress_percent": 100.0,
                "downloaded_mb": round(total_bytes / (1024 * 1024), 1),
                "total_mb": round(total_bytes / (1024 * 1024), 1),
                "message": "¡Descarga de archivos completada exitosamente!"
            })

        logger.info(f"Modelo {repo_id} descargado completamente en {target_dir}")
        return target_dir

    def start_huggingface_download(self, repo_id: str, callback: Optional[Callable] = None) -> str:
        task_id = repo_id
        with self.lock:
            if task_id in self.active_downloads and self.active_downloads[task_id].status == "descargando":
                return task_id
            tracker = DownloadTracker(task_id, repo_id)
            tracker.status = "descargando"
            self.active_downloads[task_id] = tracker

        def _worker():
            try:
                def _prog(event):
                    tracker.update(
                        downloaded=event.get("downloaded_bytes", 0),
                        total=event.get("total_bytes", 0),
                        current_file=event.get("current_file", "")
                    )
                    tracker.speed_mb_s = event.get("speed_mb_s", 0.0)
                    tracker.eta_seconds = event.get("eta_seconds", 0)
                    tracker.progress_percent = event.get("progress_percent", 0.0)
                    if callback:
                        callback(tracker.to_dict())

                self.download_huggingface_repo_with_progress(repo_id, progress_callback=_prog)
                tracker.status = "completado"
                tracker.progress_percent = 100.0
            except Exception as e:
                logger.error(f"Error descargando {repo_id}: {e}", exc_info=True)
                tracker.status = "error"
                tracker.error_message = str(e)
            finally:
                if callback:
                    callback(tracker.to_dict())

        thread = threading.Thread(target=_worker, daemon=True)
        thread.start()
        return task_id

    def start_civitai_download(self, model_id_or_url: str, callback: Optional[Callable] = None) -> str:
        task_id = f"civitai_{int(time.time())}"
        tracker = DownloadTracker(task_id, f"Civitai: {model_id_or_url}")
        tracker.status = "iniciando"
        with self.lock:
            self.active_downloads[task_id] = tracker

        def _worker():
            try:
                download_url = model_id_or_url
                target_filename = "civitai_model.safetensors"

                if model_id_or_url.isdigit():
                    api_url = f"https://civitai.com/api/v1/models/{model_id_or_url}"
                    resp = requests.get(api_url, timeout=15)
                    resp.raise_for_status()
                    data = resp.json()
                    model_versions = data.get("modelVersions", [])
                    if not model_versions:
                        raise ValueError("No se encontraron versiones para este modelo en Civitai.")
                    latest_version = model_versions[0]
                    files = latest_version.get("files", [])
                    primary_file = next((f for f in files if f.get("primary")), files[0] if files else None)
                    if not primary_file:
                        raise ValueError("No se encontró archivo descargable para este modelo.")
                    download_url = primary_file.get("downloadUrl")
                    target_filename = primary_file.get("name", f"civitai_{model_id_or_url}.safetensors")
                    tracker.name = f"{data.get('name', 'Civitai Model')} ({target_filename})"

                elif "civitai.com" in model_id_or_url:
                    parsed = urlparse(model_id_or_url)
                    path_parts = parsed.path.strip("/").split("/")
                    if "models" in path_parts:
                        idx = path_parts.index("models")
                        if idx + 1 < len(path_parts) and path_parts[idx + 1].isdigit():
                            mid = path_parts[idx + 1]
                            return self.start_civitai_download(mid, callback)
                    download_url = model_id_or_url

                tracker.status = "descargando"
                save_dir = os.path.join(self.models_dir, "Civitai")
                os.makedirs(save_dir, exist_ok=True)
                save_path = os.path.join(save_dir, target_filename)

                with requests.get(download_url, stream=True, timeout=30) as r:
                    r.raise_for_status()
                    cd = r.headers.get("Content-Disposition", "")
                    if "filename=" in cd:
                        fn = cd.split("filename=")[-1].strip('"\'; ')
                        if fn:
                            target_filename = fn
                            save_path = os.path.join(save_dir, target_filename)

                    total_size = int(r.headers.get("content-length", 0))
                    tracker.total_bytes = total_size
                    downloaded = 0

                    with open(save_path, "wb") as f:
                        for chunk in r.iter_content(chunk_size=1024 * 1024):
                            if tracker.cancel_requested:
                                tracker.status = "cancelado"
                                f.close()
                                if os.path.exists(save_path):
                                    os.remove(save_path)
                                return
                            if chunk:
                                f.write(chunk)
                                downloaded += len(chunk)
                                tracker.update(downloaded, total_size, target_filename)
                                if callback:
                                    callback(tracker.to_dict())

                tracker.status = "completado"
                tracker.progress_percent = 100.0
            except Exception as e:
                logger.error(f"Error descargando de Civitai: {e}")
                tracker.status = "error"
                tracker.error_message = str(e)
            finally:
                if callback:
                    callback(tracker.to_dict())

        thread = threading.Thread(target=_worker, daemon=True)
        thread.start()
        return task_id

    def get_download_status(self, task_id: str) -> Optional[Dict[str, Any]]:
        with self.lock:
            tracker = self.active_downloads.get(task_id)
            if tracker:
                return tracker.to_dict()
        return None
