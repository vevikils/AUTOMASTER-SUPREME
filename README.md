# 🎛️ AUTOMASTER SUPREME 3.3: AI Mastering Suite by vevi (v3.3)

[![Release](https://img.shields.io/badge/Release-v3.3.0-cyan.svg?style=for-the-badge)](https://github.com/vevikils/AUTOMASTER-SUPREME/releases)
[![Author](https://img.shields.io/badge/Author-vevi-gold.svg?style=for-the-badge)](https://github.com/vevikils)
[![Format](https://img.shields.io/badge/Format-VST3%20%7C%20Standalone%20%7C%20WebAudio-orange.svg?style=for-the-badge)](https://github.com/vevikils/AUTOMASTER-SUPREME)
[![Meters](https://img.shields.io/badge/Meters-60%20FPS%20True--Peak-red.svg?style=for-the-badge)](https://github.com/vevikils/AUTOMASTER-SUPREME)
[![Theme](https://img.shields.io/badge/Theme-Dark%20%26%20Light%20Modes-blue.svg?style=for-the-badge)](https://github.com/vevikils/AUTOMASTER-SUPREME)
[![DSP](https://img.shields.io/badge/DSP-JUCE%208%20%7C%20C%2B%2B17%20%7C%20Zero--Alloc%20Caching-purple.svg?style=for-the-badge)](https://github.com/vevikils/AUTOMASTER-SUPREME)
[![Target](https://img.shields.io/badge/Spotify-14%20LUFS%20%7C%20--1.0%20dBTP-green.svg?style=for-the-badge)](https://github.com/vevikils/AUTOMASTER-SUPREME)
[![License](https://img.shields.io/badge/License-MIT-brightgreen.svg?style=for-the-badge)](LICENSE)

**AUTOMASTER SUPREME 3.3** (creado por **vevi**) es una estación de trabajo y plugin de masterización con inteligencia artificial diseñada para procesar mezclas musicales estéreo con precisión quirúrgica, dinámica analógica cálida y volumen comercial competitivo, calibrada estrictamente para los estándares de streaming modernos (**Spotify: -14.0 LUFS / -1.0 dBTP**).

### 🚀 Novedades de la Versión 3.3 (60 FPS Peak Meters & FFT Spectrogram Fix):
- **Barras de True Peak a 60 FPS**: Los vúmetros verticales estéreo de True-Peak (L / R) ahora se actualizan de forma continua a **60 FPS** con ataque instantáneo y balística de caída suave de grado estudio profesional.
- **Lecturas Digitales Estables (0.5s)**: Las métricas numéricas (Momentary LUFS, Short-Term LUFS, Crest Factor y estado Spotify) se actualizan de forma estable cada 0.5 segundos para una lectura descansada y sin saltos rápidos.
- **Corrección Total del Analizador de Espectro FFT (Spectrogram)**: Corregido el mapeo de magnitudes decibélicas del FFT, haciendo que la curva de frecuencias cyan reactiva cobre vida en tiempo real bajo la curva de ecualización analítica (estilo FabFilter Pro-Q).
- **Decaimiento Suave de Espectro**: Añadido algoritmo de release progresivo hacia el suelo de ruido cuando se pausa o detiene la música en el DAW.
- **Cabecera Optimizada sin Puntos Suspensivos**: Bounding box ampliado a 285 px para `AUTOMASTER SUPREME 3.3`, evitando truncamiento tipográfico en FL Studio.
- **Modo Oscuro y Claro (Dark & Light Mode)**: Conmutador instantáneo en cabecera con chasis adaptativo.

---

## 🚀 Descarga Rápida (Binarios Precompilados para Windows)

¿Quieres usarlo de inmediato en tu DAW o en tu escritorio?
Descarga el paquete oficial listo para usar en un clic:

👉 **[Descargar AUTOMASTER_SUPREME_v3.3.0_Windows.zip](https://github.com/vevikils/AUTOMASTER-SUPREME/releases/latest)**

El paquete incluye:
- `AUTOMASTER SUPREME 3.3.vst3` (Plugin nativo de 64 bits para FL Studio y DAWs).
- `AUTOMASTER SUPREME 3.3.exe` (Ejecutable de escritorio Standalone).
- Guías paso a paso de instalación y configuración.

---

## 🎯 Calibración Oficial para Spotify (-14 LUFS / -1.0 dBTP)

La "guerra del volumen" (*Loudness War*) tradicional destruye las canciones en las plataformas de streaming modernas:
- Si masterizas a **-8 o -7 LUFS**, el motor de normalización de Spotify **atenuará automáticamente entre -6 y -7 dB tu mezcla**. Tu canción sonará al mismo nivel que las demás, pero **habrá perdido el impacto de la batería, el punch del bombo y conservará la fatiga auditiva del limitador**.
- **AUTOMASTER SUPREME** está optimizado con la curva de referencia oficial de Spotify: **-14.0 LUFS Integrados** y **-1.0 dBTP Ceiling** (para prevenir distorsión inter-sample en la conversión a Ogg Vorbis / AAC).
- El resultado: mezclas con **máxima dinámica, bajos definidos, transientes explosivos y presencia vocal nítida**, sonando imponentes y sin penalización algorítmica.

---

## 🎛️ Cadena de Señal DSP (Signal Flow Architecture)

```mermaid
graph TD
    subgraph "Entrada de Audio"
        In[Señal Estéreo WAV / MP3 / Direct DAW Bus] --> InGain[Input Gain Stage ±18dB]
        InGain --> LowCut[Sub-Tamer HighPass 20-80Hz]
    end

    subgraph "Ecualización Tonal (5 Bandas)"
        LowCut --> EQ1[Low Shelf: 30-150Hz ±12dB]
        EQ1 --> EQ2[Low-Mid Bell: 150-800Hz Mud Control]
        EQ2 --> EQ3[Mid Bell: 800-3500Hz Presence]
        EQ3 --> EQ4[High-Mid Bell: 3.5k-8kHz Clarity]
        EQ4 --> EQ5[High Shelf: 8k-20kHz Air Sheen]
    end

    subgraph "Color y Armónicos"
        EQ5 --> Sat[WaveShaper 4x Oversampled]
        Sat -.-> SatModes["Modos: Analog Tape / Tube Triode / Class-A / Clean"]
    end

    subgraph "Dinámica Multibanda (Linkwitz-Riley 3-Way)"
        Sat --> Split[Crossover LR4 160Hz & 4.2kHz]
        Split -->|Bajos <160Hz| CompL[Compresor de Graves]
        Split -->|Medios 160Hz-4.2k| CompM[Compresor de Medios]
        Split -->|Agudos >4.2kHz| CompH[Compresor de Agudos]
        CompL --> Sum[Sumador Multibanda]
        CompM --> Sum
        CompH --> Sum
    end

    subgraph "Espacialidad y Maximización"
        Sum --> MidSide[Procesador Mid/Side & Mono-Maker <110Hz]
        MidSide --> Loudness[Loudness Drive Maximizer]
        Loudness --> Limiter[True-Peak Brickwall Lookahead Limiter]
        Limiter --> Ceiling[Ceiling Normalizer -1.0 dBTP Spotify]
    end

    subgraph "Salida y Telemetría 60 FPS"
        Ceiling --> MasterOut[Master Output Stage]
        MasterOut --> FFT[Analizador FFT Dual + Goniometro Lissajous]
        MasterOut --> Meters[Medición LUFS / True-Peak ITU-R BS.1770]
    end
```

---

## 🎚️ Módulos de Procesamiento y Controles

| Módulo | Parámetro | Rango | Función en el Máster |
|---|---|---|---|
| **Input & Sub** | `IN GAIN` | -18 a +18 dB | Calibración precisa del nivel de entrada a la cadena de audio |
| | `LOW CUT` | 20 a 80 Hz | Elimina frecuencias subsónicas inaudibles que saturan el limitador |
| **Parametric EQ** | `SUB BASS` | $\pm 12$ dB | Realce de peso en bombos y sub-bajos a 35 Hz |
| | `LOW-MID` | $\pm 9$ dB | Limpieza de frecuencias turbias (*mud*) en 300 Hz |
| | `PRESENCE` | $\pm 9$ dB | Definición y articulación vocal en 1.5 kHz |
| | `CLARITY` | $\pm 9$ dB | Ataque de transientes y percusiones en 4.5 kHz |
| | `AIR SHEEN` | $\pm 12$ dB | Brillo sedoso analógico en 12-20 kHz sin asperezas |
| **Tape Warmth** | `DRIVE` | 0 a 100% | Generación de armónicos pares e impares con sobremuestreo 4x |
| | `WARMTH` | 0 a 100% | Suavizado analógico de picos transientes digitales |
| | `MODO` | Tape / Tube / Class-A | Textura de saturación: cinta analógica, válvula o clase A |
| **Multiband Comp** | `LOW THRESH` | -35 a -5 dB | Control de dinámica dedicado para graves (<160 Hz) |
| | `MID THRESH` | -35 a -5 dB | Pegamento dinámico en frecuencias medias (160 Hz - 4.2 kHz) |
| | `HIGH THRESH`| -35 a -5 dB | Control de siseo y dinámica en frecuencias altas (>4.2 kHz) |
| **Stereo & Limiter**| `WIDTH` | 50 a 200% | Ensanchamiento estéreo con protección mono en graves (<110 Hz) |
| | `LOUDNESS` | 0 a 10 dB | Empuje de sonoridad hacia el objetivo comercial |
| | `CEILING` | -1.5 a 0 dBTP | Techo True-Peak calibrado a -1.0 dBTP para Spotify |
| | `OUT GAIN` | -12 a +12 dB | Nivel de salida máster final |

---

## 🔌 Instalación en FL Studio

1. Copia la carpeta **`AUTOMASTER SUPREME.vst3`** a la ruta estándar de VST3 en Windows:
   ```text
   C:\Program Files\Common Files\VST3\
   ```
2. Abre **FL Studio**.
3. Ve a **Options > Manage plugins**.
4. Haz clic en **Find installed plugins**.
5. Cuando aparezca **AUTOMASTER SUPREME**, márcalo como favorito con la estrella ⭐.
6. Ábrelo en el canal **Master** del Mixer (**F9**).

---

## 🤖 Arquitectura Multi-Agente (Google Antigravity Custom Agents)

El desarrollo de esta suite fue concebido y coordinado mediante el estándar de **Custom Agents** de Google Antigravity ubicado en `.agents/agents/`:

```text
.agents/agents/
├── agentes-personalizados.md       # 🧠 Orquestador principal y director de IA
├── arquitecto-documentador.md      # 📐 Arquitecto DSP, diagramas Mermaid y documentación técnica
├── revisor-codigo.md               # 🛡️ Auditor de estabilidad numérica, anti-clipping y seguridad
└── generador-pruebas.md            # 🧪 Generador de pruebas automatizadas y QA (14/14 tests aprobados)
```

Para verificar las pruebas matemáticas automatizadas en cualquier momento:
```bash
node test_dsp.mjs
```

---

## 🛠️ Compilación Local desde Código Fuente

### Requisitos:
- **CMake 3.22+**
- **Visual Studio 2022** con soporte C++ (MSVC v143+)
- **JUCE 8**

### Pasos de compilación:
```bash
cd vst3-plugin
cmake -B build -S .
cmake --build build --config Release
```

Los binarios generados estarán en:
- `vst3-plugin/build/AutomasterSupreme_artefacts/Release/VST3/AUTOMASTER SUPREME.vst3`
- `vst3-plugin/build/AutomasterSupreme_artefacts/Release/Standalone/AUTOMASTER SUPREME.exe`

---

## 📄 Licencia

Este proyecto está bajo la Licencia **MIT** - consulta el archivo [LICENSE](LICENSE) para más detalles.

Desarrollado con pasión por [Vevikils](https://github.com/vevikils).
