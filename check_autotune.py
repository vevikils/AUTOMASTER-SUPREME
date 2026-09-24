import os

db_dir = r"C:\Users\alfaswz\Documents\Image-Line\FL Studio\Presets\Plugin database\Installed"
for root, dirs, files in os.walk(db_dir):
    for f in files:
        if "auto" in f.lower() or "tune" in f.lower() or "antares" in f.lower() or "cla" in f.lower():
            print(os.path.join(root, f))
