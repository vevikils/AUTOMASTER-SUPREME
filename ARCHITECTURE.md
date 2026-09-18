# Especificación de Arquitectura de Audio DSP: AUTOMASTER SUPREME

> **Autor**: Subagente `arquitecto-documentador.md`  
> **Sistema**: AUTOMASTER SUPREME VST AI Mastering Suite  
> **Motor**: Web Audio API 64-bit Floating-Point DSP / OfflineAudioContext 32-bit Float Render

---

## 1. Visión General del Sistema

**AUTOMASTER SUPREME** es una estación de trabajo y plugin de masterización con inteligencia artificial que procesa señales de audio estéreo ya mezcladas (WAV / MP3) para elevar su volumen comercial competitivo, ensanchar el campo estéreo con control de fase mono en graves, aplicar ecualización correctiva y tonal, introducir armónicos analógicos cálidos y maximizar el techo con limitación True-Peak transparente.

```mermaid
graph TD
    subgraph "Nivel de Entrada"
        Source[Archivo WAV o MP3 / Pista Demo Sintetizada] --> Decoder[AudioContext.decodeAudioData]
        Decoder --> AudioBuffer[(AudioBuffer 32-bit Float Estéreo)]
    end

    subgraph "Cadena de Señal en Tiempo Real (Real-Time DSP Graph)"
        AudioBuffer --> PreAnal[AnalyserNode: Pre-Master FFT 2048]
        AudioBuffer --> DryBranch[Rama Dry con Level-Matching]
        AudioBuffer --> InGain[Input Gain Stage ±18dB]
        
        InGain --> LowCut[Low Cut / Sub-Tamer HighPass 20-80Hz]
        LowCut --> EQ1[EQ Banda 1: Low Shelf 30-150Hz]
        EQ1 --> EQ2[EQ Banda 2: Low-Mid Bell 150-800Hz]
        EQ2 --> EQ3[EQ Banda 3: Mid Bell 800-3500Hz]
        EQ3 --> EQ4[EQ Banda 4: High-Mid Bell 3.5k-8kHz]
        EQ4 --> EQ5[EQ Banda 5: High Shelf Air 8k-20kHz]
        
        EQ5 --> Sat[WaveShaper 4x Oversampled: Tape / Tube / Class-A]
        
        Sat --> Crossover[Crossover Linkwitz-Riley 3 Vías]
        Crossover -->|Bajos <160Hz| CompL[Compresor de Graves]
        Crossover -->|Medios 160Hz-4.2kHz| CompM[Compresor de Medios]
        Crossover -->|Agudos >4.2kHz| CompH[Compresor de Agudos]
        CompL --> SumMb[Sumador Multibanda]
        CompM --> SumMb
        CompH --> SumMb
        
        SumMb --> MidSide[Procesador Mid/Side & Mono-Maker <110Hz]
        MidSide --> LoudDrive[Loudness Maximizer Drive 0-14dB]
        LoudDrive --> Limiter[Lookahead True-Peak Brickwall Limiter]
        Limiter --> Ceiling[Ceiling Normalizer Gain -0.1 a -1.5 dBTP]
        
        Ceiling --> WetGain[Wet Crossfader Gain]
        DryBranch --> DryGain[Dry Crossfader Gain]
        
        WetGain --> MasterOut[Master Output Sum]
        DryGain --> MasterOut
        
        MasterOut --> PostAnal[AnalyserNode: Post-Master FFT 2048]
        PostAnal --> AudioDest[AudioContext.destination]
    end

    subgraph "Capa de Telemetría y Renderizado"
        PostAnal --> Canvas60FPS[Renderizado Canvas 60 FPS: Espectro Dual + Goniometro]
        AudioBuffer -.-> OfflineRender[OfflineAudioContext Render]
        OfflineRender --> WavEncoder[Codificador WAV RIFF 24-bit PCM con TPDF Dither]
        WavEncoder --> DownloadMaster[Archivo WAV Masterizado Descargable]
    end
```

---

## 2. Especificación de los Módulos DSP

### 2.1. Entrada y Filtro Sub-Tamer (High-Pass)
- **Rango**: 20 Hz a 80 Hz, $Q = 0.707$ (Butterworth de 2º orden).
- **Objetivo**: Eliminar sub-frecuencias inaudibles por debajo de 30Hz que consumen margen dinámico (*headroom*) del limitador y provocan sobre-excursión en woofers.

### 2.2. Ecualizador Paramétrico de 5 Bandas
Filtros biquad de fase mínima con modulación suave de coeficientes:
1. **Low Shelf**: Graves profundos (30–150 Hz, $\pm 12$ dB).
2. **Low-Mid Bell**: Control de "caja" o resonancia turbia (150–800 Hz, $Q = 0.5 - 5.0$, $\pm 9$ dB).
3. **Mid Bell**: Presencia de voces e instrumentos líderes (800–3500 Hz, $Q = 0.5 - 5.0$, $\pm 9$ dB).
4. **High-Mid Bell**: Brillo y ataque de caja/platos (3500–8000 Hz, $Q = 0.5 - 5.0$, $\pm 9$ dB).
5. **High Shelf (Air Sheen)**: Aire sedoso analógico (8000–20000 Hz, $\pm 12$ dB).

### 2.3. Generador de Armónicos y Saturación Analógica
Utiliza un `WaveShaperNode` con factor de sobremuestreo `4x` (*oversampling*) para evitar el aliasing digital reflejado:
- **Cinta Analógica (*Tape Warmth*)**:
  $$y(x) = \tanh(x \cdot (1 + 1.5 \cdot d)) + 0.12 \cdot w \cdot (x^2 - 0.25)$$
  donde $d$ es el valor de *Drive* y $w$ es el factor de *Warmth*.
- **Válvula Triodo (*Tube Saturation*)**:
  $$y(x) = \begin{cases} \frac{1 - e^{-x(1+2d)}}{1 - e^{-2}} + 0.15 w \sin(\pi x) & x \ge 0 \\ -\tanh(-x(1+1.2d)) + 0.15 w \sin(\pi x) & x < 0 \end{cases}$$
- **Clase A (*Class-A Exciter*)**:
  $$y(x) = \frac{2}{\pi} \arctan(x \cdot (1 + 2.2 d))$$

### 2.4. Dinámica Multibanda de 3 Vías
- **Divisor de Frecuencia (*Crossover*)**: Linkwitz-Riley aproximado de 24 dB/octava en 160 Hz y 4200 Hz.
- Cada banda cuenta con control de umbral (*Threshold*), relación (*Ratio*), ataque (*Attack*), relajación (*Release*) y ganancia de compensación (*Makeup Gain*).

### 2.5. Procesador Mid/Side e Imagen Estéreo
1. Matriz M/S:
   $$\text{Mid} = \frac{L + R}{2}, \quad \text{Side} = \frac{L - R}{2}$$
2. El canal *Side* atraviesa un filtro pasa-altos centrado en el parámetro `monoMakerFreq` (110 Hz por defecto), colapsando el subgrave a mono absoluto para compatibilidad con discotecas, sistemas 2.1 y prensado de vinilo.
3. El canal *Side* se escala por el multiplicador de anchura ($\text{width} = 0.5$ a $2.0$).
4. Reconstrucción estéreo:
   $$L_{\text{out}} = \text{Mid} + \text{Side}_{\text{proc}}, \quad R_{\text{out}} = \text{Mid} - \text{Side}_{\text{proc}}$$

### 2.6. Maximizador de Volumen y Limitador True-Peak
- Curva de compresión rápida con relación brickwall (20:1), tiempo de ataque ultra-rápido de 1ms y tiempo de relajación adaptativo (50ms a 150ms).
- Margen de techo (*Ceiling*) configurable de -0.1 dBTP a -1.5 dBTP para cumplimiento de normalización en Spotify, Apple Music, Tidal y YouTube.

---

## 3. Orquestación Multi-Agente en Runtime

```mermaid
sequenceDiagram
    autonumber
    actor Usuario
    participant Orch as 🧠 Orquestador (agentes-personalizados)
    participant Arch as 📐 Arquitecto Acústico (arquitecto-documentador)
    participant Rev as 🛡️ Revisor Anti-Distorsión (revisor-codigo)
    participant QA as 🧪 QA & Tester (generador-pruebas)
    participant DSP as 🎛️ DSPEngine (Web Audio)

    Usuario->>Orch: Clic en 'AUTO-MASTER AI'
    Orch->>Orch: Escaneo FFT y muestreo de Rango Dinámico (Crest Factor)
    Orch->>Arch: Telemetría: Peak, RMS y balances espectrales
    Arch->>Arch: Calcular curvas EQ y atenuación de frecuencias parásitas (Mud/Harshness)
    Arch->>Rev: Propuesta de perilla EQ y ganancia de saturación
    Rev->>Rev: Verificar umbrales multibanda y límites de seguridad anti-clipping
    Rev->>QA: Parámetros validados para cálculo de Loudness Drive
    QA->>QA: Ajustar ganancia de maximización hacia Target LUFS y verificar Ceiling dBTP
    QA->>DSP: Aplicar configuración de masterización al grafo Web Audio
    DSP->>Usuario: Feedback auditivo inmediato y animación de perillas
```

---

## 4. Exportación Offline a 24-bit PCM Broadcast WAV

El proceso de renderizado offline no depende de la velocidad de reproducción del sistema:
- Crea una instancia de `OfflineAudioContext(2, totalSamples, sampleRate)`.
- Instancia réplicas exactas de los nodos de filtrado, saturación, crossover y limitación.
- Procesa el audio más rápido que el tiempo real a 64 bits de precisión de punto flotante.
- Codifica las muestras a 24-bit little-endian aplicando **Dither TPDF (Triangular Probability Density Function)** para neutralizar el ruido de cuantización digital.
- Genera un `Blob` de tipo `audio/wav` con cabecera estándar RIFF WAVE para descarga directa.
