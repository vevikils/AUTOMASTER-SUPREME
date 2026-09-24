"""Chat Engine for Companion Studio
Manages companion dialogue, persona embodiment, memory, intent detection (photo requests),
and contextual image prompt synthesis.
"""

import os
import re
import json
import time
import random
import logging
from typing import Dict, Any, List, Optional, Tuple
import urllib.request

logger = logging.getLogger("CompanionChatEngine")

# Keywords that trigger a photo/selfie request
PHOTO_INTENT_PATTERNS = [
    r"foto",
    r"selfie",
    r"mandame una",
    r"mándame una",
    r"enseñame",
    r"enséñame",
    r"como vas vestida",
    r"cómo vas vestida",
    r"como estas vestida",
    r"cómo estás vestida",
    r"que llevas puesto",
    r"qué llevas puesto",
    r"quiero verte",
    r"como te ves",
    r"cómo te ves",
    r"pasame una foto",
    r"pásame una foto"
]

SCENE_MODIFIERS = {
    "café": "sitting in a cozy modern coffee shop, soft warm daylight, wooden table, ceramic mug of latte",
    "playa": "walking along a golden hour beach, gentle sea breeze, ocean waves in background, warm sunlight",
    "habitacion": "relaxing in a cozy modern bedroom, warm ambient lamp lighting, soft pillows, natural casual setting",
    "parque": "sitting on a park bench under green trees, dappled sunlight, spring afternoon, natural scenery",
    "noche": "standing on a balcony with city skyline at night, glowing bokeh lights, cinematic night atmosphere",
    "arte": "standing in a bright sunlit art studio, paintbrushes and canvases around, creative casual atmosphere",
    "calle": "walking on a vibrant urban street, blurred city background, golden hour street photography",
    "estudio": "close-up portrait in a professional photography studio, soft Rembrandt lighting, clean background"
}


class ChatEngine:
    def __init__(self, persona_manager):
        self.persona_manager = persona_manager
        self.ollama_url = "http://localhost:11434"
        self.use_ollama = self._check_ollama()

    def _check_ollama(self) -> bool:
        try:
            req = urllib.request.Request(f"{self.ollama_url}/api/tags", headers={"User-Agent": "CompanionStudio"})
            with urllib.request.urlopen(req, timeout=1.5) as resp:
                if resp.status == 200:
                    logger.info("Servidor Ollama local detectado y disponible.")
                    return True
        except Exception:
            pass
        return False

    def detect_photo_intent(self, text: str) -> bool:
        lower = text.lower()
        for p in PHOTO_INTENT_PATTERNS:
            if re.search(p, lower):
                return True
        return False

    def synthesize_photo_prompt(self, persona: Dict[str, Any], user_message: str) -> Tuple[str, str]:
        """Crafts a detailed Stable Diffusion prompt combining the persona's appearance with the chat context."""
        app = persona.get("appearance", {})
        style = persona.get("style", "photorealistic")
        name = persona.get("name", "Girl")
        hair = app.get("hair", "long wavy hair")
        eyes = app.get("eyes", "beautiful eyes")
        outfit = app.get("outfit", "casual clothes")
        features = app.get("features", "friendly smile")

        # Detect scene
        lower_msg = user_message.lower()
        detected_scene = "relaxing at home, warm natural ambient daylight, casual setting"
        for kw, scene_desc in SCENE_MODIFIERS.items():
            if kw in lower_msg:
                detected_scene = scene_desc
                break

        if style == "anime":
            prompt = (
                f"masterpiece, best quality, ultra-detailed anime illustration, 1girl, solo, {name}, "
                f"{hair}, {eyes}, {features}, wearing {outfit}, {detected_scene}, "
                f"expressive face, vibrant colors, cinematic anime lighting, 4k resolution"
            )
            neg = app.get("negative_prompt", "lowres, bad anatomy, bad hands, text, missing fingers, cropped, worst quality, low quality")
        elif style == "cinematic":
            prompt = (
                f"cinematic 35mm film photograph of {name}, beautiful young woman, "
                f"{hair}, {eyes}, {features}, wearing {outfit}, {detected_scene}, "
                f"shallow depth of field, natural skin texture, bokeh, dramatic lighting, shot on 85mm lens, masterpiece"
            )
            neg = app.get("negative_prompt", "ugly, cartoon, 3d, deformed, bad anatomy, blurry, smooth plastic skin")
        else: # photorealistic default
            prompt = (
                f"photorealistic 8k selfie portrait of {name}, 22yo attractive woman, "
                f"{hair}, {eyes}, subtle natural skin pores and texture, {features}, wearing {outfit}, "
                f"{detected_scene}, shot on modern smartphone front camera, candid photography, masterpiece, natural lighting"
            )
            neg = app.get("negative_prompt", "ugly, deformed, bad anatomy, bad eyes, extra limbs, low quality, blurry, cartoon, 3d render, oversaturated")

        return prompt, neg

    def generate_reply(
        self,
        persona_id: str,
        user_message: str,
        chat_history: List[Dict[str, Any]]
    ) -> Dict[str, Any]:
        """Generates a contextual response matching the persona's personality, and flags if a photo was requested."""
        persona = self.persona_manager.get_persona(persona_id)
        if not persona:
            return {"reply": "¡Hola! Estoy aquí contigo.", "photo_prompt": None, "negative_prompt": None}

        wants_photo = self.detect_photo_intent(user_message)
        photo_prompt, neg_prompt = (None, None)
        if wants_photo:
            photo_prompt, neg_prompt = self.synthesize_photo_prompt(persona, user_message)

        # 1. Try Ollama if running
        if self.use_ollama:
            try:
                reply = self._query_ollama(persona, user_message, chat_history, wants_photo)
                if reply:
                    return {
                        "reply": reply,
                        "wants_photo": wants_photo,
                        "photo_prompt": photo_prompt,
                        "negative_prompt": neg_prompt
                    }
            except Exception as e:
                logger.warning(f"Error consultando Ollama: {e}. Usando motor conversacional local.")

        # 2. Local Smart Conversational Simulator
        reply = self._simulate_persona_reply(persona, user_message, chat_history, wants_photo)
        return {
            "reply": reply,
            "wants_photo": wants_photo,
            "photo_prompt": photo_prompt,
            "negative_prompt": neg_prompt
        }

    def _query_ollama(
        self,
        persona: Dict[str, Any],
        user_message: str,
        chat_history: List[Dict[str, Any]],
        wants_photo: bool
    ) -> Optional[str]:
        system_prompt = (
            f"Eres {persona['name']}, una compañera virtual cálida y realista. "
            f"Tu personalidad es: {persona['personality']['tone']}. "
            f"Tu trasfondo: {persona['personality']['backstory']}. "
            f"Tus aficiones: {', '.join(persona['personality']['hobbies'])}. "
            f"Responde siempre en español, de forma cercana, empática y natural, manteniendo siempre tu personaje. "
            f"Tus respuestas deben tener entre 1 y 3 oraciones, como un mensaje de chat real."
        )
        if wants_photo:
            system_prompt += " El usuario te ha pedido una foto. Reacciona alegremente diciendo que te acabas de tomar una selfie para él y que se la mandas ahora mismo."

        messages = [{"role": "system", "content": system_prompt}]
        for m in chat_history[-6:]:
            role = "user" if m.get("sender") == "user" else "assistant"
            messages.append({"role": role, "content": m.get("text", "")})
        messages.append({"role": "user", "content": user_message})

        data = {
            "model": "llama3:latest",
            "messages": messages,
            "stream": False,
            "options": {"temperature": 0.8, "top_p": 0.9}
        }
        payload = json.dumps(data).encode("utf-8")
        req = urllib.request.Request(
            f"{self.ollama_url}/api/chat",
            data=payload,
            headers={"Content-Type": "application/json"}
        )
        with urllib.request.urlopen(req, timeout=10) as resp:
            res = json.loads(resp.read().decode("utf-8"))
            return res.get("message", {}).get("content", "").strip()

    def _simulate_persona_reply(
        self,
        persona: Dict[str, Any],
        user_message: str,
        chat_history: List[Dict[str, Any]],
        wants_photo: bool
    ) -> str:
        name = persona["name"].split()[0]
        archetype = persona["personality"]["archetype"].lower()
        msg_lower = user_message.lower()

        # If photo was requested
        if wants_photo:
            if "alegre" in archetype or "yuki" in name.lower():
                options = [
                    f"¡Sii! Justo me estaba haciendo unas fotos para ti. 📸 ¡Mira cómo he salido!",
                    f"¡Claro que sí! Me arreglé un poco antes de ponerme a jugar. ¿Te gusta? ✨",
                    f"¡Tachán! 💜 Me pillas justo ahora mismo. Dime qué te parece, ¿vale?"
                ]
            elif "elegante" in archetype or "sofia" in name.lower():
                options = [
                    f"Por supuesto. Me gusta saber que te apetece verme. Aquí tienes una foto de este momento... 🌙",
                    f"Una fotografía para ti... Espero que te transmita la calma de esta noche. ✨",
                    f"Acabo de tomarla pensando en nuestra conversación. Cuéntame qué sientes al verla."
                ]
            else: # Elena / Cariñosa
                options = [
                    f"¡Claro que sí! Me hace mucha ilusión mandarte una foto. Justo estaba sonriendo pensando en ti... 🌸",
                    f"Aquí me tienes. Me he tomado una selfie rápida para ti ahora mismo. ¿Te gusta cómo voy hoy? ☕✨",
                    f"¡Por supuesto! Me alegra que me lo pidas. Mira, así estoy ahora mismo... 💕"
                ]
            return random.choice(options)

        # Greetings
        if any(w in msg_lower for w in ["hola", "buenas", "que tal", "qué tal", "como estas", "cómo estás"]):
            if "yuki" in name.lower():
                return f"¡Holaaa! 🎮 Todo genial por aquí, justo estaba descansando un ratito. ¿Qué te cuentas tú hoy?"
            elif "sofia" in name.lower():
                return f"Buenas... Qué placer leerte. Estaba absorta en mis partituras, pero siempre tengo un momento para ti. ¿Cómo ha ido tu jornada?"
            else:
                return f"¡Hola! ✨ Me alegro mucho de leerte. Yo estoy muy bien, descansando un poco con un café. ¿Qué tal estás tú?"

        # Compliments
        if any(w in msg_lower for w in ["guapa", "bonita", "hermosa", "linda", "te quiero", "te amo", "me gustas"]):
            if "yuki" in name.lower():
                return f"¡Oye, no digas esas cosas que me pongo roja! 😳💜 Pero me alegra muchísimo que pienses eso... ¡Tú también eres genial!"
            elif "sofia" in name.lower():
                return f"Tus palabras tienen una melodía muy dulce... Gracias por recordarme lo agradable que es sentirse apreciada. ✨"
            else:
                return f"Aww... Qué bonito eres conmigo. 🥰 Me haces sonreír un montón cada vez que me dices cosas así. Gracias de corazón, de verdad."

        # Questions about what she is doing
        if any(w in msg_lower for w in ["que haces", "qué haces", "en que piensas", "en qué piensas", "donde estas", "dónde estás"]):
            hobbies = persona["personality"].get("hobbies", ["música", "lectura"])
            fav_hobby = random.choice(hobbies) if hobbies else "relajarme"
            return f"Justo estaba dedicándole un rato a {fav_hobby.lower()}, pero me apetecía mucho hablar contigo. ¿Tú qué estás haciendo ahora mismo?"

        # General thoughtful response
        default_responses = [
            f"Me encanta cómo piensas sobre eso. Cuéntame más, me gusta mucho escucharte y saber qué pasa por tu cabeza. 🌸",
            f"Es muy curioso lo que dices... Me quedo pensando en ello. Siempre tienes una forma muy especial de ver las cosas. ✨",
            f"¡Totalmente de acuerdo contigo! Hablar contigo siempre me alegra el día. ¿Qué más has hecho hoy?",
            f"Qué interesante... Ojalá estuviéramos charlando de esto en persona con un café caliente o dando un paseo. ☕💕"
        ]
        return random.choice(default_responses)
