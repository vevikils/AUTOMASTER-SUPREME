# 🎬 Wan 2.1 Video Studio — RTX 4060 Edition

Aplicación de escritorio de generación de vídeo impulsada por **Wan 2.1**, optimizada a medida para la arquitectura y componentes específicos de tu ordenador:
- **GPU**: NVIDIA GeForce RTX 4060 (8 GB VRAM, Ada Lovelace, Tensor Cores Gen 4 con aceleración nativa FP8 y TensorFloat-32).
- **CPU**: Intel Core i7-12700KF (12 núcleos / 20 hilos).
- **RAM del Sistema**: ~48 GB DDR (más de 32 GB libres).
- **Almacenamiento**: Disco **D:\** (638 GB libres) preconfigurado como almacén de modelos para proteger el disco de sistema C:\.

---

## ⚡ ¿Por qué esta versión es especial para tu RTX 4060 (8GB VRAM)?

Los modelos de vídeo Wan 2.1 son de última generación y normalmente requieren tarjetas gráficas de centro de datos (24GB a 80GB de VRAM) para funcionar en precisión completa sin ajustes. Para que funcionen en tu **RTX 4060 (8GB)** con **máxima velocidad y calidad**, esta suite implementa:

1. **CPU Offloading Dinámico Inteligente**:
   - Tu equipo cuenta con **48 GB de RAM** (32 GB libres). La aplicación aprovecha esta memoria para alojar el gigantesco codificador de texto **T5-XXL** (~9 GB) en la memoria RAM del sistema.
   - Solo los bloques de la red de difusión activos se trasladan a los 8 GB de la RTX 4060 durante el proceso de muestreo.
2. **VAE Tiling & VAE Slicing**:
   - La fase de decodificación de vídeo en el espacio latente suele generar picos que superan los 8GB de VRAM en resoluciones panorámicas. Con el *Tiling* y *Slicing* automáticos, el decodificador fragmenta los fotogramas en bloques procesados en GPU sin perder un solo píxel de resolución y **sin riesgo de `CUDA Out of Memory`**.
3. **Aceleración TensorFloat-32 (TF32) y cuDNN Benchmark**:
   - Activa el modo TF32 nativo de los núcleos Tensor de 4ª generación de Ada Lovelace, multiplicando el rendimiento de las operaciones matriciales de difusión temporal.
4. **Soporte Wan 2.1 Turbo Distilled (4 a 8 pasos)**:
   - Permite generar clips de vídeo completos en tan solo **20 a 45 segundos** en lugar de esperar 5 a 10 minutos por clip.
5. **Gestor de Modelos Hugging Face y Civitai Integrado**:
   - Descarga con 1 clic de los repositorios oficiales de Hugging Face (`Wan-AI/Wan2.1-T2V-1.3B-Diffusers`, etc.).
   - Descargador directo de Civitai para LoRAs y checkpoints mediante su ID o URL.
   - Almacenamiento por defecto en `D:\WanVideoStudio_Models` (aprovechando tus 638 GB libres).

---

## 🚀 Cómo Iniciar la Aplicación

Puedes iniciar la aplicación con un solo clic:

1. Ve a la carpeta `wan-desktop-studio`.
2. Haz doble clic en **`Iniciar_Wan_Studio.bat`**.
3. Se abrirá la ventana de escritorio nativa (Microsoft Edge WebView2) con la interfaz Cyber-Studio en modo oscuro, telemetría en vivo de tu RTX 4060 y controles completos de generación.

*(Si deseas ejecutarlo manualmente por terminal: `wan_env\Scripts\python.exe main_desktop.py`)*

---

## 🎛️ Modos y Características de la Interfaz

- **Text-to-Video (T2V)**: Genera vídeos a partir de descripciones textuales.
- **Image-to-Video (I2V)**: Sube cualquier imagen y anímala con movimiento cinemático realista.
- **Mejorador de Prompts con IA**: Añade automáticamente palabras clave cinematográficas, iluminación de estudio, lentes de 35mm y realismo en 1 clic.
- **Selector de Formato Rápido**:
  - 🟢 `16:9 Panorámico` (832 × 480) — *Recomendado para máxima velocidad en RTX 4060*.
  - 🟢 `9:16 Vertical` (480 × 832) — *Ideal para TikTok, Instagram Reels y YouTube Shorts*.
  - 🟢 `1:1 Cuadrado` (624 × 624) — *Feed de redes sociales*.
  - 🟡 `720p HD` (1280 × 720) — *Alta resolución*.
- **HUD de Telemetría en Vivo**:
  - Monitoriza el uso real de VRAM en tu RTX 4060, temperatura de la GPU, RAM del sistema y espacio en disco en tiempo real.
- **Galería Integrada**:
  - Reproducción instantánea de creaciones anteriores, descarga de archivos MP4 y reutilización de semillas y prompts.
