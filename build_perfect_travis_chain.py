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

# Define the 10 slots
# Each slot is: (is_wrapper, name_utf16, internal_id, chunk)
WRAPPER_NAME = "Fruity Wrapper".encode('utf-16le') + b'\x00\x00'

slots = [
    # Slot 0: Antares Auto-Tune Pro
    (True, WRAPPER_NAME, 6972764, evs_ts[14][2]),
    # Slot 1: FabFilter Pro-Q 4
    (True, WRAPPER_NAME, 3158064, evs_voces[124][2]),
    # Slot 2: Waves CLA Vocals
    (True, WRAPPER_NAME, 3158064, evs_jaf[131][2]),
    # Slot 3: Waves RVox (Renaissance Vox)
    (True, WRAPPER_NAME, 3158064, evs_voces[117][2]),
    # Slot 4: Waves CLA-2A
    (True, WRAPPER_NAME, 3158064, evs_voces[138][2]),
    # Slot 5: FabFilter Pro-DS (De-Esser)
    (True, WRAPPER_NAME, 3158064, evs_voces[145][2]),
    # Slot 6: Slate Digital Fresh Air
    (True, WRAPPER_NAME, 3158064, evs_voces[131][2]),
    # Slot 7: Fruity Blood Overdrive (Analog Saturation)
    (False, "Fruity Blood Overdrive".encode('utf-16le') + b'\x00\x00', 5656904, evs_ts[36][2]),
    # Slot 8: Valhalla Space Reverb (or Reverb)
    (True, WRAPPER_NAME, 6972764, evs_ts[50][2]),
    # Slot 9: Fruity Limiter (Mastering/Bus dynamics)
    (False, "Fruity Limiter".encode('utf-16le') + b'\x00\x00', 5656904, evs_ts[29][2])
]

# Build the complete FL event list matching user's working FL 24.2 format
master_events = []

# Header from 2026 TUNE TS - IA.fst
master_events.append((199, "VAR", b'24.2.2.4597\x00'))
master_events.append((159, "DWORD", 4597))
master_events.append((169, "DWORD", 5))
master_events.append((28, "BYTE", 1)) # UNICODE STRINGS!
master_events.append((37, "BYTE", 1))
master_events.append((149, "DWORD", 0x0028A0F5)) # Flame Gold/Orange
master_events.append((42, "BYTE", 1))
master_events.append((204, "VAR", "TRAVIS SCOTT VOCAL".encode('utf-16le') + b'\x00\x00'))
master_events.append((236, "VAR", b'\x00\x00\x00\x00L \x00\x00\x00\x00\x00\x00'))

for slot_0_idx, (is_wrapper, name_utf16, internal_id, chunk) in enumerate(slots):
    slot_1_idx = slot_0_idx + 1
    x = 350 + (slot_0_idx * 25)
    y = 120 + (slot_0_idx * 20)
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

# Footer routing table (Event 225)
master_events.append((225, "VAR", evs_ts[-1][2]))

# FLhd chunk header
flhd_payload = open(p_ts, "rb").read()[8:14]
binary_preset = encode_fl_events(master_events, flhd_payload=flhd_payload)

print(f"Generated clean Travis Scott preset: {len(binary_preset)} bytes")

dest_paths = [
    r"C:\Users\alfaswz\Desktop\TRAVIS SCOTT - VOCAL CHAIN (PRO).fst",
    r"C:\Users\alfaswz\Documents\Image-Line\FL Studio\Presets\Mixer presets\TRAVIS SCOTT - VOCAL CHAIN (PRO).fst",
    r"C:\Users\alfaswz\Documents\Image-Line\FL Studio\Presets\Mixer presets\Vocals\TRAVIS SCOTT - VOCAL CHAIN (PRO).fst"
]

for dp in dest_paths:
    os.makedirs(os.path.dirname(dp), exist_ok=True)
    with open(dp, "wb") as f:
        f.write(binary_preset)
    print("SUCCESS: Written to", dp)
