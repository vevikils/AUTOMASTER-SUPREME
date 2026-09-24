"""Contextual AI Prompt Enhancer for Wan 2.1 Video Studio
Analyzes user inputs (Spanish/English), detects subject, environment, motion dynamics,
and synthesizes bespoke cinematographic video prompts tailored to the exact user prompt.
"""

import re
import random
from typing import Dict, Any, List, Tuple

# Multilingual term mappings to identify core concepts
SUBJECT_PATTERNS = [
    # Female / Woman / Girl
    (r"\b(chica|mujer|muchacha|senorita|señorita|modelo|chicas|mujeres|actriz|dama|princesa|girl|woman|female|lady|model|actress)\b", "woman"),
    # Male / Man / Boy
    (r"\b(chico|hombre|muchacho|senor|señor|caballero|actor|guerrero|man|male|boy|guy|gentleman|warrior)\b", "man"),
    # Animals
    (r"\b(perro|perrito|cachorro|can|dog|puppy|hound)\b", "dog"),
    (r"\b(gato|gatito|felino|cat|kitten|feline)\b", "cat"),
    (r"\b(caballo|yegua|potro|horse|stallion|mare)\b", "horse"),
    (r"\b(pajaro|pájaro|ave|aguila|águila|halcon|halcón|bird|eagle|hawk)\b", "bird"),
    (r"\b(dragon|dragón|dragones|monster|monstruo|creature|criatura|bestia|beast)\b", "fantasy_creature"),
    # Vehicles
    (r"\b(coche|auto|automovil|automóvil|carro|vehiculo|vehículo|car|sports car|supercar|ferrari|porsche|bmw|audi|lamborghini)\b", "sports_car"),
    (r"\b(moto|motocicleta|bike|motorcycle|superbike)\b", "motorcycle"),
    (r"\b(avion|avión|helicoptero|helicóptero|nave|cohete|airplane|plane|spaceship|rocket|drone)\b", "aircraft"),
    # Sci-Fi / Robots
    (r"\b(robot|cyborg|androide|mecha|cyberpunk|ia|ai|futurista)\b", "robot"),
    # Cosmic / Astronomy
    (r"\b(espacio|galaxia|universo|estrella|estrellas|planeta|planetas|nebulosa|space|galaxy|cosmos|nebula|star|stars|supernova|black hole)\b", "cosmic"),
    # Elemental / Dynamic effects
    (r"\b(fuego|llama|llamas|explosion|explosión|magma|volcan|volcán|fire|flames|explosion|blast|lava)\b", "fire_elemental"),
    (r"\b(agua|rio|río|ola|olas|splash|gota|gotas|water|waves|river|splash|droplets)\b", "water_elemental"),
    # Food / Drink / Lifestyle
    (r"\b(cafe|café|comida|postre|hamburguesa|pizza|vino|trago|cocktail|coffee|food|drink|beverage|dish)\b", "food_drink"),
    # Landscapes / Nature (when no living subject)
    (r"\b(playa|costa|mar|oceano|océano|beach|ocean|sea|coast)\b", "beach"),
    (r"\b(bosque|selva|arboles|árboles|forest|woods|jungle)\b", "forest"),
    (r"\b(montana|montaña|montanas|montañas|alpes|sierra|mountain|mountains|peaks)\b", "mountain"),
    (r"\b(cascada|catarata|waterfall|falls)\b", "waterfall"),
    (r"\b(ciudad|calle|avenida|rascacielos|city|street|skyline|urban)\b", "city"),
    (r"\b(desierto|dunas|desert|sand dunes)\b", "desert"),
]

SETTING_PATTERNS = [
    (r"\b(playa|costa|arena|orilla|mar|beach|shore|ocean|coast|seaside)\b", "beach"),
    (r"\b(bosque|selva|arboleda|forest|jungle|woodland)\b", "forest"),
    (r"\b(montana|montaña|nieve|snow|mountain|alps)\b", "mountain"),
    (r"\b(ciudad|calle|avenida|callejon|callejón|city|street|downtown|alley)\b", "city"),
    (r"\b(habitacion|habitación|dormitorio|cama|casa|salon|salón|room|bedroom|indoor|cozy room)\b", "cozy_interior"),
    (r"\b(cafeteria|cafetería|bar|restaurante|cafe|coffee shop|restaurant)\b", "cafe"),
    (r"\b(estudio|set de grabacion|studio|photo studio)\b", "studio"),
    (r"\b(espacio|galaxia|universo|planeta|space|galaxy|cosmos|alien planet)\b", "space"),
    (r"\b(piscina|alberca|pool|swimming pool)\b", "pool"),
    (r"\b(carretera|autopista|pista|road|highway|asphalt|track)\b", "highway"),
    (r"\b(desierto|duna|dunas|desert|dunes)\b", "desert"),
    (r"\b(bajo el agua|submarino|underwater|coral reef|aquarium)\b", "underwater"),
]

WEATHER_LIGHTING_PATTERNS = [
    (r"\b(lluvia|lloviendo|tormenta|mojado|rain|raining|storm|wet)\b", "rain"),
    (r"\b(nieve|nevando|frio|frío|snow|snowing|blizzard|frost)\b", "snow"),
    (r"\b(atardecer|puesta de sol|golden hour|ocaso|sunset|dusk)\b", "sunset"),
    (r"\b(amanecer|madrugada|alba|sunrise|dawn)\b", "sunrise"),
    (r"\b(noche|nocturno|oscuridad|night|nocturnal|darkness)\b", "night"),
    (r"\b(soleado|sol|dia|día|sunny|bright sun|sunlight)\b", "sunny"),
    (r"\b(niebla|neblina|humo|fog|mist|foggy|hazy|smoky)\b", "fog"),
    (r"\b(neon|neones|neon lights|fluorescent)\b", "neon"),
]

ACTION_MOTION_PATTERNS = [
    (r"\b(caminando|andando|paseando|walking|strolling|striding)\b", "walking"),
    (r"\b(corriendo|a toda velocidad|running|sprinting|speeding|fast)\b", "running"),
    (r"\b(bailando|baila|dancing|dance)\b", "dancing"),
    (r"\b(sonriendo|sonrie|sonríe|smiling|smile|laughing)\b", "smiling"),
    (r"\b(mirando|mira|observando|looking|gazing|staring)\b", "gazing"),
    (r"\b(nadando|banandose|bañándose|swimming)\b", "swimming"),
    (r"\b(conduciendo|pilotando|driving|drifting|cruising)\b", "driving"),
    (r"\b(volando|planeando|flying|soaring|gliding)\b", "flying"),
    (r"\b(posando|sentada|sentado|resting|posing|sitting|relaxed)\b", "posing"),
    (r"\b(explotando|ardiendo|exploding|burning|blazing)\b", "exploding"),
]

CLOTHING_PATTERNS = [
    (r"\b(bikini|traje de bano|traje de baño|banador|bañador|swimsuit|swimwear)\b", "bikini swimwear"),
    (r"\b(vestido|dress|gown)\b", "elegant dress"),
    (r"\b(vaqueros|jeans|denim)\b", "casual denim and stylish top"),
    (r"\b(chaqueta|abrigo|cuero|leather jacket|coat)\b", "stylish leather jacket"),
    (r"\b(ropa deportiva|fitness|gym|sportswear|athletic wear)\b", "form-fitting athletic sportswear"),
    (r"\b(armadura|suit|armor|traje espacial|spacesuit)\b", "intricate futuristic armor"),
    (r"\b(camisa|shirt|traje|suit|tuxedo|esmoquin)\b", "sharp tailored suit"),
]


def detect_context(text: str) -> Dict[str, Any]:
    text_lower = text.lower()
    
    # 1. Subject
    subject = "scene"
    for pat, s_type in SUBJECT_PATTERNS:
        if re.search(pat, text_lower):
            subject = s_type
            break
            
    # 2. Setting
    setting = None
    for pat, set_type in SETTING_PATTERNS:
        if re.search(pat, text_lower):
            setting = set_type
            break
    if not setting:
        if subject == "beach":
            setting = "beach"
        elif subject in ["sports_car", "motorcycle"]:
            setting = "highway"
        elif subject == "robot":
            setting = "city"
        elif subject == "cosmic":
            setting = "space"
        elif subject == "food_drink":
            setting = "cafe"
        elif subject == "desert":
            setting = "desert"
        else:
            setting = "outdoor_scenic"

    # 3. Weather / Lighting
    weather_light = "cinematic_soft"
    for pat, wl_type in WEATHER_LIGHTING_PATTERNS:
        if re.search(pat, text_lower):
            weather_light = wl_type
            break

    # 4. Action / Motion
    action = None
    for pat, act_type in ACTION_MOTION_PATTERNS:
        if re.search(pat, text_lower):
            action = act_type
            break

    # 5. Clothing (if human)
    clothing = None
    for pat, cloth_type in CLOTHING_PATTERNS:
        if re.search(pat, text_lower):
            clothing = cloth_type
            break

    return {
        "subject": subject,
        "setting": setting,
        "weather_light": weather_light,
        "action": action,
        "clothing": clothing,
        "raw_text": text.strip()
    }


def synthesize_motion_and_camera(ctx: Dict[str, Any], style: str) -> Tuple[str, str, str]:
    """Returns (subject_details, dynamic_action_details, camera_lighting_setup) tailored to the prompt."""
    subj = ctx["subject"]
    setting = ctx["setting"]
    wl = ctx["weather_light"]
    act = ctx["action"]
    clothing = ctx["clothing"]

    # --- Subject & Action Dynamics ---
    if subj in ["woman", "man"]:
        is_female = (subj == "woman")
        pronoun = "her" if is_female else "his"
        subject_noun = "a stunning young woman" if is_female else "a charismatic young man"
        
        # Attire
        attire_str = f"wearing {clothing}" if clothing else f"dressed in a stylish cinematic outfit"
        
        # Motion
        if act == "walking":
            action_desc = f"confidently walking towards the camera, gentle natural sway, hair lightly dancing in the ambient breeze, authentic lifelike stride"
        elif act == "running":
            action_desc = f"running with dynamic athletic fluidity, subtle motion blur on the limbs, intense expressive focus, wind rushing past"
        elif act == "smiling" or act == "gazing":
            action_desc = f"slowly turning to meet the viewer's gaze, subtle warm authentic smile blooming across {pronoun} lips, natural gentle eye blinks, soft micro-expressions"
        elif act == "posing":
            action_desc = f"relaxing with poised elegance, gentle shifting of posture, breathing naturally, hair strands catching subtle air currents"
        elif act == "swimming":
            action_desc = f"gracefully emerging from the crystal water, water droplets glistening on {pronoun} skin, wet hair slicked back with cinematic clarity"
        elif act == "dancing":
            action_desc = f"performing fluid expressive dance movements, rhythmic body choreography, fabric swirling in sync with motion"
        else:
            action_desc = f"looking towards the horizon then shifting gaze towards camera with an authentic captivating expression, soft wind parting {pronoun} hair"

        subject_details = f"{subject_noun} {attire_str}, ultra-detailed skin textures, lifelike facial features, realistic catchlights in the eyes"
        dynamic_action = action_desc

    elif subj in ["sports_car", "motorcycle"]:
        vehicle_type = "high-performance sports car" if subj == "sports_car" else "custom sleek superbike"
        if act == "driving" or act == "running":
            action_desc = "roaring down the asphalt at high speed, alloy wheels spinning with realistic motion blur, exhaust heat shimmering in the wake"
        else:
            action_desc = "cruising smoothly with low-slung aggressive stance, aerodynamic body curves gleaming, tires cutting cleanly through the surface"
        
        subject_details = f"a pristine {vehicle_type}, deep glossy automotive reflections, intricate carbon fiber textures, luminous LED headlights"
        dynamic_action = action_desc

    elif subj in ["dog", "cat", "horse", "bird", "fantasy_creature"]:
        creature_map = {
            "dog": ("an energetic loyal dog", "playfully moving with wagging tail and alert curious eyes, silky fur swaying with every step"),
            "cat": ("a graceful domestic cat", "moving with whisper-quiet feline stealth, twitching whiskers and luminous reflective pupils"),
            "horse": ("a majestic thoroughbred horse", "galloping with powerful muscular stride, flowing mane and tail fluttering gracefully in the wind"),
            "bird": ("a majestic bird with wings outstretched", "soaring through the air currents with effortless wingbeats, sharp penetrating gaze"),
            "fantasy_creature": ("a formidable mythical dragon", "slowly unfurling its immense wings, embers and ethereal heat rising from its scales, powerful serpentine motion")
        }
        creature_name, creature_action = creature_map.get(subj, ("a lifelike creature", "moving naturally"))
        subject_details = f"{creature_name}, ultra-fine fur and feather rendering, authentic anatomy, vibrant lifelike eyes"
        dynamic_action = creature_action

    elif subj == "aircraft":
        subject_details = "a streamlined aerodynamic aircraft cutting through atmospheric layers, heat exhaust trails shimmering"
        dynamic_action = "banking smoothly into a sweeping turn, condensation vortices swirling gracefully off the wingtips"

    elif subj == "robot":
        subject_details = "an advanced futuristic humanoid cyborg, intricate joint hydraulics, glowing internal conduits, weathered metallic alloy plating"
        dynamic_action = "stepping forward with deliberate mechanical precision, optic sensors refocusing with a subtle luminescent pulse"

    elif subj == "cosmic":
        subject_details = "a mesmerizing cosmic spectacle, luminous celestial dust clusters, iridescent galactic rings"
        dynamic_action = "swirling nebular clouds slowly revolving with awe-inspiring astronomical motion, stellar radiance pulsating across deep space"

    elif subj == "fire_elemental":
        subject_details = "intense kinetic inferno energy, brilliant incandescent orange and crimson flame tendrils"
        dynamic_action = "roaring upward in a dynamic vortex, glowing embers drifting and sparkling through the air with fluid turbulence"

    elif subj == "water_elemental":
        subject_details = "pure crystalline water fluid dynamics, microscopic bubbles, pristine refraction and light caustics"
        dynamic_action = "surging forward in powerful rhythmic fluid motion, delicate splashes bursting into fine airborne droplets"

    elif subj == "food_drink":
        subject_details = "an artisan gastronomic presentation, tantalizing rich textures, immaculate plating and culinary detail"
        dynamic_action = "gentle wisps of aromatic steam rising slowly into the air, glistening fresh sauce catching warm specular light highlights"

    else:
        # Pure landscape / environment
        subject_details = "a breathtaking expansive landscape filled with depth, rich organic textures, and cinematic scope"
        if setting == "beach":
            dynamic_action = "rhythmic ocean waves cresting and gently breaking onto the glistening wet shoreline, fine sea foam receding over polished stones"
        elif setting == "waterfall":
            dynamic_action = "cascading sheets of turquoise water tumbling down the rocky precipice into a mist-shrouded pool, ethereal water spray floating in the air"
        elif setting == "mountain":
            dynamic_action = "wisps of alpine cloud sweeping across jagged peaks, golden light shifting across the snow ridges"
        elif setting == "city":
            dynamic_action = "smooth urban traffic streams, reflective skyscrapers towering overhead, ambient city life pulsing below"
        elif setting == "space":
            dynamic_action = "stellar cosmic dust drifting majestically, vibrant gaseous shockwaves expanding across the stellar void, distant pulsars pulsing"
        elif setting == "desert":
            dynamic_action = "wind-swept golden sands rippling across majestic towering dunes, shimmering thermal heat waves on the horizon"
        elif setting == "underwater":
            dynamic_action = "sunbeams piercing deep into turquoise water, schooling fish weaving past vibrant swaying corals, floating crystalline air bubbles"
        elif setting in ["cozy_interior", "cafe"]:
            dynamic_action = "ambient dust motes dancing in warm sunbeams, gentle steam drifting upward, warm relaxing atmosphere"
        else:
            dynamic_action = "leaves gently trembling in the breeze, shifting light rays illuminating the foreground with organic motion"

    # --- Setting & Atmospheric Lighting Details ---
    setting_descs = {
        "beach": "on a sun-drenched golden coast, soft ambient sea spray, distant horizon meeting the ocean",
        "forest": "deep within an ancient lush woodland, towering trees with dappled sunbeams filtering through the foliage",
        "mountain": "set against majestic snow-capped alpine peaks, crisp thin mountain air and expansive panoramic grandeur",
        "city": "in a vibrant metropolitan downtown avenue, sleek architecture, reflective glass facades",
        "cozy_interior": "inside an intimate warmly lit architectural interior, soft ambient textures, cozy luxurious vibe",
        "cafe": "at a charming boutique Parisian-style cafe, warm indoor glow, delicate steam rising from a porcelain cup",
        "studio": "in a high-end minimalist film studio, clean infinity backdrop, master key and fill lighting",
        "space": "in the cosmic void overlooking a shimmering nebula, distant celestial stars and planetary curvature",
        "pool": "by the edge of a luxury infinity pool, crystalline turquoise water caressed by warm ambient light",
        "highway": "along a winding scenic coastal highway, pristine asphalt stretching toward the horizon",
        "desert": "in an expansive golden desert expanse under a vast open sky, timeless wind patterns sculpted in the dunes",
        "underwater": "submerged in an enchanting aquatic wonderland, sunlight filtering through clear blue waters"
    }
    setting_text = setting_descs.get(setting, "in an authentic beautifully composed environment")

    # Lighting / Weather nuance
    light_map = {
        "sunset": "bathed in warm golden hour twilight, amber and crimson gradients across the sky, long cinematic shadows",
        "sunrise": "illuminated by gentle crisp dawn light, soft pink and golden hues breaking through morning haze",
        "rain": "captured under atmospheric rainfall, glistening wet surfaces, shimmering puddle reflections, delicate falling rain droplets",
        "snow": "in a gentle winter snowfall, soft flakes drifting through the air, crisp diffused cold illumination",
        "night": "set in moody nocturnal lighting, soft chiaroscuro contrast, subtle cinematic blue rim light",
        "neon": "surrounded by vibrant volumetric neon glow, cyan and magenta rim highlights, rich specular reflections",
        "fog": "wrapped in ethereal morning mist, soft diffused volumetric light beams slicing through the fog",
        "sunny": "illuminated by natural bright sunshine, crisp specular highlights, vibrant saturated color palette",
        "cinematic_soft": "balanced with three-point master studio lighting, subtle rim illumination, soft flattering contrast"
    }
    lighting_text = light_map.get(wl, "cinematic natural lighting, soft shadow roll-off")

    # --- Camera Framing & Lens according to Style ---
    camera_map = {
        "cinematic": "captured on 35mm Arri Alexa camera, 50mm anamorphic prime lens, smooth fluid gimbal tracking shot, shallow depth of field, 24fps filmic cadence",
        "photorealistic": "shot on Sony A7R V with 85mm f/1.4 G-Master lens, pristine 8k optical clarity, authentic bokeh falloff, natural organic color science",
        "anime": "Makoto Shinkai artistic anime movie aesthetic, lush hand-painted background textures, vibrant emotional sky gradients, sparkling dust motes",
        "scifi": "cinematic cyberpunk atmosphere, anamorphic 2.39:1 widescreen framing, subtle chromatic aberration, volumetric fog, Unreal Engine 5 movie grade",
        "drone": "breathtaking sweeping 4K aerial drone shot, steady panoramic orbit, expansive wide-angle field of view, cinematic downward tilt"
    }
    camera_text = camera_map.get(style, camera_map["cinematic"])

    return subject_details, dynamic_action, f"{setting_text}, {lighting_text}, {camera_text}"


def enhance_user_prompt(raw_prompt: str, style: str = "cinematic") -> Dict[str, str]:
    """Generates a contextual, non-repetitive video prompt faithfully derived from user input."""
    raw = raw_prompt.strip()
    if not raw:
        return {"enhanced_prompt": "", "suggested_negative": ""}

    ctx = detect_context(raw)
    subject_details, dynamic_action, camera_env = synthesize_motion_and_camera(ctx, style)

    # Clean raw text from trailing commas or periods
    clean_raw = re.sub(r"[,.\s]+$", "", raw)

    # Compose fluent, cohesive cinematic paragraph
    enhanced = f"{clean_raw}. Featuring {subject_details}. {dynamic_action.capitalize()}. Set {camera_env}, masterpiece quality, highly detailed fluid motion."

    # Contextual Negative Prompt based on detected entities
    subj = ctx["subject"]
    base_negative = [
        "low quality", "blurry", "distorted", "flickering artifacts", "watermark",
        "text", "grainy", "jittery motion", "pixelated", "overexposed"
    ]

    if subj in ["woman", "man"]:
        base_negative.extend([
            "bad anatomy", "deformed fingers", "extra hands", "missing limbs",
            "plastic doll skin", "uncanny valley", "unnatural eyes", "wax figure look", "static portrait"
        ])
    elif subj in ["sports_car", "motorcycle"]:
        base_negative.extend([
            "static wheels", "deformed car body", "crooked chassis", "unnatural speed physics", "toy car look"
        ])
    elif subj in ["dog", "cat", "horse", "bird", "fantasy_creature"]:
        base_negative.extend([
            "mutated legs", "extra tails", "distorted snout", "stiff unnatural animation", "amateur render"
        ])
    elif subj in ["aircraft", "robot"]:
        base_negative.extend([
            "deformed mechanics", "static propellers", "unrealistic physics", "amateur 3d model"
        ])
    else:
        base_negative.extend([
            "static still photograph", "jerky panning", "muddy textures", "unrealistic lighting"
        ])

    if style != "anime":
        base_negative.extend(["cartoon", "3d animation", "cgi illustration", "drawing"])

    negative_str = ", ".join(dict.fromkeys(base_negative))  # Unique preserve order

    return {
        "enhanced_prompt": enhanced,
        "suggested_negative": negative_str,
        "detected_subject": subj,
        "detected_setting": ctx["setting"]
    }
