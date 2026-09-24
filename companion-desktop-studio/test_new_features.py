"""Verification script for Companion Studio upgrades
Tests clean empty state, persona creation, and photo album synthesis.
"""

import os
import sys

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
if BASE_DIR not in sys.path:
    sys.path.insert(0, BASE_DIR)

from engine.persona_manager import PersonaManager

def test():
    print("=== TEST COMPANION STUDIO UPGRADES ===")
    data_dir = os.path.join(BASE_DIR, "data")
    pm = PersonaManager(data_dir)

    # 1. Verify clean empty state (no default profiles)
    personas = pm.get_personas()
    print(f"[1] Perfiles cargados: {len(personas)}")
    assert len(personas) == 0, f"Expected 0 personas, got {len(personas)}"
    print("    [OK] Estado limpio verificado: perfiles predeterminados eliminados.")

    # 2. Test creating a custom persona with rich description
    new_data = {
        "name": "Valentina",
        "tagline": "Fotografa y amante de los viajes en tren",
        "style": "photorealistic",
        "avatar": "/ui/avatars/default.jpg",
        "appearance": {
            "hair": "long wavy copper ginger hair with soft bangs",
            "eyes": "deep emerald green eyes with expressive gaze",
            "outfit": "emerald knit sweater and delicate silver necklace",
            "features": "sweet charming smile with natural cute freckles"
        },
        "personality": {
            "archetype": "Aventurera y carinosa",
            "tone": "dulce, apasionada por la fotografia y cercana",
            "backstory": "Nacio en Barcelona, viaja constantemente con su camara analogica buscando rincones con luz especial.",
            "greeting": "Hola! Justo estaba revelando unos negativos de mi ultimo viaje. Que tal estas?",
            "hobbies": ["Fotografia analogica", "Cafe", "Viajes"]
        }
    }

    created = pm.create_persona(new_data)
    print(f"[2] Creada companera: {created['name']} (ID: {created['id']})")
    assert created["name"] == "Valentina"
    assert "ginger" in created["appearance"]["hair"]
    print("    [OK] Creacion y persistencia de perfil verificadas.")

    # 3. Test album prompt synthesis based on description
    album_prompts = pm.get_album_prompts(created)
    print(f"[3] Escenarios de album sintetizados ({len(album_prompts)} fotos):")
    for idx, sc in enumerate(album_prompts):
        print(f"    - Foto {idx+1}: {sc['title']}")
        print(f"      Prompt: {sc['prompt'][:100]}...")
        # Verify the prompt contains her specific hair and eyes
        assert "ginger" in sc["prompt"]
        assert "emerald" in sc["prompt"]
        assert "Valentina" in sc["prompt"]

    print("    [OK] Sintesis de prompts contextuales verificada: todas las fotos incorporan su descripcion fisica.")

    # 4. Clean up test persona so user starts with a clean slate
    pm.delete_persona(created["id"])
    assert len(pm.get_personas()) == 0
    print("[4] Limpieza final verificada: sistema 100% listo y limpio.")
    print("\nTODOS LOS TESTS PASARON EXITOSAMENTE.")

if __name__ == "__main__":
    test()
