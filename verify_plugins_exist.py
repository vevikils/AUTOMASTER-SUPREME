import os

vst3_db = r"C:\Users\alfaswz\Documents\Image-Line\FL Studio\Presets\Plugin database\Installed\Effects\VST3"

required_plugins = [
    "Auto-Tune Pro.fst",
    "Pro-Q 4.fst",
    "CLA-76 Stereo.fst",
    "CLA-2A Stereo.fst",
    "Pro-DS.fst",
    "Saturn 2.fst",
    "RVox Stereo.fst",
    "REVERB SUPREME PRO.fst",
    "Pro-C 2.fst"
]

print("Checking presence of required plugin presets:")
for p in required_plugins:
    full = os.path.join(vst3_db, p)
    exists = os.path.exists(full)
    sz = os.path.getsize(full) if exists else 0
    print(f"  {p:25s}: {'EXISTS (' + str(sz) + ' B)' if exists else 'NOT FOUND'}")
