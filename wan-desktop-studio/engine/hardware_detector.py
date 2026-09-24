"""Hardware Detector and RTX 4060 Optimizer
Inspects NVIDIA GPU, CPU, RAM, and Disk space to configure Wan 2.1 inference.
"""

import os
import shutil
import subprocess
import json
import logging
from typing import Dict, Any, Optional

logger = logging.getLogger("HardwareDetector")

def get_drive_info(drive_letter: str) -> Dict[str, Any]:
    try:
        path = f"{drive_letter}:\\"
        if os.path.exists(path):
            total, used, free = shutil.disk_usage(path)
            return {
                "drive": drive_letter,
                "exists": True,
                "total_gb": round(total / (1024 ** 3), 2),
                "used_gb": round(used / (1024 ** 3), 2),
                "free_gb": round(free / (1024 ** 3), 2),
            }
    except Exception as e:
        logger.warning(f"Error checking drive {drive_letter}: {e}")
    return {"drive": drive_letter, "exists": False, "total_gb": 0, "used_gb": 0, "free_gb": 0}

def get_recommended_model_dir() -> str:
    # Check drive D: first (usually has the most space for high capacity datasets/models)
    d_info = get_drive_info("D")
    if d_info.get("exists") and d_info.get("free_gb", 0) > 30.0:
        recommended = os.path.join("D:\\", "WanVideoStudio_Models")
    else:
        # Fallback to C:\ or user home
        user_home = os.path.expanduser("~")
        recommended = os.path.join(user_home, "WanVideoStudio_Models")
    
    os.makedirs(recommended, exist_ok=True)
    
    # Direct all Hugging Face caches to drive D: to protect drive C:
    hf_cache_dir = os.path.join(recommended, "hf_cache")
    os.makedirs(hf_cache_dir, exist_ok=True)
    os.environ["HF_HOME"] = hf_cache_dir
    os.environ["HUGGINGFACE_HUB_CACHE"] = hf_cache_dir
    
    return recommended

def get_nvidia_smi_telemetry() -> Dict[str, Any]:
    """Query nvidia-smi for accurate real-time metrics on Windows."""
    try:
        cmd = [
            "nvidia-smi",
            "--query-gpu=name,memory.total,memory.used,memory.free,temperature.gpu,utilization.gpu",
            "--format=csv,noheader,nounits"
        ]
        result = subprocess.run(cmd, capture_output=True, text=True, check=True)
        lines = result.stdout.strip().split("\n")
        if lines and lines[0]:
            parts = [p.strip() for p in lines[0].split(",")]
            if len(parts) >= 6:
                name = parts[0]
                total_mb = float(parts[1])
                used_mb = float(parts[2])
                free_mb = float(parts[3])
                temp_c = float(parts[4])
                util_gpu = float(parts[5])
                return {
                    "available": True,
                    "name": name,
                    "vram_total_mb": total_mb,
                    "vram_used_mb": used_mb,
                    "vram_free_mb": free_mb,
                    "vram_used_percent": round((used_mb / total_mb) * 100, 1) if total_mb > 0 else 0,
                    "temperature_c": temp_c,
                    "utilization_gpu_percent": util_gpu,
                    "is_rtx_4060": "4060" in name,
                }
    except Exception as e:
        logger.debug(f"nvidia-smi error: {e}")
    return {"available": False}

def get_system_telemetry() -> Dict[str, Any]:
    """Return consolidated telemetry for GPU, CPU, RAM and Storage."""
    # RAM and CPU
    ram_info = {"total_gb": 0, "available_gb": 0, "used_percent": 0}
    cpu_info = {"name": "CPU", "cores": 0, "threads": 0, "percent": 0}
    try:
        import psutil
        vm = psutil.virtual_memory()
        ram_info = {
            "total_gb": round(vm.total / (1024 ** 3), 2),
            "available_gb": round(vm.available / (1024 ** 3), 2),
            "used_percent": vm.percent
        }
        cpu_info = {
            "cores": psutil.cpu_count(logical=False) or 8,
            "threads": psutil.cpu_count(logical=True) or 16,
            "percent": psutil.cpu_percent(interval=None)
        }
    except Exception:
        pass

    # GPU
    gpu_info = get_nvidia_smi_telemetry()

    # Drives
    drives = {
        "C": get_drive_info("C"),
        "D": get_drive_info("D"),
    }

    # Recommended settings specifically optimized for RTX 4060 8GB
    optimization_profile = {
        "device": "cuda",
        "cpu_offload_required": True,
        "vae_tiling": True,
        "vae_slicing": True,
        "allow_tf32": True,
        "recommended_dtype": "bfloat16",
        "recommended_resolution": "832x480",
        "recommended_steps_standard": 25,
        "recommended_steps_distilled": 6,
        "recommended_frames": 49,
        "default_model_dir": get_recommended_model_dir()
    }

    return {
        "gpu": gpu_info,
        "ram": ram_info,
        "cpu": cpu_info,
        "drives": drives,
        "optimization_profile": optimization_profile
    }

if __name__ == "__main__":
    data = get_system_telemetry()
    print(json.dumps(data, indent=2))
