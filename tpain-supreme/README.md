# 🎤 SUPREME TUNER Real Time v3.6: Ultra-Low Latency Vocal Quantization Engine

<p align="center">
  <img src="assets/supreme_tuner_cover.jpg" alt="Supreme Tuner Real Time v3.6 Cover" width="100%">
</p>

<p align="center">
  <a href="https://github.com/vevikils/AUTOMASTER-SUPREME"><img src="https://img.shields.io/badge/Release-v3.6.0-purple.svg?style=for-the-badge&logo=github" alt="Release v3.6.0"></a>
  <a href="https://github.com/vevikils"><img src="https://img.shields.io/badge/Author-vevi-gold.svg?style=for-the-badge" alt="Author vevi"></a>
  <img src="https://img.shields.io/badge/Format-VST3%20%7C%20Standalone%2064--bit-blueviolet.svg?style=for-the-badge" alt="Formats">
  <img src="https://img.shields.io/badge/Latency-%3C%202.9%20ms%20%7C%20Live%20Engine-brightgreen.svg?style=for-the-badge" alt="Latency">
  <img src="https://img.shields.io/badge/UI-Fruity%20Limiter%20Amber%20Knobs%20%2B%20Console%20Faders-f59e0b.svg?style=for-the-badge" alt="UI Style">
  <img src="https://img.shields.io/badge/Stress%20Tests-12%2F12%20PASSED-success.svg?style=for-the-badge" alt="Stress Tests">
</p>

---

## 🌟 Descripción General

**SUPREME TUNER Real Time v3.6** es un plugin VST3 y aplicación Standalone de corrección de tono y cuantización vocal en tiempo real de ultra-baja latencia (< 2.9 ms), diseñado tanto para directos sobre el escenario como para producción intensiva en estudio (Trap, Drill, Reggaetón, Hyperpop, R&B y Pop moderno).

Esta versión v3.6 incorpora el rediseño gráfico exacto supervisado por el subagente de IA especializada **`experto-gui-vision`** (basado en el modelo de visión avanzada `gemini-3`):
- **Roscas exactas de Fruity Limiter**: Perillas rotatorias con casquetes de pizarra/carbón oscuro (`#13171d` a `#262c36`), bisel exterior slate (`#4b5563`), arcos activos en ámbar dorado brillante (`#f59e0b` / `#fbbf24`), y aguja de precisión con núcleo blanco y joya central.
- **Faders Verticales de Consola Metálica**: Deslizadores analógicos con tapas de fader de aluminio cepillado, estrías táctiles y muesca central iluminada en oro/blanco.
- **Arquitectura Híbrida Balanceada**: Faders verticales para dinámica (`TRANSIENTS`, `COMPRESSION`) y para ganancia master (`INPUT`, `OUTPUT`), alternados armoniosamente con perillas circulares.
- **Cero Solapamientos (100% Zero-Overlap)**: Aislamiento milimétrico entre badges de estado, vúmetros y faders.

---

## 🎛️ Principales Características de la v3.6

### 1. 🎛️ Roscas Rotatorias Estilo Fruity Limiter
Inspiradas en los codiciados potenciómetros de dinámica de FL Studio Fruity Limiter:
- **Tapa Dark Slate / Carbón de Estudio**: Acabado mate no reflectante con cuerpo texturizado en grafito profundo.
- **Bisel Exterior Slate & Sombras de Profundidad**: Relieve tridimensional moldeado con iluminación cenital.
- **Arco Activo Ámbar Dorado Radiante**: Barra de valor curva en gradiente ámbar/dorado (`#f59e0b` a `#fbbf24`) de máxima visibilidad.
- **Aguja Notch con Núcleo Blanco y Joya Central**: Puntero ámbar con núcleo blanco nítido y joya luminosa en el eje.

### 2. 🎚️ Faders Verticales de Consola de Estudio
- **Controles de Dinámica (Unit 1)**: `TRANSIENTS` y `COMPRESSION` en deslizadores verticales de alta resolución para un control táctil milimétrico del pegada y compresión vocal.
- **Controles de Ganancia Master (Unit 4)**: `INPUT` y `OUTPUT` faders verticales dedicados con vúmetros de pico live peak LED de -60 dB a +6 dB integrados al costado de cada fader.

### 3. 🎯 Cero Solapamientos y Geometría Milimétrica
- **NOISE GATE & DYNAMICS**:
  - **Fila 1**: 3 Perillas rotatorias Fruity Limiter (`UMBRAL`, `ATAQUE`, `LIBERACION`).
  - **Fila 2**: Badge rectangular con LED de estado (`[GATE: OPEN]` / `[GATE: ATTENUATING -XX dB]`) completamente aislado con 10px de margen libre.
  - **Fila 3**: 2 Faders verticales de consola para dinámica (`TRANSIENTS` y `COMPRESSION`).
  - **Fila 4**: Vúmetro LED horizontal de reducción de ganancia (`COMP GAIN REDUCTION` y `--XX.X dB`).
- **GAIN STAGING & MASTER**:
  - **Izquierda**: Fader vertical de `INPUT` + Vúmetro Peak live LED tricolor (`IN`).
  - **Centro**: 2 Perillas rotatorias Fruity Limiter (`STEREO WIDTH` y `DRY / WET`).
  - **Derecha**: Fader vertical de `OUTPUT` + Vúmetro Peak live LED tricolor (`OUT`).
  - **Base**: Insignia de saturación analógica de cinta (`[ANALOG SATURATION: ACTIVE]`).

### 4. 🌐 Control de Imagen Estéreo Mid/Side (`STEREO WIDTH`)
- **`0 %` (Mono)**: Colapsa la señal a mono puro, garantizando compatibilidad total en altavoces de móvil y sistemas de club.
- **`100 %` (Normal)**: Estéreo natural y balanceado.
- **`200 %` (Super-Wide)**: Abre los canales laterales (*Side*), creando un campo estéreo envolvente para coros, doblajes y ad-libs.

### 5. 🎚️ Gain Staging y Vúmetros Peak en Vivo
- Monitoreo continuo de entrada y salida de **-60 dB a +6 dB**.
- Indicadores LED con escala de 3 segmentos (Verde para rango nominal, Ámbar para presencia, Rojo para advertencia de recorte).

---

## 🎨 Banco de 52 Presets de Artistas

| Categoría | Presets Incluidos |
|---|---|
| **Leyendas del Autotune** | *T-Pain - Buy U a Drank (Hard Snap)*, *Travis Scott - Sicko Mode*, *Drake - God's Plan*, *Bad Bunny - Tití Me Preguntó*, *Rosalía - Motomami Snappy*, *The Weeknd - Blinding Lights 80s*, *Kanye - Heartless (808s)*, *Post Malone - Circles*, *Billie Eilish - Ocean Eyes*, *Juice WRLD - Lucid Dreams* |
| **Urbano Latino & Trap** | *Rauw Alejandro - Todo De Ti*, *Feid - Feliz Cumpleaños Ferxxo*, *Mora - Memorias*, *Anuel AA - Real Hasta La Muerte*, *Karol G - Provenza*, *Bizarrap - Music Sessions*, *Duki - Goteo Trap*, *Trueno - Dance Crip*, *Eladio Carrión - Mbappé*, *Young Miko - Lisa Snappy* |
| **Rage & Hyperpop** | *Lil Uzi Vert - Just Wanna Rock*, *Playboi Carti - Magnolia*, *Yeat - Monëy so big*, *Peso Pluma - Ella Baila Sola*, *Natanael Cano - Pacas De Billetes*, *Fuerza Regida - Bebe Dame*, *Charli XCX - Von Dutch*, *glaive - astrid Digicore*, *Sophie - Immaterial Glitch* |
| **R&B, Soul & Pop** | *SZA - Kill Bill*, *Ariana Grande - 7 Rings*, *Frank Ocean - Nights*, *Tyler The Creator - Earfquake*, *Giveon - Heartbreak Anniversary*, *Kali Uchis - Telepatía*, *Justin Bieber - Peaches* |
| **Estudio FX & Filtros** | *Vintage Telephone (AM Bandpass)*, *Megaphone Lo-Fi Distortion*, *Underwater Sub Voicing*, *Radio FM Broadcast Drive*, *Space Astronaut Comm Link*, *Daft Punk Robotic Vocoder*, *Cybernetic Android AI*, *Ambient Cloud Reverb*, *Slapback Echo 50s*, *Doubler Stereo Widener*, *Octave Sub Heavy Pitch*, *Whisper Plate Vocals* |

---

## 💻 Instalación Rápida

### VST3 (DAWs: FL Studio, Ableton, Reaper, Cubase, Studio One)
Copia la carpeta `Supreme Tuner Real Time v3.6.vst3` en tu directorio estándar VST3 de Windows:
```
C:\Program Files\Common Files\VST3\Supreme Tuner Real Time v3.6.vst3
```

### Standalone (Ejecutable de Escritorio)
Ejecuta directamente:
```
Supreme Tuner Real Time v3.6.exe
```
Ideal para ensayos, directos con tarjeta de sonido USB/Thunderbolt y pruebas con latencia mínima sin necesidad de abrir un DAW.

---

## 🛠️ Compilación desde el Código Fuente

### Requisitos:
- **Visual Studio 2022** (MSVC v143 con soporte C++20).
- **CMake 3.22+**.
- **Windows 10 / 11 (64-bit)**.

```bash
# 1. Clonar repositorio
git clone https://github.com/vevikils/AUTOMASTER-SUPREME.git
cd AUTOMASTER-SUPREME/tpain-supreme

# 2. Generar solución con CMake
cmake -B build -G "Visual Studio 17 2022" -A x64

# 3. Compilar en modo Release
cmake --build build --config Release --parallel

# 4. O compilar y desplegar automáticamente con el script deploy.py
python deploy.py
```

### Suite de Tests de Estrés (12/12):
```bash
.\build\Release\StressTest.exe
```
Valida inmunidad contra NaN/Inf, sample rates de 8 kHz a 384 kHz, buffers variables, desconexiones en caliente de micrófonos, fuzzing a 5,000 bloques y ráfagas DC de +100 dBFS.

---

## 📜 Licencia
Distribuido bajo la licencia MIT. Creado con ❤️ por **vevi**.
