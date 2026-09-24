"""FastAPI Server for Companion Studio
Local private API for personas, conversational chat, RTX 4060 image generation, and Model Hub.
Includes automated contextual photo album synthesis for created characters.
"""

import os
import sys
import json
import time
import uuid
import asyncio
import logging
from typing import Dict, Any, List, Optional
from fastapi import FastAPI, WebSocket, WebSocketDisconnect, HTTPException, BackgroundTasks
from fastapi.staticfiles import StaticFiles
from fastapi.middleware.cors import CORSMiddleware
from pydantic import BaseModel

# Add project root to sys.path
BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
if BASE_DIR not in sys.path:
    sys.path.insert(0, BASE_DIR)

from engine.persona_manager import PersonaManager
from engine.model_hub import ModelHub, get_default_models_dir
from engine.image_engine import ImageEngine, DEFAULT_MODEL
from engine.chat_engine import ChatEngine

logging.basicConfig(level=logging.INFO, format="%(asctime)s [%(levelname)s] %(name)s: %(message)s")
logger = logging.getLogger("CompanionServer")

DATA_DIR = os.path.join(BASE_DIR, "data")
OUTPUTS_DIR = os.path.join(BASE_DIR, "outputs")
UI_DIR = os.path.join(BASE_DIR, "ui")
DEFAULT_MODELS_DIR = get_default_models_dir()

os.makedirs(DATA_DIR, exist_ok=True)
os.makedirs(OUTPUTS_DIR, exist_ok=True)
os.makedirs(UI_DIR, exist_ok=True)

# Initialize engines
persona_manager = PersonaManager(DATA_DIR)
model_hub = ModelHub(DEFAULT_MODELS_DIR)
image_engine = ImageEngine(DEFAULT_MODELS_DIR, OUTPUTS_DIR)
chat_engine = ChatEngine(persona_manager)

app = FastAPI(title="Companion Studio", version="1.1.0")

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# Active WebSockets
connected_websockets: List[WebSocket] = []
main_event_loop: Optional[asyncio.AbstractEventLoop] = None

@app.on_event("startup")
async def on_startup():
    global main_event_loop
    main_event_loop = asyncio.get_running_loop()
    logger.info("Main asyncio loop registrado para Companion Studio.")

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
    global main_event_loop
    if main_event_loop is not None and main_event_loop.is_running():
        try:
            asyncio.run_coroutine_threadsafe(broadcast_ws(data), main_event_loop)
        except Exception as e:
            logger.warning(f"Error WS broadcast: {e}")

# Request Models
class SendMessageRequest(BaseModel):
    persona_id: str
    message: str
    model_id: str = DEFAULT_MODEL
    force_photo: bool = False
    custom_prompt: Optional[str] = None
    width: int = 512
    height: int = 768
    steps: int = 22
    cfg: float = 6.5
    seed: Optional[int] = None

class PersonaCreateRequest(BaseModel):
    name: str
    tagline: str
    style: str = "photorealistic" # photorealistic, anime, cinematic
    avatar: Optional[str] = None
    appearance: Dict[str, Any]
    personality: Dict[str, Any]

class AlbumGenerateRequest(BaseModel):
    count: int = 4
    model_id: Optional[str] = None
    width: int = 512
    height: int = 768
    cfg: float = 6.5

class SetAvatarRequest(BaseModel):
    avatar_url: str

class DownloadHFRequest(BaseModel):
    repo_id: str

class DownloadCivitaiRequest(BaseModel):
    model_id_or_url: str

class SettingsUpdateRequest(BaseModel):
    models_dir: str

# -------------------------------------------------------------
# REST ENDPOINTS: HARDWARE TELEMETRY
# -------------------------------------------------------------

@app.get("/api/system/status")
def get_system_telemetry():
    vram_used = 0.0
    vram_total = 8.0
    gpu_name = "NVIDIA GeForce RTX 4060"
    gpu_temp = 42

    try:
        import torch
        if torch.cuda.is_available():
            gpu_name = torch.cuda.get_device_name(0)
            allocated = torch.cuda.memory_allocated() / (1024**3)
            reserved = torch.cuda.memory_reserved() / (1024**3)
            vram_used = round(max(allocated, reserved), 1)
            vram_total = round(torch.cuda.get_device_properties(0).total_memory / (1024**3), 1)
    except Exception:
        pass

    ram_used = 18.0
    ram_total = 48.0
    try:
        import psutil
        v = psutil.virtual_memory()
        ram_used = round((v.total - v.available) / (1024**3), 1)
        ram_total = round(v.total / (1024**3), 1)
    except Exception:
        pass

    return {
        "gpu": {
            "name": gpu_name,
            "vram_used_gb": vram_used,
            "vram_total_gb": vram_total,
            "vram_percent": round((vram_used / vram_total) * 100, 1) if vram_total > 0 else 0,
            "temp_c": gpu_temp
        },
        "ram": {
            "used_gb": ram_used,
            "total_gb": ram_total,
            "percent": round((ram_used / ram_total) * 100, 1) if ram_total > 0 else 0
        },
        "models_dir": model_hub.models_dir
    }

# -------------------------------------------------------------
# REST ENDPOINTS: PERSONA CRUD
# -------------------------------------------------------------

@app.get("/api/personas")
def list_personas():
    return persona_manager.get_personas()

@app.get("/api/personas/{persona_id}")
def get_persona(persona_id: str):
    p = persona_manager.get_persona(persona_id)
    if not p:
        raise HTTPException(status_code=404, detail="Personaje no encontrado")
    return p

@app.post("/api/personas")
def create_persona(req: PersonaCreateRequest):
    new_persona = persona_manager.create_persona(req.dict())
    sync_ws_broadcast({
        "type": "persona_created",
        "persona": new_persona
    })
    return new_persona

@app.put("/api/personas/{persona_id}")
def update_persona(persona_id: str, req: PersonaCreateRequest):
    updated = persona_manager.update_persona(persona_id, req.dict())
    if not updated:
        raise HTTPException(status_code=404, detail="Personaje no encontrado")
    sync_ws_broadcast({
        "type": "persona_updated",
        "persona": updated
    })
    return updated

@app.delete("/api/personas/{persona_id}")
def delete_persona(persona_id: str):
    success = persona_manager.delete_persona(persona_id, outputs_dir=OUTPUTS_DIR)
    if not success:
        raise HTTPException(status_code=404, detail="Personaje no encontrado")
    sync_ws_broadcast({
        "type": "persona_deleted",
        "persona_id": persona_id
    })
    return {"status": "ok", "message": "Personaje y sus fotos eliminados"}

@app.post("/api/personas/{persona_id}/avatar")
def set_avatar(persona_id: str, req: SetAvatarRequest):
    p = persona_manager.get_persona(persona_id)
    if not p:
        raise HTTPException(status_code=404, detail="Personaje no encontrado")
    p["avatar"] = req.avatar_url
    updated = persona_manager.update_persona(persona_id, p)
    sync_ws_broadcast({
        "type": "avatar_updated",
        "persona_id": persona_id,
        "avatar": req.avatar_url
    })
    return updated

@app.get("/api/personas/{persona_id}/history")
def get_chat_history(persona_id: str):
    return persona_manager.load_chat_history(persona_id)

@app.post("/api/personas/{persona_id}/history/clear")
def clear_chat_history(persona_id: str):
    persona = persona_manager.get_persona(persona_id)
    initial_msg = []
    if persona:
        initial_msg = [{
            "id": str(uuid.uuid4()),
            "sender": "companion",
            "text": persona["personality"]["greeting"],
            "timestamp": time.strftime("%H:%M"),
            "image_url": None
        }]
    persona_manager.save_chat_history(persona_id, initial_msg)
    return {"status": "ok", "messages": initial_msg}

# -------------------------------------------------------------
# REST ENDPOINTS: CONVERSATION & IMAGE GENERATION
# -------------------------------------------------------------

@app.post("/api/chat/send")
def send_chat_message(req: SendMessageRequest):
    persona = persona_manager.get_persona(req.persona_id)
    if not persona:
        raise HTTPException(status_code=404, detail="Personaje no encontrado")

    history = persona_manager.load_chat_history(req.persona_id)

    # 1. Append user message
    user_msg_id = str(uuid.uuid4())
    user_entry = {
        "id": user_msg_id,
        "sender": "user",
        "text": req.message,
        "timestamp": time.strftime("%H:%M"),
        "image_url": None
    }
    history.append(user_entry)

    # 2. Get conversational response
    chat_result = chat_engine.generate_reply(req.persona_id, req.message, history)
    reply_text = chat_result["reply"]

    # 3. Check if image generation is triggered
    should_generate_image = req.force_photo or chat_result.get("wants_photo", False)
    image_url = None
    image_meta = None

    if should_generate_image:
        def prog_cb(event):
            sync_ws_broadcast({
                "type": "generation_progress",
                "persona_id": req.persona_id,
                "data": event
            })

        photo_prompt = req.custom_prompt or chat_result.get("photo_prompt")
        neg_prompt = chat_result.get("negative_prompt") or persona["appearance"].get("negative_prompt", "")

        try:
            image_result = image_engine.generate_companion_image(
                prompt=photo_prompt,
                negative_prompt=neg_prompt,
                persona_id=req.persona_id,
                model_id=req.model_id,
                width=req.width,
                height=req.height,
                num_inference_steps=req.steps,
                guidance_scale=req.cfg,
                seed=req.seed,
                progress_callback=prog_cb
            )
            image_url = image_result["url"]
            image_meta = image_result
        except Exception as e:
            logger.error(f"Error generando imagen durante chat: {e}")
            reply_text += " *(Intenté tomarme la foto pero hubo un pequeño fallo técnico con la cámara)*"

    # 4. Save companion message
    comp_msg_id = str(uuid.uuid4())
    comp_entry = {
        "id": comp_msg_id,
        "sender": "companion",
        "text": reply_text,
        "timestamp": time.strftime("%H:%M"),
        "image_url": image_url,
        "image_metadata": image_meta
    }
    history.append(comp_entry)
    persona_manager.save_chat_history(req.persona_id, history)

    # 5. Broadcast complete
    sync_ws_broadcast({
        "type": "new_message",
        "persona_id": req.persona_id,
        "message": comp_entry
    })

    return comp_entry

# -------------------------------------------------------------
# REST ENDPOINTS: AUTOMATED PHOTO ALBUM SYNTHESIS
# -------------------------------------------------------------

@app.post("/api/personas/{persona_id}/generate-album")
def generate_persona_album(persona_id: str, req: AlbumGenerateRequest, background_tasks: BackgroundTasks):
    persona = persona_manager.get_persona(persona_id)
    if not persona:
        raise HTTPException(status_code=404, detail="Personaje no encontrado")

    scenarios = persona_manager.get_album_prompts(persona)
    selected_scenarios = scenarios[:max(1, min(req.count, len(scenarios)))]
    active_model = req.model_id or DEFAULT_MODEL

    def _album_worker():
        generated_photos = []
        total_photos = len(selected_scenarios)
        logger.info(f"Iniciando síntesis de álbum ({total_photos} fotos) para {persona['name']}...")

        sync_ws_broadcast({
            "type": "album_started",
            "persona_id": persona_id,
            "total_photos": total_photos,
            "message": f"Iniciando sesión fotográfica para {persona['name']} en RTX 4060..."
        })

        for idx, sc in enumerate(selected_scenarios):
            logger.info(f"Generando foto {idx+1}/{total_photos}: '{sc['title']}'...")

            def step_cb(data):
                data_copy = dict(data)
                data_copy["album_photo_index"] = idx + 1
                data_copy["album_total_photos"] = total_photos
                data_copy["scenario_title"] = sc["title"]
                sync_ws_broadcast({
                    "type": "album_progress",
                    "persona_id": persona_id,
                    "data": data_copy
                })

            try:
                img_res = image_engine.generate_companion_image(
                    prompt=sc["prompt"],
                    negative_prompt=sc["negative_prompt"],
                    persona_id=persona_id,
                    model_id=active_model,
                    width=req.width,
                    height=req.height,
                    num_inference_steps=sc.get("steps", 22),
                    guidance_scale=req.cfg,
                    progress_callback=step_cb
                )
                img_res["scenario_title"] = sc["title"]
                generated_photos.append(img_res)

                # Update metadata file with scenario title
                meta_file = img_res["filepath"].replace(".jpg", ".json").replace(".png", ".json")
                if os.path.exists(meta_file):
                    try:
                        with open(meta_file, "r", encoding="utf-8") as mf:
                            m_data = json.load(mf)
                        m_data["scenario_title"] = sc["title"]
                        with open(meta_file, "w", encoding="utf-8") as mf:
                            json.dump(m_data, mf, indent=2, ensure_ascii=False)
                    except Exception:
                        pass

                # If avatar is default, set this first photo as the main avatar!
                curr_p = persona_manager.get_persona(persona_id)
                if curr_p and (not curr_p.get("avatar") or "default" in curr_p.get("avatar", "")):
                    curr_p["avatar"] = img_res["url"]
                    persona_manager.update_persona(persona_id, curr_p)
                    sync_ws_broadcast({
                        "type": "avatar_updated",
                        "persona_id": persona_id,
                        "avatar": img_res["url"]
                    })

                sync_ws_broadcast({
                    "type": "album_photo_saved",
                    "persona_id": persona_id,
                    "photo": img_res,
                    "current": idx + 1,
                    "total": total_photos
                })

            except Exception as e:
                logger.error(f"Error generando foto #{idx+1} de álbum: {e}")

        sync_ws_broadcast({
            "type": "album_completed",
            "persona_id": persona_id,
            "photos": generated_photos,
            "message": f"¡Sesión de fotos completada! Se han generado {len(generated_photos)} fotos en la galería."
        })

    background_tasks.add_task(_album_worker)
    return {
        "status": "started",
        "persona_id": persona_id,
        "total_photos": len(selected_scenarios),
        "scenarios": [s["title"] for s in selected_scenarios]
    }

# -------------------------------------------------------------
# REST ENDPOINTS: GALLERY
# -------------------------------------------------------------

@app.get("/api/gallery/{persona_id}")
def get_persona_gallery(persona_id: str):
    """Retrieve all photos generated for this specific companion."""
    folder = os.path.join(OUTPUTS_DIR, persona_id)
    items = []
    if os.path.exists(folder):
        for f in os.listdir(folder):
            if f.endswith((".jpg", ".png")):
                meta_f = os.path.join(folder, f.replace(".jpg", ".json").replace(".png", ".json"))
                meta = None
                if os.path.exists(meta_f):
                    try:
                        with open(meta_f, "r", encoding="utf-8") as mf:
                            meta = json.load(mf)
                    except Exception:
                        pass
                
                full_path = os.path.join(folder, f)
                items.append({
                    "filename": f,
                    "url": f"/outputs/{persona_id}/{f}",
                    "created_at": os.path.getmtime(full_path),
                    "size_kb": round(os.path.getsize(full_path) / 1024, 1),
                    "metadata": meta
                })
    items.sort(key=lambda x: x["created_at"], reverse=True)
    return items

@app.delete("/api/gallery/{persona_id}/{filename}")
def delete_gallery_photo(persona_id: str, filename: str):
    folder = os.path.join(OUTPUTS_DIR, persona_id)
    img_path = os.path.join(folder, filename)
    meta_path = os.path.join(folder, filename.replace(".jpg", ".json").replace(".png", ".json"))
    
    deleted = False
    if os.path.exists(img_path):
        try:
            os.remove(img_path)
            deleted = True
        except Exception as e:
            logger.error(f"Error borrando imagen: {e}")

    if os.path.exists(meta_path):
        try:
            os.remove(meta_path)
        except Exception:
            pass

    if deleted:
        return {"status": "ok", "message": "Foto eliminada"}
    raise HTTPException(status_code=404, detail="Foto no encontrada")

# -------------------------------------------------------------
# REST ENDPOINTS: MODEL HUB
# -------------------------------------------------------------

@app.get("/api/models")
def list_models():
    return model_hub.list_models()

@app.post("/api/models/download/hf")
def download_hf(req: DownloadHFRequest):
    def callback(info):
        sync_ws_broadcast({"type": "download_progress", "data": info})
    task_id = model_hub.start_huggingface_download(req.repo_id, callback)
    return {"status": "started", "task_id": task_id}

@app.post("/api/models/download/civitai")
def download_civitai(req: DownloadCivitaiRequest):
    def callback(info):
        sync_ws_broadcast({"type": "download_progress", "data": info})
    task_id = model_hub.start_civitai_download(req.model_id_or_url, callback)
    return {"status": "started", "task_id": task_id}

@app.post("/api/settings/update")
def update_settings(req: SettingsUpdateRequest):
    if not os.path.exists(req.models_dir):
        os.makedirs(req.models_dir, exist_ok=True)
    model_hub.set_models_dir(req.models_dir)
    image_engine.models_dir = os.path.abspath(req.models_dir)
    return {"status": "ok", "models_dir": req.models_dir}

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
    logger.info("Cliente WebSocket conectado a Companion Studio.")
    try:
        while True:
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
        logger.debug(f"Error WS: {e}")

# Mount static directories
app.mount("/outputs", StaticFiles(directory=OUTPUTS_DIR), name="outputs")
app.mount("/", StaticFiles(directory=UI_DIR, html=True), name="ui")
