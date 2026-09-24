import os
import zipfile
import subprocess
import shutil

ROOT_DIR = r"c:\Users\alfaswz\.gemini\antigravity-ide\scratch\agentes-personalizados"
TPAIN_DIR = os.path.join(ROOT_DIR, "tpain-supreme")
BUILD_DIR = os.path.join(TPAIN_DIR, "build", "SupremeTuneRealTimeV4_artefacts", "Release")

PLUGIN_NAME = "Supreme Tune Real Time v4.0"
VST3_DIR = os.path.join(BUILD_DIR, "VST3", f"{PLUGIN_NAME}.vst3")
EXE_PATH = os.path.join(BUILD_DIR, "Standalone", f"{PLUGIN_NAME}.exe")
COVER_PATH = os.path.join(TPAIN_DIR, "assets", "supreme_tuner_cover.jpg")
README_PATH = os.path.join(TPAIN_DIR, "README.md")

ZIP_NAME = "SUPREME_TUNE_v4.0.0_Windows.zip"
ZIP_PATH = os.path.join(ROOT_DIR, ZIP_NAME)

print(f"Creating release package: {ZIP_PATH}...")
if os.path.exists(ZIP_PATH):
    os.remove(ZIP_PATH)

with zipfile.ZipFile(ZIP_PATH, "w", zipfile.ZIP_DEFLATED) as zipf:
    # 1. Add Standalone exe
    if os.path.exists(EXE_PATH):
        print(f"Adding Standalone exe: {EXE_PATH}")
        zipf.write(EXE_PATH, f"{PLUGIN_NAME}.exe")
    else:
        print(f"WARNING: Standalone exe not found at {EXE_PATH}")

    # 2. Add VST3 bundle directory
    if os.path.exists(VST3_DIR):
        print(f"Adding VST3 bundle from {VST3_DIR}...")
        for root, dirs, files in os.walk(VST3_DIR):
            for file in files:
                full_path = os.path.join(root, file)
                rel_path = os.path.relpath(full_path, os.path.dirname(VST3_DIR))
                zipf.write(full_path, rel_path)
    else:
        print(f"WARNING: VST3 bundle not found at {VST3_DIR}")

    # 3. Add Cover and Readme
    if os.path.exists(COVER_PATH):
        zipf.write(COVER_PATH, "supreme_tune_cover.jpg")
    if os.path.exists(README_PATH):
        zipf.write(README_PATH, "README.md")

zip_mb = os.path.getsize(ZIP_PATH) / (1024 * 1024)
print(f"Package created successfully! Size: {zip_mb:.2f} MB")
