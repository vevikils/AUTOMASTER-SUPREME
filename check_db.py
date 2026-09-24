import os

paths = [
    r"C:\Users\alfaswz\Documents\Image-Line\FL Studio\Presets\Plugin database",
    r"C:\Program Files\Image-Line\FL Studio 2026\Data\Patches\Plugin database",
    r"C:\Program Files\Image-Line\FL Studio 2025\Data\Patches\Plugin database"
]

for p in paths:
    if os.path.exists(p):
        print("FOUND DB PATH:", p)
        for root, dirs, files in os.walk(p):
            for f in files:
                if any(x in f.lower() for x in ["auto", "tune", "pro-q", "pro-c", "pro-ds", "saturn", "cla", "rvox", "waves", "reverb supreme"]):
                    print("  ", os.path.join(root, f))
