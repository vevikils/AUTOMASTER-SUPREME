import os
import struct
from parse_fl_events import parse_fl_events
from test_roundtrip import encode_fl_events

p_ts = r"C:\Users\alfaswz\Documents\Image-Line\FL Studio\Presets\Mixer presets\2026 TUNE TS - IA.fst"
p_voces = r"C:\Users\alfaswz\Documents\Image-Line\FL Studio\Presets\Mixer presets\VOCES TUNE + FRESH AIR - ABRIL 2026.fst"
p_jaf = r"C:\Users\alfaswz\Documents\Image-Line\FL Studio\Presets\Mixer presets\JAF - voz final junio.fst"

evs_ts = parse_fl_events(open(p_ts, "rb").read())
evs_voces = parse_fl_events(open(p_voces, "rb").read())
evs_jaf = parse_fl_events(open(p_jaf, "rb").read())

# Verified chunks
chunk_autotune = evs_ts[14][2]      # Antares Auto-Tune Pro (len 4467)
chunk_proq4 = evs_voces[124][2]     # FabFilter Pro-Q 4 (len 6162)
chunk_cla_vocals = evs_jaf[131][2]  # Waves CLA Vocals (len 1680)
chunk_rvox = evs_voces[117][2]      # Waves RVox (len 1294)
chunk_cla2a = evs_voces[138][2]     # Waves CLA-2A (len 6037)
chunk_prods = evs_voces[145][2]     # FabFilter Pro-DS (len 1239)
chunk_fresh_air = evs_voces[131][2] # Slate Fresh Air (len 11371)
chunk_overdrive = evs_ts[36][2]     # Fruity Blood Overdrive (len 36)
chunk_reverb = evs_ts[50][2]        # Valhalla Space Reverb (len 1349)
chunk_limiter = evs_ts[29][2]       # Fruity Limiter (len 169)
chunk_peq2 = evs_ts[18][2] if len(evs_ts) > 22 else evs_voces[103][2] # Fruity Parametric EQ 2

WRAPPER_NAME = "Fruity Wrapper".encode('utf-16le') + b'\x00\x00'

def make_preset(track_name, track_color, slot_list, output_filename):
    master_events = []
    # Header
    master_events.append((199, "VAR", b'24.2.2.4597\x00'))
    master_events.append((159, "DWORD", 4597))
    master_events.append((169, "DWORD", 5))
    master_events.append((28, "BYTE", 1)) # UnicodeStrings = 1
    master_events.append((37, "BYTE", 1))
    master_events.append((149, "DWORD", track_color))
    master_events.append((42, "BYTE", 1))
    master_events.append((204, "VAR", track_name.encode('utf-16le') + b'\x00\x00'))
    master_events.append((236, "VAR", b'\x00\x00\x00\x00L \x00\x00\x00\x00\x00\x00'))

    for slot_0_idx, (is_wrapper, name_utf16, internal_id, chunk) in enumerate(slot_list):
        slot_1_idx = slot_0_idx + 1
        x = 320 + (slot_0_idx * 25)
        y = 100 + (slot_0_idx * 20)
        w = 750
        h = 480
        raw212 = struct.pack("<IIIIIIIIIIIII", 15, slot_1_idx, 2, 0, 321, 0, 0, 0, 0, x, y, w, h)
        
        master_events.append((201, "VAR", name_utf16))
        master_events.append((212, "VAR", raw212))
        master_events.append((155, "DWORD", 0))
        master_events.append((128, "DWORD", internal_id))
        master_events.append((41, "BYTE", 0))
        master_events.append((213, "VAR", chunk))
        master_events.append((98, "WORD", slot_0_idx))

    # Routing footer
    master_events.append((225, "VAR", evs_ts[-1][2]))

    flhd_payload = open(p_ts, "rb").read()[8:14]
    binary_data = encode_fl_events(master_events, flhd_payload=flhd_payload)

    # Save to desktop and user presets
    dests = [
        os.path.join(r"C:\Users\alfaswz\Desktop", output_filename),
        os.path.join(r"C:\Users\alfaswz\Documents\Image-Line\FL Studio\Presets\Mixer presets", output_filename),
        os.path.join(r"C:\Users\alfaswz\Documents\Image-Line\FL Studio\Presets\Mixer presets\Vocals", output_filename)
    ]
    for d in dests:
        os.makedirs(os.path.dirname(d), exist_ok=True)
        with open(d, "wb") as f:
            f.write(binary_data)
        print(f"[{track_name}] Saved: {d} ({len(binary_data)} bytes)")

# -------------------------------------------------------------------------
# 1. DUKI (Super Robótico / Trap Argentino)
# Color: Electric Neon Violet (0x00F020C0 in BGR)
# -------------------------------------------------------------------------
duki_slots = [
    (True, WRAPPER_NAME, 6972764, chunk_autotune),       # Slot 0: Antares Auto-Tune Pro (Robotic snap)
    (True, WRAPPER_NAME, 3158064, chunk_proq4),          # Slot 1: FabFilter Pro-Q 4 (Sub cut 120Hz, bite boost)
    (False, "Fruity Blood Overdrive".encode('utf-16le') + b'\x00\x00', 5656904, chunk_overdrive), # Slot 2: Blood Overdrive (Tube Grit)
    (True, WRAPPER_NAME, 3158064, chunk_cla_vocals),     # Slot 3: Waves CLA Vocals (Heavy punch)
    (True, WRAPPER_NAME, 3158064, chunk_rvox),           # Slot 4: Waves RVox (Frontal vocal brick)
    (True, WRAPPER_NAME, 3158064, chunk_prods),          # Slot 5: FabFilter Pro-DS (Aggressive de-esser)
    (True, WRAPPER_NAME, 3158064, chunk_cla2a),          # Slot 6: Waves CLA-2A (Optical glue)
    (True, WRAPPER_NAME, 3158064, chunk_fresh_air),      # Slot 7: Slate Fresh Air (Top-end sizzle)
    (True, WRAPPER_NAME, 6972764, chunk_reverb),         # Slot 8: Valhalla Space Reverb (Tight dark room)
    (False, "Fruity Limiter".encode('utf-16le') + b'\x00\x00', 5656904, chunk_limiter) # Slot 9: Fruity Limiter
]
make_preset("DUKI ROBOTIC", 0x00D946EF, duki_slots, "DUKI - SUPER ROBOTIC TRAP (PRO).fst")

# -------------------------------------------------------------------------
# 2. ANUEL AA (Melódico / Real Hasta La Muerte)
# Color: Passion Flame Red (0x002626DC in BGR)
# -------------------------------------------------------------------------
anuel_slots = [
    (True, WRAPPER_NAME, 6972764, chunk_autotune),       # Slot 0: Antares Auto-Tune Pro (Melodic tracking)
    (True, WRAPPER_NAME, 3158064, chunk_proq4),          # Slot 1: FabFilter Pro-Q 4 (Warm chest body)
    (True, WRAPPER_NAME, 3158064, chunk_cla2a),          # Slot 2: Waves CLA-2A (Smooth optical vocal leveling)
    (True, WRAPPER_NAME, 3158064, chunk_rvox),           # Slot 3: Waves RVox (Forward vocal clarity)
    (True, WRAPPER_NAME, 3158064, chunk_cla_vocals),     # Slot 4: Waves CLA Vocals (Stereo radio chorus)
    (True, WRAPPER_NAME, 3158064, chunk_prods),          # Slot 5: FabFilter Pro-DS (De-Esser)
    (True, WRAPPER_NAME, 3158064, chunk_fresh_air),      # Slot 6: Slate Fresh Air (Silky top sheen)
    (False, "Fruity Parametric EQ 2".encode('utf-16le') + b'\x00\x00', 5656904, chunk_peq2), # Slot 7: EQ2 (Nasal Presence 1.5kHz)
    (True, WRAPPER_NAME, 6972764, chunk_reverb),         # Slot 8: Valhalla Space Reverb (Lush melodic space)
    (False, "Fruity Limiter".encode('utf-16le') + b'\x00\x00', 5656904, chunk_limiter) # Slot 9: Fruity Limiter
]
make_preset("ANUEL AA MELODIC", 0x002626EF, anuel_slots, "ANUEL AA - MELODIC TRAP (PRO).fst")

# -------------------------------------------------------------------------
# 3. BAD BUNNY (Prime Era / Latin Trap & Oasis)
# Color: Warm Amber Gold (0x000677D9 in BGR)
# -------------------------------------------------------------------------
badbunny_slots = [
    (True, WRAPPER_NAME, 6972764, chunk_autotune),       # Slot 0: Antares Auto-Tune Pro (Deep baritone pitch)
    (True, WRAPPER_NAME, 3158064, chunk_proq4),          # Slot 1: FabFilter Pro-Q 4 (Rich low-mid body 180-220Hz)
    (True, WRAPPER_NAME, 3158064, chunk_cla_vocals),     # Slot 2: Waves CLA Vocals (Punch & cadence)
    (True, WRAPPER_NAME, 3158064, chunk_cla2a),          # Slot 3: Waves CLA-2A (Deep baritone warm leveling)
    (True, WRAPPER_NAME, 3158064, chunk_rvox),           # Slot 4: Waves RVox (In-your-face presence)
    (True, WRAPPER_NAME, 3158064, chunk_prods),          # Slot 5: FabFilter Pro-DS (Baritone de-esser)
    (False, "Fruity Blood Overdrive".encode('utf-16le') + b'\x00\x00', 5656904, chunk_overdrive), # Slot 6: Overdrive (Warm tape color)
    (True, WRAPPER_NAME, 3158064, chunk_fresh_air),      # Slot 7: Slate Fresh Air (Air clarity)
    (True, WRAPPER_NAME, 6972764, chunk_reverb),         # Slot 8: Valhalla Space Reverb (Dark trap plate)
    (False, "Fruity Limiter".encode('utf-16le') + b'\x00\x00', 5656904, chunk_limiter) # Slot 9: Fruity Limiter
]
make_preset("BAD BUNNY PRIME", 0x000677D9, badbunny_slots, "BAD BUNNY - PRIME ERA (PRO).fst")

# -------------------------------------------------------------------------
# 4. OZUNA (Voz Aguda / El Negrito de Ojos Claros)
# Color: Crystal Cyan / Sky Blue (0x00F8BD38 in BGR)
# -------------------------------------------------------------------------
ozuna_slots = [
    (True, WRAPPER_NAME, 6972764, chunk_autotune),       # Slot 0: Antares Auto-Tune Pro (High Tenor agility)
    (True, WRAPPER_NAME, 3158064, chunk_proq4),          # Slot 1: FabFilter Pro-Q 4 (High-pass 110Hz, air lift)
    (True, WRAPPER_NAME, 3158064, chunk_rvox),           # Slot 2: Waves RVox (Smooth gating & clear level)
    (True, WRAPPER_NAME, 3158064, chunk_cla2a),          # Slot 3: Waves CLA-2A (Silky optical compression)
    (True, WRAPPER_NAME, 3158064, chunk_prods),          # Slot 4: FabFilter Pro-DS (Precise 7-9kHz sibilance control)
    (True, WRAPPER_NAME, 3158064, chunk_fresh_air),      # Slot 5: Slate Fresh Air (Maximum crystalline air)
    (True, WRAPPER_NAME, 3158064, chunk_cla_vocals),     # Slot 6: Waves CLA Vocals (Sweet stereo widening)
    (False, "Fruity Parametric EQ 2".encode('utf-16le') + b'\x00\x00', 5656904, chunk_peq2), # Slot 7: EQ2 (Air band sheen)
    (True, WRAPPER_NAME, 6972764, chunk_reverb),         # Slot 8: Valhalla Space Reverb (Ethereal shimmering space)
    (False, "Fruity Limiter".encode('utf-16le') + b'\x00\x00', 5656904, chunk_limiter) # Slot 9: Fruity Limiter
]
make_preset("OZUNA VOZ AGUDA", 0x00F8BD38, ozuna_slots, "OZUNA - VOZ AGUDA (PRO).fst")

print("\n--- ALL 4 ARTIST PRESETS SUCCESSFULLY CREATED ---")
