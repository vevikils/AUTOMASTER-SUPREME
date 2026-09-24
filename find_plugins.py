import os
import glob

search_paths = [
    r"C:\Program Files\Common Files\VST3",
    r"C:\Program Files\VSTPlugins",
    r"C:\Program Files (x86)\VSTPlugins",
    r"C:\Program Files\Common Files\VST2",
    r"C:\Program Files\Common Files\Avid\Audio\Plug-Ins",
    r"C:\Program Files\Antares Audio Technologies",
    r"C:\Program Files\Waves",
    r"C:\Program Files (x86)\Waves",
    r"C:\Program Files\FabFilter",
    r"C:\Program Files\Image-Line\FL Studio 2026\Data\Patches",
    r"C:\Program Files\Image-Line\FL Studio 2025\Data\Patches",
    r"C:\Users\alfaswz\Documents\Image-Line",
    r"C:\Users\alfaswz\AppData\Local",
    r"C:\Users\alfaswz\AppData\Roaming"
]

print("--- Searching for Antares, Waves, FabFilter, AutoTune ---")
keywords = ["antares", "autotune", "auto-tune", "fabfilter", "pro-q", "pro-c", "pro-ds", "waveshell", "cla-76", "cla-2a", "renaissance", "r-vox", "rvox"]

found = []
for p in search_paths:
    if not os.path.exists(p):
        continue
    for root, dirs, files in os.walk(p):
        # Limit depth for AppData
        if ("AppData" in root or "Documents" in root) and root.count(os.sep) > 7:
            continue
        for item in dirs + files:
            lower = item.lower()
            if any(k in lower for k in keywords):
                full_path = os.path.join(root, item)
                found.append(full_path)

print(f"Total items found: {len(found)}")
for item in found[:40]:
    print(item)
