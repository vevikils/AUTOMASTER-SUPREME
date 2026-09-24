import os
import zipfile
import subprocess
import shutil

ROOT_DIR = r"c:\Users\alfaswz\.gemini\antigravity-ide\scratch\agentes-personalizados"
TPAIN_DIR = os.path.join(ROOT_DIR, "tpain-supreme")
BUILD_DIR = os.path.join(TPAIN_DIR, "build", "SupremeTunerRealTimeV3_artefacts", "Release")

VST3_DIR = os.path.join(BUILD_DIR, "VST3", "Supreme Tuner Real Time v3.5.vst3")
EXE_PATH = os.path.join(BUILD_DIR, "Standalone", "Supreme Tuner Real Time v3.5.exe")
COVER_PATH = os.path.join(TPAIN_DIR, "assets", "supreme_tuner_cover.jpg")
README_PATH = os.path.join(TPAIN_DIR, "README.md")

ZIP_NAME = "SUPREME_TUNER_v3.5.0_Windows.zip"
ZIP_PATH = os.path.join(ROOT_DIR, ZIP_NAME)

print(f"Creating release package: {ZIP_PATH}...")
if os.path.exists(ZIP_PATH):
    os.remove(ZIP_PATH)

with zipfile.ZipFile(ZIP_PATH, "w", zipfile.ZIP_DEFLATED) as zipf:
    # 1. Add Standalone exe
    if os.path.exists(EXE_PATH):
        print(f"Adding Standalone exe: {EXE_PATH}")
        zipf.write(EXE_PATH, "Supreme Tuner Real Time v3.5.exe")
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
        zipf.write(COVER_PATH, "supreme_tuner_cover.jpg")
    if os.path.exists(README_PATH):
        zipf.write(README_PATH, "README.md")

zip_mb = os.path.getsize(ZIP_PATH) / (1024 * 1024)
print(f"Package created successfully! Size: {zip_mb:.2f} MB")

# Create GitHub Release
release_notes = """# 🎤 SUPREME TUNER Real Time v3.5: Luxury Edition

![Supreme Tuner Cover](supreme_tuner_cover.jpg)

### 🌟 Novedades Principales:
- 🟣 **Perillas Moradas de Lujo**: Todo el rack dinámico y de efectos con estética neón violeta (`#8b5cf6` a `#d946ef`), aguja de alta visibilidad con núcleo blanco y joya central iluminada.
- 🏆 **El Fader Dorado del Máster**: Control de volumen master deslizante vertical con acabado en oro 24K cepillado de consola analógica.
- 🎯 **Cero Solapamientos**: Espaciado y geometría rediseñada en la unidad de puerta de ruido y compresor.
- 🌐 **Control de Imagen Estéreo Mid/Side**: De 0% Mono a 200% Super-Wide.
- 🎚️ **Monitoreo de Ganancia con Vúmetros Peak en Vivo**: Rango de -60 dB a +6 dB con balística LED tricolor.
- 🎨 **52 Presets de Artistas**: T-Pain, Travis Scott, Drake, Bad Bunny, Rosalía, Feid, Mora, Yeat, Peso Pluma y más.
- ⚡ **Latencia Ultra-Baja**: < 2.9 ms con motor de audio a 64 bits.

### 📦 Contenido del Paquete:
- `Supreme Tuner Real Time v3.5.vst3` (64-bit VST3 para FL Studio, Ableton, Cubase, etc.)
- `Supreme Tuner Real Time v3.5.exe` (Standalone para directo y ensayos sin DAW)
- `supreme_tuner_cover.jpg` (Portada en alta definición)
- `README.md` (Guía completa de usuario y presets)
"""

notes_path = os.path.join(ROOT_DIR, "release_notes_v35.md")
with open(notes_path, "w", encoding="utf-8") as f:
    f.write(release_notes)

cmd = [
    "gh", "release", "create", "v3.5.0",
    ZIP_PATH,
    COVER_PATH,
    "--title", "SUPREME TUNER Real Time v3.5 (Luxury Edition)",
    "--notes-file", notes_path
]

print("Executing gh release create...")
res = subprocess.run(cmd, cwd=ROOT_DIR, capture_output=True, text=True, encoding="utf-8")
print("STDOUT:", res.stdout)
print("STDERR:", res.stderr)
print(f"Exit code: {res.returncode}")
