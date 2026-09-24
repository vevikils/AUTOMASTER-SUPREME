# 🎛️ AUTOMASTER SUPREME SUITE: v1.0.0-beta.1 (Public Beta)
### 👑 Creado por **vevi** (@vevikils)

[![Release](https://img.shields.io/badge/Release-v1.0.0--beta.1%20Public%20Beta-cyan.svg?style=for-the-badge)](https://github.com/vevikils/AUTOMASTER-SUPREME/releases)
[![Author](https://img.shields.io/badge/Author-vevi-gold.svg?style=for-the-badge)](https://github.com/vevikils)
[![Format](https://img.shields.io/badge/Format-VST3%20%7C%20Standalone%2064--bit-orange.svg?style=for-the-badge)](https://github.com/vevikils/AUTOMASTER-SUPREME)
[![DSP](https://img.shields.io/badge/DSP-JUCE%208%20%7C%20C%2B%2B20%20%7C%20Ultra--Low%20Latency-purple.svg?style=for-the-badge)](https://github.com/vevikils/AUTOMASTER-SUPREME)
[![Target](https://img.shields.io/badge/Spotify-14%20LUFS%20%7C%20--1.0%20dBTP-green.svg?style=for-the-badge)](https://github.com/vevikils/AUTOMASTER-SUPREME)
[![License](https://img.shields.io/badge/License-MIT-brightgreen.svg?style=for-the-badge)](LICENSE)

¡Bienvenidos a la **primera versión Beta Pública oficial** de **AUTOMASTER SUPREME SUITE**!
Una suite completa de producción, afinación vocal en tiempo real, espacialidad y masterización profesional asistida por IA para Windows (64-bit VST3 y Standalone).

---

## 📦 ¿Qué incluye la Suite Oficial Beta v1.0.0?

La suite integra 4 plugins de nivel profesional listos para FL Studio, Ableton Live, Cubase, Studio One, Reaper y modo Standalone:

### 1. 🎤 **Supreme Tune Real Time v4.2**
- **Afinación Vocal Ultra-Rápida**: Latencia inferior a 2.9 ms, optimizada para directo y grabación sin retardo perceptible.
- **Interfaz de Rack Analógico Híbrido 19"**: Chasis de titanio y aluminio anodizado, diales circulares en violeta neón y controles retroiluminados.
- **Búmetros Analógicos Dobles**: Con balística VU de respuesta clásica (300 ms) y display digital interior de nota en tiempo real.
- **Fader Master en Oro 24K**: Recorrido calibrado de -24 dB a +6 dB.
- **Racks Integrados**: Noise Gate, Compresor con control de transientes, Modelado de Tono Vocal (Air 12kHz, Body 250Hz, Warmth/Drive), Stereo Width y Reverb/Echo.
- **52 Presets de Artistas**: Configuraciones instantáneas para estilos como Travis Scott, T-Pain, Drake, Bad Bunny, Rosalia, Feid, Mora, etc.

### 2. 🎛️ **AUTOMASTER SUPREME 3.4**
- **Masterización Asistida por IA**: Calibración estricta a los estándares de streaming (**Spotify: -14.0 LUFS / -1.0 dBTP**).
- **Espectro Dual Voxengo SPAN-Style**:
  - Trazo **AVG (RMS)** en cian eléctrico y curva **MAX (Peak Hold)** en oro/ámbar con retención de picos.
  - Inspector de cursor con retícula dinámica y conversión en tiempo real de frecuencia a **Nota Musical y cents** (ej. `D#6 -14c`).
- **Ecualizador Quirúrgico 5 Bandas**: Low Shelf, Low-Mid Bell, Mid Presence, High-Mid Clarity, High Shelf.
- **Saturador WaveShaper 4x Oversampled** y limitador analógico anti-clipping inter-sample.
- **Medidores True-Peak a 60 FPS** con balística relajada.

### 3. 🌌 **REVERB SUPREME PRO**
- **Motor Algorítmico de Espacio Tridimensional**: Salas, cámaras, placas y reverberaciones infinitas cristalinas sin acumulación de frecuencias graves ni resonancias metálicas.
- **Filtros Damping y Pre-Delay Sincronizable**: Modela colas suaves y naturales para voces e instrumentos.

### 4. ⏱️ **Supreme Tuner BPM V.2.4**
- **Sincronización Métrica y Detección**: Afinación y timing coordinado con el tempo del proyecto en milisegundos y subdivisiones rítmicas de compás.

---

## 🚀 Descarga Rápida (Binarios Precompilados para Windows)

Descarga el paquete oficial completo listo para usar en un clic:

👉 **[Descargar AUTOMASTER_SUPREME_SUITE_v1.0.0_BETA_Windows.zip](https://github.com/vevikils/AUTOMASTER-SUPREME/releases/latest)**

### Estructura del paquete descargable:
```text
AUTOMASTER_SUPREME_SUITE_v1.0.0_BETA_Windows.zip
├── VST3/
│   ├── Supreme Tune Real Time v4.2.vst3
│   ├── AUTOMASTER SUPREME.vst3
│   ├── REVERB SUPREME PRO.vst3
│   └── Supreme Tuner BPM V.2.4.vst3
└── Standalone/
    ├── Supreme Tune Real Time v4.2.exe
    ├── AUTOMASTER SUPREME.exe
    ├── REVERB SUPREME PRO.exe
    └── Supreme Tuner BPM V.2.4.exe
```

---

## 💻 Instalación en FL Studio y otros DAWs

1. Descarga y extrae el archivo zip.
2. Copia el contenido de la carpeta `VST3` en tu directorio estándar de plugins de Windows:
   ```text
   C:\Program Files\Common Files\VST3\
   ```
3. Abre FL Studio (o tu DAW favorito) y realiza un escaneo rápido en el **Plugin Manager** (*Options -> Manage plugins -> Find installed plugins*).
4. ¡Listo! Los plugins aparecerán inmediatamente en tu lista de efectos.

---

## 📚 Recursos y Kits Incluidos en el Repositorio

- **`SUPREME-PRODUCER-DRUM-KIT/`**: Más de 400 elementos profesionales (One-shots afinados en C, 32 loops de melodías complejas con metadatos ACID para auto-tempo, 62 loops de batería por géneros: Trap, Drill, Reggaeton, BoomBap, Afro).
- **`SUPREME-VOCAL-CHAINS-FL-STUDIO/`**: 52 cadenas de mezcla vocal profesionales para el mezclador de FL Studio.
- **`companion-desktop-studio/`**: Asistente de producción con interfaz de escritorio.
- **`wan-desktop-studio/`**: Estudio de generación de contenido visual para productores.

---

## 🛠️ Compilación desde Código Fuente

### Requisitos:
- **Windows 10/11 (64-bit)**
- **CMake 3.22+**
- **Visual Studio 2022** con soporte C++ (MSVC v143+)
- **JUCE 8**

---

## 📄 Licencia

Este proyecto está bajo la Licencia **MIT** - consulta el archivo [LICENSE](LICENSE) para más detalles.

Desarrollado con dedicación y pasión por [vevi](https://github.com/vevikils).
