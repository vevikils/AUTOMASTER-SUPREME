import os
from parse_fl_events import parse_fl_events

paths_to_check = [
    r"C:\Program Files\Image-Line\FL Studio 2026\Data\Patches",
    r"C:\Program Files\Image-Line\FL Studio 2025\Data\Patches",
    r"C:\Users\alfaswz\Documents\Image-Line"
]

found = []
for p in paths_to_check:
    for root, dirs, files in os.walk(p):
        for f in files:
            if f.endswith(".fst"):
                full = os.path.join(root, f)
                try:
                    data = open(full, "rb").read()
                    if b"Fruity Wrapper" in data or b"F\x00r\x00u\x00i\x00t\x00y\x00 \x00W\x00" in data:
                        found.append(full)
                except Exception:
                    pass

print(f"Found {len(found)} fst files with Wrapper:")
for f in found[:30]:
    print(f)
