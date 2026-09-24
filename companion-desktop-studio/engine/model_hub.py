"""Model Hub for Companion Studio
Discovers local safetensors checkpoints, manages models directory, and handles downloads from Hugging Face & Civitai.
"""

import os
import re
import json
import time
import uuid
import logging
import threading
from typing import Dict, Any, List, Optional, Callable
import urllib.request

logger = logging.getLogger("CompanionModelHub")

CURATED_IMAGE_MODELS = [
    {
        "id": "SG161222/Realistic_Vision_V5.1_noVAE",
        "name": "Realistic Vision V5.1 (SD 1.5)",
        "source": "huggingface",
        "type": "sd15",
        "style": "photorealistic",
        "size_gb": 2.1,
        "description": "Modelo fotorrealista de máxima calidad para retratos y selfies casuales. Ultrarrápido en RTX 4060 (~1.8s por imagen).",
        "recommended_for": ["Elena Morales", "Estilos Reales"],
        "default_steps": 22,
        "default_cfg": 6.5
    },
    {
        "id": "Lykon/dreamshaper-8",
        "name": "DreamShaper 8 (SD 1.5)",
        "source": "huggingface",
        "type": "sd15",
        "style": "cinematic",
        "size_gb": 2.1,
        "description": "Excelente versatilidad estética, iluminación cinematográfica, tonos cálidos y detalles nítidos.",
        "recommended_for": ["Sofía Rossi", "Estilos Cinematográficos"],
        "default_steps": 25,
        "default_cfg": 7.0
    },
    {
        "id": "runwayml/stable-diffusion-v1-5",
        "name": "Stable Diffusion 1.5 Base",
        "source": "huggingface",
        "type": "sd15",
        "style": "universal",
        "size_gb": 4.2,
        "description": "Modelo base de referencia open-source de Stability AI, ligero y altamente compatible.",
        "recommended_for": ["Cualquier personaje"],
        "default_steps": 25,
        "default_cfg": 7.0
    },
    {
        "id": "cagliostrolab/animagine-xl-3.1",
        "name": "Animagine XL 3.1 (SDXL)",
        "source": "huggingface",
        "type": "sdxl",
        "style": "anime",
        "size_gb": 6.6,
        "description": "El modelo anime de referencia en alta resolución (1024x1024). Estética visual Makoto Shinkai y colores vivos.",
        "recommended_for": ["Yuki Tanaka", "Estilos Anime"],
        "default_steps": 25,
        "default_cfg": 6.5
    }
]


def get_default_models_dir() -> str:
    """Select Drive D: if available to protect C:"""
    if os.path.exists("D:\\"):
        d_path = os.path.join("D:\\", "CompanionStudio_Models")
        try:
            os.makedirs(d_path, exist_ok=True)
            return d_path
        except Exception:
            pass
    fallback = os.path.join(os.path.expanduser("~"), "CompanionStudio_Models")
    os.makedirs(fallback, exist_ok=True)
    return fallback


class ModelHub:
    def __init__(self, models_dir: Optional[str] = None):
        self.models_dir = os.path.abspath(models_dir or get_default_models_dir())
        self.checkpoints_dir = os.path.join(self.models_dir, "checkpoints")
        os.makedirs(self.checkpoints_dir, exist_ok=True)
        
        self.download_tasks: Dict[str, Dict[str, Any]] = {}
        self.lock = threading.Lock()

    def set_models_dir(self, new_dir: str):
        with self.lock:
            self.models_dir = os.path.abspath(new_dir)
            self.checkpoints_dir = os.path.join(self.models_dir, "checkpoints")
            os.makedirs(self.checkpoints_dir, exist_ok=True)

    def scan_local_checkpoints(self) -> List[Dict[str, Any]]:
        """List all .safetensors files in checkpoints directory."""
        files = []
        try:
            if os.path.exists(self.checkpoints_dir):
                for f in os.listdir(self.checkpoints_dir):
                    if f.endswith(".safetensors") or f.endswith(".ckpt"):
                        full_path = os.path.join(self.checkpoints_dir, f)
                        size_mb = round(os.path.getsize(full_path) / (1024 * 1024), 1)
                        files.append({
                            "id": full_path,
                            "filename": f,
                            "name": os.path.splitext(f)[0].replace("_", " ").title(),
                            "path": full_path,
                            "size_mb": size_mb,
                            "size_gb": round(size_mb / 1024, 2),
                            "source": "local_safetensors",
                            "installed": True
                        })
        except Exception as e:
            logger.error(f"Error escaneando checkpoints locales: {e}")
        return files

    def list_models(self) -> Dict[str, Any]:
        """Return combined curated models + custom local checkpoints."""
        local_safetensors = self.scan_local_checkpoints()
        
        curated_status = []
        for m in CURATED_IMAGE_MODELS:
            folder_name = m["id"].replace("/", "--")
            local_path = os.path.join(self.models_dir, folder_name)
            is_installed = os.path.exists(local_path) and len(os.listdir(local_path)) > 0
            
            # Check if active download task exists
            task_info = None
            for t_id, t_data in self.download_tasks.items():
                if t_data.get("model_id") == m["id"]:
                    task_info = t_data
                    break

            m_copy = dict(m)
            m_copy["installed"] = is_installed
            m_copy["local_path"] = local_path if is_installed else None
            m_copy["download_info"] = task_info
            curated_status.append(m_copy)

        return {
            "models_dir": self.models_dir,
            "checkpoints_dir": self.checkpoints_dir,
            "curated": curated_status,
            "local_checkpoints": local_safetensors
        }

    def start_huggingface_download(self, repo_id: str, progress_callback: Optional[Callable] = None) -> str:
        task_id = f"hf_{uuid.uuid4().hex[:6]}"
        with self.lock:
            self.download_tasks[task_id] = {
                "task_id": task_id,
                "model_id": repo_id,
                "status": "iniciando",
                "progress_percent": 0.0,
                "message": "Preparando descarga desde Hugging Face..."
            }

        def _worker():
            try:
                from huggingface_hub import snapshot_download
                folder_name = repo_id.replace("/", "--")
                target_dir = os.path.join(self.models_dir, folder_name)
                os.makedirs(target_dir, exist_ok=True)

                def update_cb(pct, msg):
                    with self.lock:
                        self.download_tasks[task_id]["progress_percent"] = pct
                        self.download_tasks[task_id]["message"] = msg
                    if progress_callback:
                        progress_callback(self.download_tasks[task_id])

                update_cb(10.0, f"Descargando {repo_id} en disco D:...")
                snapshot_download(
                    repo_id=repo_id,
                    local_dir=target_dir,
                    resume_download=True
                )
                with self.lock:
                    self.download_tasks[task_id]["status"] = "completado"
                    self.download_tasks[task_id]["progress_percent"] = 100.0
                    self.download_tasks[task_id]["message"] = "Modelo descargado e instalado."
                if progress_callback:
                    progress_callback(self.download_tasks[task_id])
            except Exception as e:
                logger.error(f"Error descargando {repo_id}: {e}")
                with self.lock:
                    self.download_tasks[task_id]["status"] = "error"
                    self.download_tasks[task_id]["message"] = str(e)
                if progress_callback:
                    progress_callback(self.download_tasks[task_id])

        t = threading.Thread(target=_worker, daemon=True)
        t.start()
        return task_id

    def start_civitai_download(self, model_url_or_id: str, progress_callback: Optional[Callable] = None) -> str:
        task_id = f"civitai_{uuid.uuid4().hex[:6]}"
        with self.lock:
            self.download_tasks[task_id] = {
                "task_id": task_id,
                "model_id": model_url_or_id,
                "status": "iniciando",
                "progress_percent": 0.0,
                "message": "Conectando con Civitai..."
            }

        def _worker():
            try:
                # Extract model version id or direct download link
                url = model_url_or_id.strip()
                if not url.startswith("http"):
                    # Treat as Civitai modelId
                    url = f"https://civitai.com/api/download/models/{url}"

                # Generate clean filename
                filename = f"civitai_model_{int(time.time())}.safetensors"
                out_path = os.path.join(self.checkpoints_dir, filename)

                req = urllib.request.Request(
                    url,
                    headers={'User-Agent': 'CompanionStudio/1.0'}
                )

                with urllib.request.urlopen(req) as resp, open(out_path, 'wb') as out_f:
                    total_bytes = int(resp.info().get('Content-Length', 0))
                    downloaded = 0
                    start_t = time.time()

                    while True:
                        chunk = resp.read(1024 * 1024)  # 1MB
                        if not chunk:
                            break
                        out_f.write(chunk)
                        downloaded += len(chunk)

                        pct = round((downloaded / total_bytes * 100), 1) if total_bytes > 0 else 50.0
                        elapsed = max(time.time() - start_t, 0.1)
                        speed_mb = round((downloaded / (1024 * 1024)) / elapsed, 1)

                        with self.lock:
                            self.download_tasks[task_id]["status"] = "descargando"
                            self.download_tasks[task_id]["progress_percent"] = pct
                            self.download_tasks[task_id]["speed_mb_s"] = speed_mb
                            self.download_tasks[task_id]["downloaded_mb"] = round(downloaded / (1024*1024), 1)
                            self.download_tasks[task_id]["total_mb"] = round(total_bytes / (1024*1024), 1)
                            self.download_tasks[task_id]["message"] = f"Descargando de Civitai: {pct}% a {speed_mb} MB/s"

                        if progress_callback:
                            progress_callback(self.download_tasks[task_id])

                with self.lock:
                    self.download_tasks[task_id]["status"] = "completado"
                    self.download_tasks[task_id]["progress_percent"] = 100.0
                    self.download_tasks[task_id]["message"] = f"Descarga de Civitai guardada en {filename}."
                if progress_callback:
                    progress_callback(self.download_tasks[task_id])

            except Exception as e:
                logger.error(f"Error en descarga de Civitai: {e}")
                with self.lock:
                    self.download_tasks[task_id]["status"] = "error"
                    self.download_tasks[task_id]["message"] = str(e)
                if progress_callback:
                    progress_callback(self.download_tasks[task_id])

        t = threading.Thread(target=_worker, daemon=True)
        t.start()
        return task_id

    def get_download_status(self, task_id: str) -> Optional[Dict[str, Any]]:
        with self.lock:
            return self.download_tasks.get(task_id)
