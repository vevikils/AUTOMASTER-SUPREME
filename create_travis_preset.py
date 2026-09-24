import os
import struct
from parse_fl_events import parse_fl_events
from test_roundtrip import encode_fl_events

vst3_db = r"C:\Users\alfaswz\Documents\Image-Line\FL Studio\Presets\Plugin database\Installed\Effects\VST3"

# 10 slots in sequence
chain_plugins = [
    ("Auto-Tune Pro.fst", "Auto-Tune Pro"),
    ("Pro-Q 4.fst", "FabFilter Pro-Q 4 (Pre-EQ)"),
    ("CLA-76 Stereo.fst", "Waves CLA-76 (Peak Comp)"),
    ("CLA-2A Stereo.fst", "Waves CLA-2A (Opto Leveler)"),
    ("Pro-DS.fst", "FabFilter Pro-DS (De-Esser)"),
    ("Saturn 2.fst", "FabFilter Saturn 2 (Saturation)"),
    ("RVox Stereo.fst", "Waves RVox (Vocal Gate/Comp)"),
    ("Pro-Q 4.fst", "FabFilter Pro-Q 4 (Air & Polish)"),
    ("REVERB SUPREME PRO.fst", "REVERB SUPREME PRO (Spatial)"),
    ("Pro-C 2.fst", "FabFilter Pro-C 2 (Glue/Limiter)")
]

def load_plugin_data(filename):
    path = os.path.join(vst3_db, filename)
    evs = parse_fl_events(open(path, "rb").read())
    ev201 = None
    ev203 = None
    ev213 = None
    for e in evs:
        if e[0] == 201:
            ev201 = e[2]
        elif e[0] == 203:
            ev203 = e[2]
        elif e[0] == 213:
            ev213 = e[2]
    return ev201, ev203, ev213

# Load Default.fst to get baseline mixer track envelope / routing data
default_mixer = r"C:\Program Files\Image-Line\FL Studio 2026\Data\Patches\Mixer presets\Default.fst"
def_evs = parse_fl_events(open(default_mixer, "rb").read())

# Mixer routing table (Event 225)
ev225 = None
for e in def_evs:
    if e[0] == 225:
        ev225 = e[2]
        break

# Track color: Travis Scott Flame Amber / Neon Cyan (0x0028A0F5)
track_color = 0x0028A0F5

# Build complete mixer track event list
master_events = []
master_events.append((199, "VAR", b'8.0.0\x00'))
master_events.append((149, "DWORD", track_color))
master_events.append((204, "VAR", b'TRAVIS SCOTT VOCAL\x00'))
master_events.append((27, "BYTE", 15)) # Microphone icon
master_events.append((236, "VAR", b'\x00\x00\x00\x00\x01'))

for slot_idx, (filename, display_name) in enumerate(chain_plugins, start=1):
    ev201, ev203, ev213 = load_plugin_data(filename)
    
    # 52 bytes Event 212
    # int0=0, int1=slot_idx, int2=2 (enabled), int3=0, int4=64 (100% mix), int5..7=0, int8=16328357
    x_pos = 100 + (slot_idx * 30)
    y_pos = 150 + (slot_idx * 20)
    w_pos = 600
    h_pos = 400
    raw212 = struct.pack("<IIIIIIIIIIIII", 0, slot_idx, 2, 0, 64, 0, 0, 0, 16328357, x_pos, y_pos, w_pos, h_pos)
    
    # Custom display name in UTF-16LE
    custom_name_utf16 = display_name.encode('utf-16le') + b'\x00\x00'
    
    master_events.append((201, "VAR", ev201))
    master_events.append((212, "VAR", raw212))
    master_events.append((203, "VAR", custom_name_utf16))
    master_events.append((213, "VAR", ev213))

# Append mixer routing table
if ev225 is not None:
    master_events.append((225, "VAR", ev225))

# Encode to binary
flhd_payload = open(default_mixer, "rb").read()[8:14]
binary_data = encode_fl_events(master_events, flhd_payload=flhd_payload)

print(f"Generated TRAVIS SCOTT VOCAL CHAIN preset: {len(binary_data)} bytes")

# Save to destination folders
dest_paths = [
    r"C:\Users\alfaswz\Desktop\TRAVIS SCOTT - VOCAL CHAIN (PRO).fst",
    r"C:\Users\alfaswz\Documents\Image-Line\FL Studio\Presets\Mixer presets\TRAVIS SCOTT - VOCAL CHAIN (PRO).fst",
    r"C:\Users\alfaswz\Documents\Image-Line\FL Studio\Presets\Mixer presets\Vocals\TRAVIS SCOTT - VOCAL CHAIN (PRO).fst",
    r"C:\Program Files\Image-Line\FL Studio 2026\Data\Patches\Mixer presets\TRAVIS SCOTT - VOCAL CHAIN (PRO).fst",
    r"C:\Program Files\Image-Line\FL Studio 2025\Data\Patches\Mixer presets\TRAVIS SCOTT - VOCAL CHAIN (PRO).fst"
]

for dp in dest_paths:
    parent = os.path.dirname(dp)
    try:
        if not os.path.exists(parent):
            os.makedirs(parent, exist_ok=True)
        with open(dp, "wb") as f:
            f.write(binary_data)
        print("Successfully saved to:", dp)
    except Exception as e:
        print(f"Could not write to {dp}: {e}")
