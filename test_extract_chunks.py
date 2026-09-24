import os
import struct
from parse_fl_events import parse_fl_events
from test_roundtrip import encode_fl_events

# Sources of proven, working VST states on user's FL Studio:
p_ts = r"C:\Users\alfaswz\Documents\Image-Line\FL Studio\Presets\Mixer presets\2026 TUNE TS - IA.fst"
p_voces = r"C:\Users\alfaswz\Documents\Image-Line\FL Studio\Presets\Mixer presets\VOCES TUNE + FRESH AIR - ABRIL 2026.fst"
p_jaf = r"C:\Users\alfaswz\Documents\Image-Line\FL Studio\Presets\Mixer presets\JAF - voz final junio.fst"

evs_ts = parse_fl_events(open(p_ts, "rb").read())
evs_voces = parse_fl_events(open(p_voces, "rb").read())
evs_jaf = parse_fl_events(open(p_jaf, "rb").read())

# Extract exact working VST chunks:
# 1. Antares Auto-Tune from p_ts (event 14, len 4467)
chunk_autotune = evs_ts[14][2]

# 2. FabFilter Pro-Q 4 from p_voces (event 124, len 6162)
chunk_proq4 = evs_voces[124][2]

# 3. Waves CLA Vocals from p_jaf (event 131, len 1680)
chunk_cla_vocals = evs_jaf[131][2]

# 4. Waves Renaissance Vox (RVox) from p_voces (event 117, len 1294)
chunk_rvox = evs_voces[117][2]

# 5. Waves CLA-2A / Sibilant from p_voces (event 138, len 6037)
chunk_cla2a = evs_voces[138][2]

# 6. FabFilter Pro-DS from p_voces (event 145, len 1239)
chunk_prods = evs_voces[145][2]

# 7. Slate Digital Fresh Air from p_voces (event 131, len 11371)
chunk_fresh_air = evs_voces[131][2]

# 8. Fruity Blood Overdrive from p_ts (event 36, len 36)
chunk_overdrive = evs_ts[36][2]

# 9. ValhallaSupermassive / Reverb from p_ts (event 50, len 1349)
chunk_reverb = evs_ts[50][2]

# 10. Fruity Limiter from p_ts (event 29, len 169)
chunk_limiter = evs_ts[29][2]

print("All 10 working chunks extracted successfully!")
print("  1. Antares Auto-Tune:", len(chunk_autotune))
print("  2. FabFilter Pro-Q 4:", len(chunk_proq4))
print("  3. Waves CLA Vocals:", len(chunk_cla_vocals))
print("  4. Waves RVox:", len(chunk_rvox))
print("  5. Waves CLA-2A:", len(chunk_cla2a))
print("  6. FabFilter Pro-DS:", len(chunk_prods))
print("  7. Slate Fresh Air:", len(chunk_fresh_air))
print("  8. Blood Overdrive:", len(chunk_overdrive))
print("  9. Reverb/Space:", len(chunk_reverb))
print(" 10. Fruity Limiter:", len(chunk_limiter))
