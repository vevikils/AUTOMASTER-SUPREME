# Guía de Instalación y Uso en FL Studio: AUTOMASTER SUPREME 3.3 VST3 (v3.3) · Por vevi

Esta guía detalla cómo utilizar **AUTOMASTER SUPREME 3.3** (creado por **vevi**) como plugin VST3 nativo en **FL Studio**, cómo alternar entre **Modo Oscuro y Claro (Dark/Light)**, cómo usar el **Espectrograma en Tiempo Real**, los vúmetros a **60 FPS** con controles numéricos a **0.5 segundos** y cómo aprovechar la calibración para los estándares oficiales de **Spotify (-14 LUFS / -1.0 dBTP)**.

---

## 🎯 ¿Por qué usar los Volúmenes de Referencia de Spotify (-14 LUFS)?

Las plataformas de streaming modernas (Spotify, Apple Music, YouTube Music, Tidal, Amazon Music) aplican **Normalización de Volumen (Loudness Normalization)** automática:

1. **La Referencia de Spotify**:
   - Spotify normaliza por defecto a **-14.0 LUFS Integrados** (usando la norma ITU-R BS.1770 / EBU R128).
   - El techo máximo recomendado es **-1.0 dBTP (True Peak)** (o -2.0 dBTP para pistas muy cargadas).
2. **El Problema del "Volumen Muy Muy Alto" (Loudness War)**:
   - Si masterizas una canción a -8 o -7 LUFS (volumen aplastado de club antiguo), Spotify aplicará una **atenuación automática de -6 a -7 dB**.
   - El resultado es devastador: Tu canción no sonará más fuerte que las demás, pero **habrá perdido todos sus transientes, la pegada del bombo, la dinámica de la voz y conservará la distorsión del limitador**.
3. **La Solución de AUTOMASTER SUPREME 3.3 con Telemetría 60 FPS y Espectrograma Reactivo**:
   - Creado por **vevi** con diseño ampliado (+20%, 1296x888 px), perillas estilo FabFilter y vúmetros Waves con respuesta a 60 FPS.
   - **Vúmetros True-Peak a 60 FPS**: Barras verticales ultra-rápidas y reactivas con caída suave de estudio.
   - **Métricas Numéricas a 0.5s**: Valores digitales de LUFS y Crest Factor que se actualizan de forma descansada sin saltos bruscos.
   - **Espectrograma FFT Dinámico**: Representación gráfica fiel de las frecuencias de tu música bajo la curva de ecualización.
   - **Modo Oscuro / Claro**: Cambia el tema con el botón `THEME: DARK` / `THEME: LIGHT` para adaptarse a cualquier entorno de estudio o iluminación.
   - **Zero-Allocation DSP**: Motor optimizado que no genera asignaciones dinámicas por bloque en el hilo de audio en tiempo real, garantizando rendimiento fluido y sin picos de CPU en FL Studio.
   - **Leyenda Interactiva en Tiempo Real**: Simplemente pasa el cursor sobre cualquier perilla para ver en la consola inferior su rango, qué hace y el valor óptimo recomendado para Spotify.
   - Botón **`CONTROL GUIDE`** para consultar la tabla resumen de todos los módulos en cualquier instante.

---

## 🔌 Cómo Instalar el Plugin en FL Studio

1. **Ubicación Estándar de VST3 en Windows**:
   FL Studio escanea de forma nativa la carpeta:
   `C:\Program Files\Common Files\VST3\`
   (El plugin `AUTOMASTER SUPREME 3.3.vst3` ya ha sido instalado en esa ubicación).

2. **Escanear el Plugin en FL Studio**:
   - Abre **FL Studio**.
   - En la barra superior, haz clic en **Options > Manage plugins** (Opciones > Administrar plugins).
   - Asegúrate de que `C:\Program Files\Common Files\VST3` esté en la lista de rutas de búsqueda (*Plugin search paths*).
   - Haz clic en el botón amarillo **Find installed plugins** (Buscar plugins instalados).
   - Cuando termine el escaneo, busca **AUTOMASTER SUPREME 3.3** en la lista de efectos (*Effects*).
   - Marca la **estrella ⭐** a su izquierda para añadirlo a tus plugins favoritos.

3. **Insertar en el Canal Master**:
   - Abre la mesa de mezclas de FL Studio (**F9** o View > Mixer).
   - Selecciona el canal **Master** (el primer canal a la izquierda).
   - En cualquiera de los 10 slots de inserción (Slot 1 a 10), haz clic y selecciona **AUTOMASTER SUPREME 3.3**.

---

## 🎛️ Cómo Usarlo en tu Proyecto de FL Studio

1. **Seleccionar Preset de Género**:
   - En la esquina superior derecha del plugin, abre el desplegable de **Presets**.
   - Selecciona **`Spotify Standard Reference (-14 LUFS / -1.0 dBTP)`** o el género correspondiente a tu pista (*Modern Pop*, *Hip-Hop / Trap*, *EDM / Dance*, *Acoustic*, etc.).
2. **Presionar `⚡ AUTO-MASTER AI`**:
   - El motor analizará la señal de audio que pasa por el máster, calibrará los compresores y la ecualización, y fijará la sonoridad en el rango óptimo de Spotify.
3. **Automatización en FL Studio**:
   - Todos los parámetros del plugin están registrados en el árbol VST3.
   - Puedes hacer clic derecho en cualquier perilla o ir a **Tools > Last Tweaked > Create automation clip** en FL Studio para automatizar la ganancia, el ancho estéreo o la saturación a lo largo de tu canción.
