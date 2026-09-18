# Manual de Usuario: AUTOMASTER SUPREME VST AI Suite

Bienvenido a **AUTOMASTER SUPREME**, la suite de masterización profesional asistida por inteligencia artificial multi-agente diseñada para productores musicales, ingenieros de mezcla y creadores de contenido sonoro.

---

## 🚀 1. Cómo Iniciar la Aplicación

Para abrir la suite de masterización en tu máquina local:

1. Abre la terminal en el directorio del proyecto:
   ```bash
   cd c:\Users\alfaswz\.gemini\antigravity-ide\scratch\agentes-personalizados
   ```
2. Inicia el servidor nativo:
   ```bash
   npm start
   # o bien: node server.mjs
   ```
3. Abre tu navegador favorito en:
   👉 **`http://localhost:4321`**

---

## 🎛️ 2. Guía Rápida de Masterización en 3 Pasos

### Paso 1: Cargar la Canción
Tienes tres formas de cargar audio:
- **Pista Demo Inmediata**: Haz clic en el botón **`🎵 Pista Demo`** para generar al instante una pista multitrack completa sintetizada en alta fidelidad (124 BPM Synthwave/Groove).
- **Arrastrar y Soltar**: Arrastra cualquier archivo `.wav` o `.mp3` directamente sobre el visor de onda (*waveform*).
- **Explorador de Archivos**: Haz clic en el visor de onda para seleccionar un archivo de tu disco.

### Paso 2: Masterización con Inteligencia Artificial
1. En la barra superior, selecciona el **Género Musical** de tu pista (por ejemplo, *EDM*, *Hip-Hop / 808*, *Modern Pop*, *Reggaeton*, etc.).
2. Haz clic en el botón brillante **`⚡ AUTO-MASTER AI`**.
3. Observa en la consola inferior cómo los **4 Agentes** analizan tu canción:
   - 🧠 **Orquestador**: Calcula el rango dinámico (*Crest Factor*) y la distancia al objetivo LUFS.
   - 📐 **Arquitecto Acústico**: Calibra el ecualizador para corregir lodo en 300Hz o asperezas en 4kHz.
   - 🛡️ **Revisor de Código**: Ajusta los compresores multibanda para evitar saturación indeseada.
   - 🧪 **Tester QA**: Maximiza el volumen cuidando el techo True-Peak.
4. Las perillas se moverán suavemente a las posiciones óptimas calculadas por la IA.

### Paso 3: Ajuste Fino y Comparación A/B
- **Audición A/B**: Alterna entre **`WET (MÁSTER)`** y **`DRY (ORIGINAL)`** para evaluar el impacto del procesamiento.
- **Level Match (Nivelación de Volumen)**: Con esta casilla marcada, el sistema iguala el volumen percibido del audio original al volumen del máster, permitiéndote comprobar si la masterización mejora el cuerpo, la claridad y el balance sin el engaño del "suena más fuerte, luego suena mejor".
- **Perillas Táctiles**:
  - Arrastra verticalmente con el ratón para ajustar.
  - Mantén presionada la tecla **Shift** para calibración ultra-fina (incrementos de 0.1).
  - Haz **Doble Clic** en cualquier perilla para restablecerla a su valor por defecto.

### Paso 4: Exportación del Máster
1. Haz clic en el botón verde **`💾 EXPORTAR MÁSTER`**.
2. Revisa las especificaciones (WAV Broadcast 24-bit PCM con dither TPDF).
3. Haz clic en **`INICIAR RENDER Y DESCARGA`**.
4. En pocos segundos se procesará toda la pista a velocidad turbo y se descargará automáticamente tu archivo `.wav` listo para plataformas de streaming (Spotify, Apple Music, YouTube) o discoteca.

---

## 🎚️ 3. Módulos y Parámetros Disponibles

| Módulo | Parámetro | Rango | Función en el Máster |
|---|---|---|---|
| **Input & Sub** | `IN GAIN` | -18 a +18 dB | Calibración del nivel de entrada a la cadena |
| | `LOW CUT` | 20 a 80 Hz | Elimina subgraves inaudibles que ensucian el limitador |
| **Parametric EQ** | `SUB BASS` | $\pm 12$ dB | Realce de pegada en bombos y sub-bajos |
| | `LOW-MID` | $\pm 9$ dB | Limpieza de frecuencias turbias (*mud*) en 250-400Hz |
| | `PRESENCE` | $\pm 9$ dB | Cuerpo y articulación de voces e instrumentos líderes |
| | `CLARITY` | $\pm 9$ dB | Definición de transientes y ataque de baterías |
| | `AIR SHEEN` | $\pm 12$ dB | Brillo sedoso superior (10-20 kHz) sin asperezas |
| **Tape Warmth** | `DRIVE` | 0 a 100% | Cantidad de distorsión armónica analógica |
| | `WARMTH` | 0 a 100% | Calidez y redondeo de picos digitales agresivos |
| | `MODO` | Tape/Tube/ClassA | Tipo de textura de saturación analógica |
| **Multiband Comp** | `LOW THRESH` | -35 a -5 dB | Control dinámico exclusivo para el grave (<160 Hz) |
| | `MID THRESH` | -35 a -5 dB | Control de dinámica de rango medio (160 Hz - 4.2 kHz) |
| | `HIGH THRESH`| -35 a -5 dB | Control de brillo y siseos dinámicos (>4.2 kHz) |
| **Stereo & Limiter**| `WIDTH` | 50 a 200% | Ensanchamiento del plano estéreo (100% = natural) |
| | `LOUDNESS` | 0 a 14 dB | Empuje de sonoridad hacia el techo comercial |
| | `CEILING` | -1.5 a 0 dBTP | Margen True-Peak anti-distorsión para streaming |
| | `OUT GAIN` | -12 a +12 dB | Ganancia final de salida |

---

## 👥 4. Colaboración de los Agentes Personalizados

En la parte inferior de la interfaz encontrarás la consola en vivo con los 4 agentes del sistema:
- 🧠 **Agente Orquestador (`agentes-personalizados.md`)**
- 📐 **Agente Arquitecto Acústico (`arquitecto-documentador.md`)**
- 🛡️ **Agente Revisor de Código y Seguridad (`revisor-codigo.md`)**
- 🧪 **Agente Generador de Pruebas y QA (`generador-pruebas.md`)**

Cada acción que ejecutes se refleja con telemetría en tiempo real para brindarte máxima transparencia sobre el procesamiento de tu música.
