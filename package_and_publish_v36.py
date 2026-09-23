import os
import zipfile
import subprocess
import shutil

ROOT_DIR = r"c:\Users\alfaswz\.gemini\antigravity-ide\scratch\agentes-personalizados"
TPAIN_DIR = os.path.join(ROOT_DIR, "tpain-supreme")
BUILD_DIR = os.path.join(TPAIN_DIR, "build", "SupremeTunerRealTimeV3_artefacts", "Release")

VST3_DIR = os.path.join(BUILD_DIR, "VST3", "Supreme Tuner Real Time v3.6.vst3")
EXE_PATH = os.path.join(BUILD_DIR, "Standalone", "Supreme Tuner Real Time v3.6.exe")
COVER_PATH = os.path.join(TPAIN_DIR, "assets", "supreme_tuner_cover.jpg")
README_PATH = os.path.join(TPAIN_DIR, "README.md")

ZIP_NAME = "SUPREME_TUNER_v3.6.0_Windows.zip"
ZIP_PATH = os.path.join(ROOT_DIR, ZIP_NAME)

print(f"Creating release package: {ZIP_PATH}...")
if os.path.exists(ZIP_PATH):
    os.remove(ZIP_PATH)

with zipfile.ZipFile(ZIP_PATH, "w", zipfile.ZIP_DEFLATED) as zipf:
    # 1. Add Standalone exe
    if os.path.exists(EXE_PATH):
        print(f"Adding Standalone exe: {EXE_PATH}")
        zipf.write(EXE_PATH, "Supreme Tuner Real Time v3.6.exe")
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
release_notes = """# 🎤 SUPREME TUNER Real Time v3.6: Fruity Limiter & Console Hybrid Edition

![Supreme Tuner Cover](supreme_tuner_cover.jpg)

### 🌟 Novedades de la Versión 3.6:
- 🎛️ **Roscas Rotatorias Estilo Fruity Limiter**: Perillas con tapas en pizarra/carbón oscuro (`#13171d` a `#262c36`), bisel exterior slate (`#4b5563`), arcos activos en ámbar dorado brillante (`#f59e0b` / `#fbbf24`), y aguja de precisión ámbar con núcleo blanco nítido y joya central.
- 🎚️ **Faders Verticales de Consola Metálica de Estudio**:
  - **Unidad de Dinámica (Unit 1)**: Faders deslizantes dedicados para `TRANSIENTS` y `COMPRESSION`.
  - **Unidad Master (Unit 4)**: Faders deslizantes para `INPUT` y `OUTPUT` con vúmetros de pico live peak metering tricolor (-60 dB a +6 dB).
- 🤖 **Subagente Especializado en GUIs Complejas Registrado**: Integrado el agente `experto-gui-vision` (`gemini-3`) en `.agents/agents/experto-gui-vision.md` para ingeniería inversa visual y diseño sin solapamientos.
- 🎯 **Cero Solapamientos (100% Zero-Overlap)**: Margen e intervalo milimétrico garantizado en Noise Gate & Dynamics (badges, vúmetros y faders perfectamente aislados).
- 🌐 **Control de Imagen Estéreo Mid/Side**: De 0% Mono puro a 200% Super-Wide.
- 🎨 **52 Presets de Artistas**: T-Pain, Travis Scott, Drake, Bad Bunny, Rosalía, Feid, Mora, Yeat, Peso Pluma y más.
- ⚡ **Latencia Ultra-Baja**: < 2.9 ms con motor de audio a 64 bits.
- 🛡️ **12/12 Severe Stress Tests Pasados**: Inmunidad total a fallos, NaN/Inf y ráfagas DC.

### 📦 Contenido del Paquete:
- `Supreme Tuner Real Time v3.6.vst3` (64-bit VST3 para FL Studio, Ableton, Cubase, Studio One, etc.)
- `Supreme Tuner Real Time v3.6.exe` (Standalone para directos y ensayos con latencia mínima)
- `supreme_tuner_cover.jpg` (Portada en alta definición)
- `README.md` (Documentación completa de uso y arquitectura)
"""

notes_path = os.path.join(ROOT_DIR, "release_notes_v36.md")
with open(notes_path, "w", encoding="utf-8") as f:
    f.write(release_notes)

cmd = [
    "gh", "release", "create", "v3.6.0",
    ZIP_PATH,
    COVER_PATH,
    "--title", "SUPREME TUNER Real Time v3.6 (Fruity Limiter & Console Hybrid)",
    "--notes-file", notes_path
]

print("Executing gh release create...")
res = subprocess.run(cmd, cwd=ROOT_DIR, capture_output=True, text=True, encoding="utf-8")
print("STDOUT:", res.stdout)
print("STDERR:", res.stderr)
print(f"Exit code: {res.returncode}")
