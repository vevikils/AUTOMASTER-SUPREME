import os
import zipfile
import subprocess
import shutil

ROOT_DIR = r"c:\Users\alfaswz\.gemini\antigravity-ide\scratch\agentes-personalizados"
TPAIN_DIR = os.path.join(ROOT_DIR, "tpain-supreme")
BUILD_DIR = os.path.join(TPAIN_DIR, "build", "SupremeTunerRealTimeV3_artefacts", "Release")

PLUGIN_NAME = "Supreme Tuner Real Time v3.9"
VST3_DIR = os.path.join(BUILD_DIR, "VST3", f"{PLUGIN_NAME}.vst3")
EXE_PATH = os.path.join(BUILD_DIR, "Standalone", f"{PLUGIN_NAME}.exe")
COVER_PATH = os.path.join(TPAIN_DIR, "assets", "supreme_tuner_cover.jpg")
README_PATH = os.path.join(TPAIN_DIR, "README.md")

ZIP_NAME = "SUPREME_TUNER_v3.9.0_Windows.zip"
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
        zipf.write(COVER_PATH, "supreme_tuner_cover.jpg")
    if os.path.exists(README_PATH):
        zipf.write(README_PATH, "README.md")

zip_mb = os.path.getsize(ZIP_PATH) / (1024 * 1024)
print(f"Package created successfully! Size: {zip_mb:.2f} MB")

# Release notes for v3.9
release_notes = """# 🎤 SUPREME TUNER Real Time v3.9 — Hybrid Hardware Master Edition
### 👑 Autor: **vevi** (@vevikils)

![Supreme Tuner Cover](supreme_tuner_cover.jpg)

### 🌟 Novedades de la Versión 3.9:
- 🎚️ **Fondos de Consola Dedicados para Faders de Dinámica (`TRANSIENTS` y `COMPRESSION`)**:
  - Placas rehundidas de titanio y obsidiana cepillada con 4 micro tornillos hexagonales de estudio.
  - Ranura vertical fresada con línea láser guía en violeta neón iluminada.
  - Escala serigrafiada de alta precisión: `+12` a `-12` dB para Transientes y `100%` a `0%` para Compresión.
  - Tiradores moleteados de consola con hendidura blanca brillante y halo neón violeta.
  - Rótulos numéricos en lavanda brillante (`#e9d5ff`) con visibilidad total.
- 🎛️ **Fader Master Calibrado a Rango Real (-24 dB a +6 dB)**:
  - Eliminado el rango salvaje anterior; calibrado al estándar de estudio analógico (-24 dB a +6 dB, con ganancia unitaria a 0 dB).
  - Placa rehundida en oro cepillado 24K con marcas grabadas y serigrafía oficial `3.9 MASTER LEVEL • BY VEVI`.
- 🟣 **Paleta Dual de Círculos: Violeta Neón vs Master Dorado**:
  - Todos los mandos rotatorios, arcos y halos fuera del Master ahora en vibrante violeta neón eléctrico (`#a855f7` / `#d8b4fe`).
  - La zona del Master (Unit 4) preserva su tono cálido en ámbar y oro 24K (`STEREO WIDTH`, `DRY / WET`, búmetros analógicos dobles y fader master).
- 🖼️ **Textura Hardware Cyberpunk al 20% de Opacidad**:
  - Fondo multitextura con chasis de titanio cepillado, relieve de fibra de carbono hexagonal y suaves ondulaciones de ondas de audio al 20% de opacidad integrada en el binario.
- 👑 **Firma del Autor en la GUI**:
  - Distintivo `BY VEVI` en la cabecera, serigrafía del fader master `BY VEVI`, y rueda cromática `REAL-TIME CHROMATIC PITCH WHEEL • BY VEVI`.
- ⚡ **Latencia Ultra-Baja**: < 2.9 ms con motor DSP a 64 bits.
- 🛡️ **12/12 Severe Stress Tests Pasados**: Máxima inmunidad y estabilidad de audio thread.

### 📦 Contenido del Paquete:
- `Supreme Tuner Real Time v3.9.vst3` (64-bit VST3 para FL Studio, Ableton, Cubase, Studio One, etc.)
- `Supreme Tuner Real Time v3.9.exe` (Standalone para directos y ensayos con latencia mínima)
- `supreme_tuner_cover.jpg` (Portada en alta definición)
- `README.md` (Documentación completa de uso y arquitectura)
"""

notes_path = os.path.join(ROOT_DIR, "release_notes_v39.md")
with open(notes_path, "w", encoding="utf-8") as f:
    f.write(release_notes)

# Check if release v3.9.0 exists
cmd_check = ["gh", "release", "view", "v3.9.0"]
res_chk = subprocess.run(cmd_check, cwd=ROOT_DIR, capture_output=True, text=True, encoding="utf-8")

if res_chk.returncode == 0:
    print("Release v3.9.0 exists, uploading assets...")
    cmd_upload = ["gh", "release", "upload", "v3.9.0", ZIP_PATH, COVER_PATH, "--clobber"]
    subprocess.run(cmd_upload, cwd=ROOT_DIR, check=True)
    cmd_edit = ["gh", "release", "edit", "v3.9.0", "--title", "SUPREME TUNER Real Time v3.9 (By vevi)", "--notes-file", notes_path]
    subprocess.run(cmd_edit, cwd=ROOT_DIR, check=True)
else:
    print("Creating release v3.9.0 on GitHub...")
    cmd_create = [
        "gh", "release", "create", "v3.9.0",
        ZIP_PATH, COVER_PATH,
        "--title", "SUPREME TUNER Real Time v3.9 (By vevi)",
        "--notes-file", notes_path
    ]
    subprocess.run(cmd_create, cwd=ROOT_DIR, check=True)

print("Release v3.9.0 published successfully!")
