"""Persona Manager for Companion Studio
Handles creation, updating, retrieval, persistence of virtual companions, chat history,
and contextual photo album synthesis based on character descriptions.
"""

import os
import json
import time
import uuid
import shutil
import logging
from typing import Dict, Any, List, Optional

logger = logging.getLogger("PersonaManager")


class PersonaManager:
    def __init__(self, data_dir: str):
        self.data_dir = os.path.abspath(data_dir)
        self.personas_file = os.path.join(self.data_dir, "personas.json")
        self.history_dir = os.path.join(self.data_dir, "chat_history")
        
        os.makedirs(self.data_dir, exist_ok=True)
        os.makedirs(self.history_dir, exist_ok=True)
        
        self.personas: List[Dict[str, Any]] = []
        self._load_personas()

    def _load_personas(self):
        """Loads personas.json. Never re-creates default personas if empty."""
        if not os.path.exists(self.personas_file):
            self.personas = []
            self._save_personas()
        else:
            try:
                with open(self.personas_file, "r", encoding="utf-8") as f:
                    content = f.read().strip()
                    if content:
                        self.personas = json.loads(content)
                    else:
                        self.personas = []
            except Exception as e:
                logger.error(f"Error leyendo {self.personas_file}: {e}")
                self.personas = []

    def _save_personas(self):
        try:
            with open(self.personas_file, "w", encoding="utf-8") as f:
                json.dump(self.personas, f, indent=2, ensure_ascii=False)
        except Exception as e:
            logger.error(f"Error guardando {self.personas_file}: {e}")

    def get_personas(self) -> List[Dict[str, Any]]:
        return self.personas

    def get_persona(self, persona_id: str) -> Optional[Dict[str, Any]]:
        for p in self.personas:
            if p["id"] == persona_id:
                return p
        return None

    def create_persona(self, data: Dict[str, Any]) -> Dict[str, Any]:
        new_id = f"persona_{uuid.uuid4().hex[:8]}"
        persona = {
            "id": new_id,
            "name": data.get("name", "Nueva Compañera"),
            "tagline": data.get("tagline", "Compañera virtual personalizada"),
            "avatar": data.get("avatar", "/ui/avatars/default.jpg"),
            "style": data.get("style", "photorealistic"),
            "appearance": {
                "hair": data.get("appearance", {}).get("hair", "long wavy hair"),
                "eyes": data.get("appearance", {}).get("eyes", "expressive eyes"),
                "outfit": data.get("appearance", {}).get("outfit", "casual stylish outfit"),
                "features": data.get("appearance", {}).get("features", "natural friendly smile"),
                "base_prompt": data.get("appearance", {}).get("base_prompt", ""),
                "negative_prompt": data.get("appearance", {}).get("negative_prompt", "")
            },
            "personality": {
                "archetype": data.get("personality", {}).get("archetype", "Cariñosa"),
                "tone": data.get("personality", {}).get("tone", "cálido y cercano"),
                "backstory": data.get("personality", {}).get("backstory", ""),
                "greeting": data.get("personality", {}).get("greeting", ""),
                "hobbies": data.get("personality", {}).get("hobbies", ["Música", "Pasear", "Fotografía"])
            },
            "created_at": time.strftime("%Y-%m-%d %H:%M:%S")
        }

        # Set default greeting if not provided
        if not persona["personality"]["greeting"]:
            persona["personality"]["greeting"] = f"¡Hola! ✨ Me alegro mucho de que estés aquí. Soy {persona['name']}. ¿Qué tal ha ido tu día?"

        # Auto-synthesize base_prompt if empty
        if not persona["appearance"]["base_prompt"]:
            if persona["style"] == "anime":
                style_prefix = "masterpiece, best quality, ultra-detailed anime illustration of"
            elif persona["style"] == "cinematic":
                style_prefix = "cinematic 35mm film photograph of"
            else:
                style_prefix = "photorealistic 8k portrait of"

            persona["appearance"]["base_prompt"] = (
                f"{style_prefix} {persona['name']}, a beautiful young woman, "
                f"{persona['appearance']['hair']}, {persona['appearance']['eyes']}, "
                f"{persona['appearance']['features']}, wearing {persona['appearance']['outfit']}"
            )

        # Negative prompt default if empty
        if not persona["appearance"]["negative_prompt"]:
            if persona["style"] == "anime":
                persona["appearance"]["negative_prompt"] = "lowres, bad anatomy, bad hands, text, missing fingers, extra digits, cropped, worst quality, low quality, normal quality, jpeg artifacts"
            elif persona["style"] == "cinematic":
                persona["appearance"]["negative_prompt"] = "oversaturated, flat lighting, amateur, deformed, bad anatomy, blurry, cartoon, grainy, low resolution, plastic skin"
            else:
                persona["appearance"]["negative_prompt"] = "ugly, deformed, disfigured, poor details, bad anatomy, bad eyes, extra limbs, low quality, blurry, cartoon, 3d render, oversaturated"

        self.personas.append(persona)
        self._save_personas()
        return persona

    def update_persona(self, persona_id: str, data: Dict[str, Any]) -> Optional[Dict[str, Any]]:
        for idx, p in enumerate(self.personas):
            if p["id"] == persona_id:
                p["name"] = data.get("name", p["name"])
                p["tagline"] = data.get("tagline", p["tagline"])
                p["style"] = data.get("style", p["style"])
                if "avatar" in data and data["avatar"]:
                    p["avatar"] = data["avatar"]
                if "appearance" in data:
                    p["appearance"].update(data["appearance"])
                if "personality" in data:
                    p["personality"].update(data["personality"])
                
                self.personas[idx] = p
                self._save_personas()
                return p
        return None

    def delete_persona(self, persona_id: str, outputs_dir: Optional[str] = None) -> bool:
        initial_len = len(self.personas)
        self.personas = [p for p in self.personas if p["id"] != persona_id]
        if len(self.personas) < initial_len:
            self._save_personas()
            # Clean history file
            hist_file = os.path.join(self.history_dir, f"{persona_id}.json")
            if os.path.exists(hist_file):
                try:
                    os.remove(hist_file)
                except Exception as e:
                    logger.warning(f"Error borrando historial: {e}")
            # Clean outputs folder if provided
            if outputs_dir:
                p_out = os.path.join(outputs_dir, persona_id)
                if os.path.exists(p_out):
                    try:
                        shutil.rmtree(p_out)
                    except Exception as e:
                        logger.warning(f"Error borrando fotos de {persona_id}: {e}")
            return True
        return False

    def get_album_prompts(self, persona: Dict[str, Any]) -> List[Dict[str, str]]:
        """Synthesizes photographic prompts specifically tailored to the persona's physical description and hobbies."""
        app = persona.get("appearance", {})
        style = persona.get("style", "photorealistic")
        name = persona.get("name", "Girl")
        hair = app.get("hair", "beautiful hair")
        eyes = app.get("eyes", "expressive eyes")
        outfit = app.get("outfit", "casual stylish outfit")
        features = app.get("features", "natural friendly smile")
        personality = persona.get("personality", {})
        hobbies = personality.get("hobbies", ["música", "café"])
        hobby_str = hobbies[0] if isinstance(hobbies, list) and len(hobbies) > 0 else "su pasatiempo favorito"

        scenarios = [
            {
                "title": "Retrato Principal",
                "scene": "close-up portrait shot, looking directly into camera with a sweet gentle smile, soft natural daylight, depth of field, sharp focus on eyes, authentic beauty",
                "steps": 22
            },
            {
                "title": "Selfie Casual en Casa",
                "scene": "candid smartphone front-camera selfie, relaxing in a cozy room, warm ambient lighting, cute playful expression, casual authentic aesthetic",
                "steps": 22
            },
            {
                "title": "Paseo al Atardecer",
                "scene": "half-body outdoor photograph, walking along scenic city streets during golden hour sunset, warm sunlight glow, soft bokeh lights",
                "steps": 24
            },
            {
                "title": "Disfrutando en una Cafetería",
                "scene": f"lifestyle photo sitting at a cozy coffee shop table with a warm ceramic cup, relaxed posture, candid authentic moment, gentle daylight",
                "steps": 22
            }
        ]

        results = []
        for s in scenarios:
            scene_desc = s["scene"]
            if style == "anime":
                prompt = (
                    f"masterpiece, best quality, ultra-detailed anime illustration, 1girl, solo, {name}, "
                    f"{hair}, {eyes}, {features}, wearing {outfit}, {scene_desc}, "
                    f"expressive face, vibrant colors, cinematic anime lighting, 4k resolution"
                )
                neg = app.get("negative_prompt") or "lowres, bad anatomy, bad hands, text, missing fingers, cropped, worst quality, low quality, jpeg artifacts"
            elif style == "cinematic":
                prompt = (
                    f"cinematic 35mm movie still of {name}, beautiful young woman, "
                    f"{hair}, {eyes}, {features}, wearing {outfit}, {scene_desc}, "
                    f"shallow depth of field, natural skin texture, dramatic soft lighting, shot on 85mm lens, masterpiece"
                )
                neg = app.get("negative_prompt") or "ugly, cartoon, 3d render, deformed, bad anatomy, blurry, plastic skin, bad eyes"
            else: # photorealistic default
                prompt = (
                    f"photorealistic 8k photograph of {name}, 22yo woman, "
                    f"{hair}, {eyes}, natural skin texture and subtle pores, {features}, wearing {outfit}, "
                    f"{scene_desc}, 35mm film photography, masterpiece, natural lighting"
                )
                neg = app.get("negative_prompt") or "ugly, deformed, bad anatomy, bad eyes, extra limbs, low quality, blurry, cartoon, 3d render, doll, oversaturated"

            results.append({
                "title": s["title"],
                "prompt": prompt,
                "negative_prompt": neg,
                "steps": s.get("steps", 22)
            })

        return results

    def load_chat_history(self, persona_id: str) -> List[Dict[str, Any]]:
        hist_file = os.path.join(self.history_dir, f"{persona_id}.json")
        if os.path.exists(hist_file):
            try:
                with open(hist_file, "r", encoding="utf-8") as f:
                    return json.load(f)
            except Exception as e:
                logger.error(f"Error cargando historial de {persona_id}: {e}")
        
        persona = self.get_persona(persona_id)
        if persona:
            greeting_msg = {
                "id": str(uuid.uuid4()),
                "sender": "companion",
                "text": persona["personality"]["greeting"],
                "timestamp": time.strftime("%H:%M"),
                "image_url": None
            }
            return [greeting_msg]
        return []

    def save_chat_history(self, persona_id: str, messages: List[Dict[str, Any]]):
        hist_file = os.path.join(self.history_dir, f"{persona_id}.json")
        try:
            with open(hist_file, "w", encoding="utf-8") as f:
                json.dump(messages, f, indent=2, ensure_ascii=False)
        except Exception as e:
            logger.error(f"Error guardando historial de {persona_id}: {e}")
