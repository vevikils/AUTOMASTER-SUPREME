import os

REPO_DIR = r"c:\Users\alfaswz\.gemini\antigravity-ide\scratch\agentes-personalizados\SUPREME-VOCAL-CHAINS-FL-STUDIO"

readme_text = """<p align="center">
  <img src="cover.jpg" alt="Supreme Vocal Chains FL Studio Cover" width="100%" style="border-radius: 12px; box-shadow: 0 8px 30px rgba(0,0,0,0.5);">
</p>

<h1 align="center">🎙️ SUPREME VOCAL CHAINS — FL STUDIO</h1>
<p align="center">
  <b>El pack definitivo de cadenas de mezcla de voces profesionales para FL Studio (Stock Plugins)</b>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/FL_Studio-20%20%7C%2021%20%7C%2024+-ff6f00?style=for-the-badge&logo=flstudio&logoColor=white" alt="FL Studio">
  <img src="https://img.shields.io/badge/Plugins-100%25%20Stock%20Native-00c853?style=for-the-badge" alt="Stock Plugins">
  <img src="https://img.shields.io/badge/Compatibility-Windows%20%7C%20macOS-7928ca?style=for-the-badge" alt="Compatibility">
  <img src="https://img.shields.io/badge/License-MIT-0070f3?style=for-the-badge" alt="License">
</p>

---

## ⚡ ¿Qué es Supreme Vocal Chains?

**Supreme Vocal Chains** es un paquete de 5 cadenas de mezcla vocal de calibre profesional inspiradas en los mayores exponentes de la música urbana y trap mundial: **Travis Scott, Duki, Anuel AA, Bad Bunny y Ozuna**.

Cada preset ha sido meticulosamente calibrado utilizando **exclusivamente plugins nativos (stock) de FL Studio**. Esto significa:
- ❌ **CERO plugins externos de pago** (ni Waves, ni FabFilter, ni Antares, ni Soundtoys).
- ✅ **100% Compatible y Plug & Play**: Cargas el preset y suena de estudio al instante.
- ✅ **10 Slots de procesamiento activo**: Autotune en tiempo real, EQ quirúrgica, saturación analógica, compresión multibanda, de-essing, ensanchamiento estéreo y delays/reverbs espaciales.

---

## 📦 Artistas y Presets Incluidos

| Preset | Artista | Estilo / Perfil Acústico |
| :--- | :--- | :--- |
| **`TRAVIS SCOTT - VOCAL CHAIN (PRO).fst`** | 🌵 Travis Scott | *Psychedelic Space Trap* — Hard tuning, delay ping-pong infinito, reverb cósmica y calidez analógica tipo Astroworld / Utopia. |
| **`DUKI - SUPER ROBOTIC TRAP (PRO).fst`** | ⚡ Duki | *Argentine Hard Trap* — Autotune súper robótico con corrección agresiva al 100%, pegada de medios afilada para cortar en 808s pesados. |
| **`ANUEL AA - MELODIC TRAP (PRO).fst`** | 👑 Anuel AA | *Melodic Street Trap & Reggaeton* — Voz brillante, presente, de-esser quirúrgico, presencia frontal y reverb expansiva de estadio. |
| **`BAD BUNNY - PRIME ERA (PRO).fst`** | 🐰 Bad Bunny | *Deep Baritone Warmth* — Tono barítono con cuerpo en frecuencias bajas-medias (150-300Hz), saturación cálida de cinta y dimensión oscura. |
| **`OZUNA - VOZ AGUDA (PRO).fst`** | 🐻 Ozuna | *Crystalline High Tenor* — Claridad cristalina en agudos, aire sedoso a 12-16kHz, brillo estéreo aterciopelado y cola de reverb flotante. |

---

## 🎛️ Arquitectura de la Cadena (10 Slots Nativos)

Cada preset aprovecha al máximo la arquitectura de procesamiento del mixer de FL Studio:

```text
Entrada Vocal (Mic)
      │
      ▼
 [SLOT 1]  Pitcher                ➜ Corrección tonal y afinación en tiempo real (Autotune)
      │
 [SLOT 2]  Fruity Parametric EQ 2 ➜ Filtro paso alto (sub-cut) y ecualización correctiva
      │
 [SLOT 3]  Soundgoodizer          ➜ Excitador armónico y saturación de brillo y pegada
      │
 [SLOT 4]  Fruity Waveshaper      ➜ Distorsión cálida y carácter de cinta analógica
      │
 [SLOT 5]  Maximus                ➜ Compresor multibanda, de-esser y nivelación dinámica
      │
 [SLOT 6]  Fruity Chorus          ➜ Micro-desafinado y ensanchamiento vocal
      │
 [SLOT 7]  Fruity Stereo Enhancer ➜ Apertura estéreo y posicionamiento central
      │
 [SLOT 8]  Fruity Delay 3         ➜ Delay estéreo sincronizado con el tempo del proyecto
      │
 [SLOT 9]  Fruity Reverb 2        ➜ Sala tridimensional, profundidad y calidez acústica
      │
 [SLOT 10] Fruity Limiter         ➜ Techo protector, control de picos y volumen comercial
      │
      ▼
Salida al Master
```

---

## 🚀 Instalación Rápida (1-Click)

### Opción 1: Instalador Automático en Windows (Recomendada)
1. Descarga el archivo zip desde la pestaña de **[Releases](../../releases)**.
2. Descomprime la carpeta.
3. Haz doble clic en **`install.bat`**.
4. ¡Listo! El script detectará tu instalación de FL Studio y colocará los presets en su carpeta oficial.

---

### Opción 2: Instalación Manual
Copia los 5 archivos `.fst` de la carpeta `Presets/` en la siguiente ruta:

* **Windows:**
  ```text
  %USERPROFILE%\\Documents\\Image-Line\\FL Studio\\Presets\\Mixer presets\\Supreme Vocal Chains\\
  ```
  *(o en OneDrive: `C:\\Users\\<TuUsuario>\\OneDrive\\Documentos\\Image-Line\\FL Studio\\Presets\\Mixer presets\\Supreme Vocal Chains\\`)*

* **macOS:**
  ```text
  ~/Documents/Image-Line/FL Studio/Presets/Mixer presets/Supreme Vocal Chains/
  ```

---

## 🎧 Cómo Usar en FL Studio

1. Abre tu proyecto en FL Studio y presiona **`F9`** para abrir el **Mixer**.
2. Selecciona el canal donde grabas o tienes tu voz (por ejemplo, *Insert 1*).
3. **Clic derecho** sobre el canal del mixer ➔ **File** ➔ **Open mixer track state...**
4. Abre la carpeta **Supreme Vocal Chains** y selecciona el artista que desees.
5. *(Opcional)* También puedes simplemente **arrastrar y soltar el archivo `.fst` directamente sobre el canal del mixer**.

### 🎹 Ajuste de Tono (Pitcher):
- En el **Slot 1 (Pitcher)**, asegúrate de colocar la **Tonalidad (Key)** y la **Escala (Scale: Major / Minor)** correspondiente a la instrumental de tu canción para que el autotune afine de forma perfecta y en armonía.

---

## 🛠️ Requisitos del Sistema
- **FL Studio:** FL Studio 20.8 o superior, FL Studio 21, FL Studio 24 (Cualquier edición: Producer, Signature, All Plugins).
- **Plugins requeridos:** Ninguno externo. Solo los plugins incluidos de fábrica en FL Studio.
- **Sistemas Operativos:** Windows 10/11, macOS Catalina / Big Sur / Monterey / Ventura / Sonoma.

---

## 📄 Licencia

Este proyecto está bajo la licencia [MIT](LICENSE) — Siéntete libre de usarlo en tus producciones musicales, canciones comerciales y proyectos personales.

---

<p align="center">
  Hecho con ❤️ para la comunidad de productores y artistas urbanos.
</p>
"""

readme_path = os.path.join(REPO_DIR, "README.md")
with open(readme_path, "w", encoding="utf-8") as f:
    f.write(readme_text)

print(f"README.md written ({len(readme_text)} bytes) to {readme_path}")
