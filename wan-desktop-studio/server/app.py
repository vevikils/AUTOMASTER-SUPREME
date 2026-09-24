"""FastAPI Backend Server and WebSocket Streamer for Wan 2.1 Video Studio
Bridges the GUI with the RTX 4060 Engine, Model Hub, and Hardware Monitor.
"""

import warnings
warnings.filterwarnings("ignore", category=FutureWarning)

import os
import sys
import json
import asyncio
import logging
from typing import Dict, Any, Optional, List
from fastapi import FastAPI, WebSocket, WebSocketDisconnect, HTTPException, BackgroundTasks, UploadFile, File
from fastapi.staticfiles import StaticFiles
from fastapi.middleware.cors import CORSMiddleware
from pydantic import BaseModel

# Add project root to sys.path
BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
if BASE_DIR not in sys.path:
    sys.path.insert(0, BASE_DIR)

from engine.hardware_detector import get_system_telemetry, get_recommended_model_dir
from engine.model_hub import ModelHub
from engine.wan_engine import WanEngine
from engine.prompt_enhancer import enhance_user_prompt

logging.basicConfig(level=logging.INFO, format="%(asctime)s [%(levelname)s] %(name)s: %(message)s")
logger = logging.getLogger("WanServer")

# Configuration paths
DEFAULT_MODELS_DIR = get_recommended_model_dir()
OUTPUTS_DIR = os.path.join(BASE_DIR, "outputs")
UI_DIR = os.path.join(BASE_DIR, "ui")

os.makedirs(DEFAULT_MODELS_DIR, exist_ok=True)
os.makedirs(OUTPUTS_DIR, exist_ok=True)
os.makedirs(UI_DIR, exist_ok=True)

# Initialize engines
model_hub = ModelHub(DEFAULT_MODELS_DIR)
wan_engine = WanEngine(DEFAULT_MODELS_DIR, OUTPUTS_DIR, model_hub=model_hub)

app = FastAPI(title="Wan 2.1 Video Studio", version="1.0.0")

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# Active WebSocket connections & Main Event Loop
connected_websockets: List[WebSocket] = []
main_event_loop: Optional[asyncio.AbstractEventLoop] = None

@app.on_event("startup")
async def on_startup():
    global main_event_loop
    main_event_loop = asyncio.get_running_loop()
    logger.info("Main asyncio loop registrado exitosamente para WebSocket broadcasts.")

async def broadcast_ws(data: Dict[str, Any]):
    dead_sockets = []
    for ws in list(connected_websockets):
        try:
            await ws.send_text(json.dumps(data))
        except Exception:
            dead_sockets.append(ws)
    for ws in dead_sockets:
        if ws in connected_websockets:
            connected_websockets.remove(ws)

def sync_ws_broadcast(data: Dict[str, Any]):
    """Thread-safe WebSocket broadcaster using the main application loop."""
    global main_event_loop
    if main_event_loop is not None and main_event_loop.is_running():
        try:
            asyncio.run_coroutine_threadsafe(broadcast_ws(data), main_event_loop)
        except Exception as e:
            logger.warning(f"WS broadcast error: {e}")

# Pydantic Request Models
class GenerateRequest(BaseModel):
    prompt: str
    negative_prompt: str = "low quality, distorted, watermark, blurry, deformed limbs, flickering artifacts"
    image_url: Optional[str] = None
    model_id: str = "Wan-AI/Wan2.1-T2V-1.3B-Diffusers"
    pipeline_type: str = "t2v"
    height: int = 480
    width: int = 832
    num_frames: int = 49
    fps: int = 16
    num_inference_steps: int = 25
    guidance_scale: float = 5.0
    seed: Optional[int] = None
    quantization: Optional[str] = None  # None, "4bit", "8bit"

class CivitaiDownloadRequest(BaseModel):
    model_id_or_url: str

class HFDownloadRequest(BaseModel):
    repo_id: str

class SettingsUpdateRequest(BaseModel):
    models_dir: str

class PromptEnhanceRequest(BaseModel):
    prompt: str
    style: str = "cinematic"  # cinematic, anime, photorealistic, scifi, drone

# -------------------------------------------------------------
# REST ENDPOINTS
# -------------------------------------------------------------

@app.get("/api/system/status")
def get_status():
    return get_system_telemetry()

@app.get("/api/generate/status")
def get_generation_status():
    prog = getattr(wan_engine, "last_progress_data", None)
    is_gen = wan_engine.is_generating
    err = getattr(wan_engine, "last_error", None)
    if prog and prog.get("status") == "error":
        is_gen = False
        if not err:
            err = prog.get("error_message") or prog.get("message")
    return {
        "is_generating": is_gen,
        "progress": prog,
        "result": getattr(wan_engine, "last_result", None),
        "error": err
    }

@app.get("/api/models/list")
def list_models():
    return model_hub.list_models()

@app.post("/api/models/download/huggingface")
def download_hf(req: HFDownloadRequest):
    def callback(info):
        sync_ws_broadcast({"type": "download_progress", "data": info})
    task_id = model_hub.start_huggingface_download(req.repo_id, callback)
    return {"status": "started", "task_id": task_id}

@app.post("/api/models/download/civitai")
def download_civitai(req: CivitaiDownloadRequest):
    def callback(info):
        sync_ws_broadcast({"type": "download_progress", "data": info})
    task_id = model_hub.start_civitai_download(req.model_id_or_url, callback)
    return {"status": "started", "task_id": task_id}

@app.get("/api/models/download/status/{task_id}")
def download_status(task_id: str):
    status = model_hub.get_download_status(task_id)
    if not status:
        raise HTTPException(status_code=404, detail="Tarea de descarga no encontrada")
    return status

@app.post("/api/settings/update")
def update_settings(req: SettingsUpdateRequest):
    if not os.path.exists(req.models_dir):
        try:
            os.makedirs(req.models_dir, exist_ok=True)
        except Exception as e:
            raise HTTPException(status_code=400, detail=f"No se puede crear la carpeta: {e}")
    model_hub.set_models_dir(req.models_dir)
    wan_engine.models_dir = os.path.abspath(req.models_dir)
    return {"status": "ok", "models_dir": req.models_dir}

@app.post("/api/prompt/enhance")
def enhance_prompt(req: PromptEnhanceRequest):
    try:
        return enhance_user_prompt(req.prompt, req.style)
    except Exception as e:
        logger.error(f"Error enhancing prompt: {e}")
        return {
            "enhanced_prompt": req.prompt,
            "suggested_negative": "low quality, blurry, distorted, watermark"
        }

@app.post("/api/upload/image")
async def upload_image(file: UploadFile = File(...)):
    """Upload input image for Image-to-Video generation."""
    upload_dir = os.path.join(OUTPUTS_DIR, "uploads")
    os.makedirs(upload_dir, exist_ok=True)
    file_path = os.path.join(upload_dir, file.filename)
    contents = await file.read()
    with open(file_path, "wb") as f:
        f.write(contents)
    return {"status": "ok", "image_url": f"/outputs/uploads/{file.filename}", "image_path": file_path}

@app.post("/api/generate/video")
def generate_video(req: GenerateRequest, background_tasks: BackgroundTasks):
    if wan_engine.is_generating:
        raise HTTPException(status_code=409, detail="Ya hay una generación de vídeo en curso.")

    def _task():
        def _prog(event):
            sync_ws_broadcast({"type": "generation_progress", "data": event})

        try:
            image_local_path = None
            if req.image_url:
                rel = req.image_url.replace("/outputs/", "")
                image_local_path = os.path.join(OUTPUTS_DIR, rel)

            result = wan_engine.generate_video(
                prompt=req.prompt,
                negative_prompt=req.negative_prompt,
                image_path=image_local_path,
                model_id=req.model_id,
                pipeline_type=req.pipeline_type,
                height=req.height,
                width=req.width,
                num_frames=req.num_frames,
                fps=req.fps,
                num_inference_steps=req.num_inference_steps,
                guidance_scale=req.guidance_scale,
                seed=req.seed,
                quantization=req.quantization,
                progress_callback=_prog
            )
            sync_ws_broadcast({"type": "generation_complete", "data": result})
        except Exception as e:
            logger.error(f"Error en tarea de generación: {e}")
            sync_ws_broadcast({"type": "generation_error", "data": {"error": str(e)}})

    background_tasks.add_task(_task)
    return {"status": "queued", "message": "Generación de vídeo iniciada en segundo plano."}

@app.get("/api/gallery")
def get_gallery():
    """List all previously generated videos and their metadata."""
    videos = []
    try:
        for f in os.listdir(OUTPUTS_DIR):
            if f.endswith(".mp4"):
                meta_file = f.replace(".mp4", ".json")
                meta_path = os.path.join(OUTPUTS_DIR, meta_file)
                video_info = {
                    "filename": f,
                    "url": f"/outputs/{f}",
                    "created_time": os.path.getmtime(os.path.join(OUTPUTS_DIR, f)),
                    "size_mb": round(os.path.getsize(os.path.join(OUTPUTS_DIR, f)) / (1024 * 1024), 2),
                    "metadata": None
                }
                if os.path.exists(meta_path):
                    try:
                        with open(meta_path, "r", encoding="utf-8") as mf:
                            video_info["metadata"] = json.load(mf)
                    except Exception:
                        pass
                videos.append(video_info)
        # Sort newest first
        videos.sort(key=lambda x: x["created_time"], reverse=True)
    except Exception as e:
        logger.error(f"Error listing gallery: {e}")
    return videos

# -------------------------------------------------------------
# WEBSOCKET STREAMING
# -------------------------------------------------------------

@app.websocket("/ws/stream")
async def websocket_endpoint(websocket: WebSocket):
    global main_event_loop
    if main_event_loop is None or not main_event_loop.is_running():
        try:
            main_event_loop = asyncio.get_running_loop()
        except Exception:
            pass
    await websocket.accept()
    connected_websockets.append(websocket)
    logger.info("Cliente WebSocket conectado.")
    try:
        while True:
            # Keepalive / listen for client ping or abort commands
            data = await websocket.receive_text()
            msg = json.loads(data)
            if msg.get("action") == "ping":
                await websocket.send_text(json.dumps({"type": "pong"}))
    except WebSocketDisconnect:
        if websocket in connected_websockets:
            connected_websockets.remove(websocket)
        logger.info("Cliente WebSocket desconectado.")
    except Exception as e:
        if websocket in connected_websockets:
            connected_websockets.remove(websocket)
        logger.debug(f"WebSocket error: {e}")

# Mount static folders for video files and frontend UI
app.mount("/outputs", StaticFiles(directory=OUTPUTS_DIR), name="outputs")
app.mount("/", StaticFiles(directory=UI_DIR, html=True), name="ui")
