import os

KIT_DIR = r"c:\Users\alfaswz\.gemini\antigravity-ide\scratch\agentes-personalizados\SUPREME-PRODUCER-DRUM-KIT"

# 1. install.bat
install_bat = """@echo off
chcp 65001 >nul
title Supreme Producer Drum Kit - Auto-Installer
cls
echo =====================================================================
echo    🔥 SUPREME PRODUCER DRUM KIT — AUTO-INSTALLER PARA FL STUDIO
echo =====================================================================
echo.

set "TARGET_BASE=%USERPROFILE%\\Documents\\Image-Line\\FL Studio\\Audio\\Packs"

if not exist "%USERPROFILE%\\Documents\\Image-Line" (
    if exist "%USERPROFILE%\\OneDrive\\Documents\\Image-Line" (
        set "TARGET_BASE=%USERPROFILE%\\OneDrive\\Documents\\Image-Line\\FL Studio\\Audio\\Packs"
    )
)

set "TARGET_DIR=%TARGET_BASE%\\Supreme Producer Drum Kit"

echo [*] Carpeta de destino en FL Studio:
echo     "%TARGET_DIR%"
echo.

if not exist "%TARGET_DIR%" (
    echo [*] Creando directorio en Packs de FL Studio...
    mkdir "%TARGET_DIR%"
)

echo [*] Copiando carpetas de sonidos, loops y midis...
xcopy /E /I /Y /Q "%~dp001_808s_&_Basses" "%TARGET_DIR%\\01_808s_&_Basses" >nul
xcopy /E /I /Y /Q "%~dp002_Kicks" "%TARGET_DIR%\\02_Kicks" >nul
xcopy /E /I /Y /Q "%~dp003_Snares_&_Rims" "%TARGET_DIR%\\03_Snares_&_Rims" >nul
xcopy /E /I /Y /Q "%~dp004_Claps" "%TARGET_DIR%\\04_Claps" >nul
xcopy /E /I /Y /Q "%~dp005_HiHats_&_Cymbals" "%TARGET_DIR%\\05_HiHats_&_Cymbals" >nul
xcopy /E /I /Y /Q "%~dp006_Percussion" "%TARGET_DIR%\\06_Percussion" >nul
xcopy /E /I /Y /Q "%~dp007_FX_&_Chants" "%TARGET_DIR%\\07_FX_&_Chants" >nul
xcopy /E /I /Y /Q "%~dp008_Melody_Loops_(Auto_Tempo)" "%TARGET_DIR%\\08_Melody_Loops_(Auto_Tempo)" >nul
xcopy /E /I /Y /Q "%~dp009_Drum_Loops_(Auto_Tempo)" "%TARGET_DIR%\\09_Drum_Loops_(Auto_Tempo)" >nul
xcopy /E /I /Y /Q "%~dp010_MIDI_Patterns" "%TARGET_DIR%\\10_MIDI_Patterns" >nul
copy /Y "%~dp0cover.jpg" "%TARGET_DIR%\\" >nul

if %errorlevel% equ 0 (
    echo.
    echo =====================================================================
    echo   [EXITO] Drum Kit instalado correctamente en FL Studio!
    echo =====================================================================
    echo.
    echo   COMO USAR EN FL STUDIO:
    echo   1. Abre FL Studio.
    echo   2. En el navegador izquierdo (Browser), ve a la carpeta 'Packs'.
    echo   3. Veras la carpeta 'Supreme Producer Drum Kit' con todos los sonidos.
    echo   4. Arrastra cualquier sonido, loop o patron MIDI al Channel Rack
    echo      o a la Playlist.
    echo.
    echo   NOTA AUTO-TEMPO:
    echo   Los loops de melodia y bateria contienen etiquetas ACID WAV internas.
    echo   Al arrastrarlos a la Playlist, FL Studio los sincroniza
    echo   automaticamente al tempo de tu proyecto!
    echo.
) else (
    echo [AVISO] Puedes arrastrar la carpeta directamente al navegador de FL Studio.
)

pause
"""

with open(os.path.join(KIT_DIR, "install.bat"), "w", encoding="utf-8") as f:
    f.write(install_bat)

# 2. install.ps1
install_ps1 = """# Supreme Producer Drum Kit - PowerShell Installer
Write-Host "=====================================================================" -ForegroundColor Cyan
Write-Host "   🔥 SUPREME PRODUCER DRUM KIT — AUTO-INSTALLER" -ForegroundColor Yellow
Write-Host "=====================================================================" -ForegroundColor Cyan

$docs = [Environment]::GetFolderPath('MyDocuments')
$targetBase = Join-Path $docs "Image-Line\\FL Studio\\Audio\\Packs"

if (-not (Test-Path -Path (Split-Path $targetBase -Parent))) {
    $oneDrive = Join-Path $env:USERPROFILE "OneDrive\\Documents\\Image-Line\\FL Studio\\Audio\\Packs"
    if (Test-Path -Path (Split-Path $oneDrive -Parent)) {
        $targetBase = $oneDrive
    }
}

$targetDir = Join-Path $targetBase "Supreme Producer Drum Kit"
Write-Host "[*] Destino: $targetDir" -ForegroundColor Gray

if (-not (Test-Path -Path $targetDir)) {
    New-Item -ItemType Directory -Path $targetDir -Force | Out-Null
}

$folders = @(
    "01_808s_&_Basses", "02_Kicks", "03_Snares_&_Rims", "04_Claps",
    "05_HiHats_&_Cymbals", "06_Percussion", "07_FX_&_Chants",
    "08_Melody_Loops_(Auto_Tempo)", "09_Drum_Loops_(Auto_Tempo)", "10_MIDI_Patterns"
)

foreach ($f in $folders) {
    $src = Join-Path $PSScriptRoot $f
    $dst = Join-Path $targetDir $f
    if (Test-Path $src) {
        Copy-Item -Path $src -Destination $targetDir -Recurse -Force
        Write-Host "  [+] Copiado: $f" -ForegroundColor Green
    }
}

Copy-Item -Path (Join-Path $PSScriptRoot "cover.jpg") -Destination $targetDir -Force

Write-Host "`n[EXITO] Drum Kit instalado en FL Studio Browser (Packs)!" -ForegroundColor Green
Write-Host "Presiona cualquier tecla para salir..."
[void][System.Console]::ReadKey()
"""

with open(os.path.join(KIT_DIR, "install.ps1"), "w", encoding="utf-8") as f:
    f.write(install_ps1)

# 3. LICENSE
license_text = """MIT License - 100% Royalty Free

Copyright (c) 2026 vevikils

Permission is hereby granted, free of charge, to any person obtaining a copy
of this audio library, software and associated documentation files (the "Software"),
to use, reproduce, modify, distribute, perform, broadcast, and synchronize
the audio samples and MIDI files in any personal, commercial, or non-commercial
music releases, beat sales, streaming platforms, or multimedia productions
without paying royalties or attribution fees to the authors.

THE SOFTWARE AND SOUNDS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND.
"""

with open(os.path.join(KIT_DIR, "LICENSE"), "w", encoding="utf-8") as f:
    f.write(license_text)

# 4. README.md
readme_text = """<p align="center">
  <img src="cover.jpg" alt="Supreme Producer Drum Kit Cover" width="100%" style="border-radius: 14px; box-shadow: 0 10px 35px rgba(0,0,0,0.6);">
</p>

<h1 align="center">🔥 SUPREME PRODUCER DRUM KIT & MELODY SUITE</h1>
<p align="center">
  <b>El kit definitivo de producción musical para FL Studio, Ableton, Logic Pro y cualquier DAW</b><br>
  <i>One-Shots, 808s afinados en C, Kicks, Snares, Claps, Hi-Hats, Percusiones, FX, Loops de Melodía con Auto-Tempo y Patrones MIDI.</i>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/FL_Studio-Compatible-ff6f00?style=for-the-badge&logo=flstudio&logoColor=white" alt="FL Studio">
  <img src="https://img.shields.io/badge/Format-WAV_24/16bit_+_MIDI-00c853?style=for-the-badge" alt="WAV & MIDI">
  <img src="https://img.shields.io/badge/Tempo_Sync-ACID_Auto--Stretch-7928ca?style=for-the-badge" alt="ACID Auto-Stretch">
  <img src="https://img.shields.io/badge/License-100%25_Royalty_Free-0070f3?style=for-the-badge" alt="100% Royalty Free">
</p>

---

## ⚡ ¿Qué incluye este Drum Kit?

El **Supreme Producer Drum Kit** ha sido diseñado desde cero con algoritmos acústicos y técnicas de síntesis DSP avanzadas para ofrecer una pegada moderna, limpia y contundente sin saturaciones indeseadas.

- 💎 **52 Archivos de Audio y MIDI de Alta Definición**.
- 🎹 **808s Afinados en C**: Todos los bajos 808 están perfectamente afinados en C (Do) para que puedas tocar cualquier escala en el Piano Roll sin desafinar.
- ⏱️ **Auto-Ajustable al Tempo (WAV ACIDized)**: Todos los Melody Loops y Drum Loops integran metadatos RIFF `acid`. Al arrastrarlos a la Playlist de **FL Studio**, se sincronizan y adaptan automáticamente al BPM de tu proyecto sin alterar el tono (*Time Stretching automático*).
- 🎼 **Patrones MIDI Profesionales**: Incluye archivos `.mid` de redobles de hi-hats (triplets, rolls 1/32), progresiones de acordes, melodías de piano drill y ritmos de dembow.
- 🔓 **100% Libre de Regalías (Royalty-Free)**: Puedes usarlo en tus canciones en Spotify, Apple Music, YouTube o venta de beats sin pagar licencias.

---

## 📂 Estructura y Contenido del Kit

### 🔊 01. 808s & Sub Basses (Afinados en C)
* `808_Spinz_Classic_C.wav` — El 808 clásico con armónicos cálidos y pegada frontal que corta en altavoces de teléfono.
* `808_Sub_Heavyweight_C.wav` — Subgrave profundo y demoledor para frecuencias de 30-60Hz.
* `808_Drill_Slide_Distorted_C.wav` — 808 agresivo para UK/NY Drill con saturación de tubo y curva de deslizamiento.
* `808_Pluck_Punchy_C.wav` — 808 corto y tenso para ritmos rápidos y tempos altos.
* `808_Cyber_Sub_C.wav` — Subgrave futurista con modulación sutil y cuerpo estéreo.

### 💥 02. Kicks (Pegada sin cancelaciones de fase)
* `Kick_Hard_Hitter_Trap.wav` — Golpe seco a 52Hz con click transitorio afilado.
* `Kick_Acoustic_Layered.wav` — Capa orgánica combinada con pegada electrónica.
* `Kick_Dembow_Punch_Latin.wav` — Kick redondo y contundente diseñado para el ritmo del reggaeton.
* `Kick_Drill_Knock.wav` — Golpe corto de respuesta rápida para convivir con 808s deslizantes.
* `Kick_Sub_Thump.wav` — Golpe con peso en subgraves para trap atmosférico.

### 🎯 03. Snares & Rims
* `Snare_Trap_Classic_Smack.wav` — Caja clásica de trap con cuerpo en 185Hz y ruido brillante.
* `Snare_Drill_Ghost_Metallic.wav` — Snare con timbre metálico para contratiempos de drill.
* `Snare_Reggaeton_Dembow.wav` — Snare acústico y cortante para el patrón característico de dembow.
* `Rimshot_Clean_Wood.wav` — Golpe de aro de madera limpio y orgánico.
* `Snare_Layer_Clap_Fusion.wav` — Capa híbrida de caja + aplauso para estribillos explosivos.

### 👏 04. Claps
* `Clap_Trap_Crisp.wav` — Clap con ráfagas previas espaciadas para máxima tensión rítmica.
* `Clap_Stadium_Wide.wav` — Clap con ensanchamiento estéreo y apertura dimensional.
* `Clap_Short_Dry.wav` — Clap apretado y seco para bounce rápido.
* `Clap_Vintage_Analog.wav` — Aplauso analógico inspirado en cajas de ritmo clásicas.

### 🎩 05. Hi-Hats & Cymbals
* `HiHat_Closed_Crisp_Clean.wav` — Hi-hat cerrado filtrado por encima de 7.5kHz para evitar asperezas.
* `HiHat_Closed_Metallic_Drill.wav` — Hi-hat ultracorto ideal para redobles a 1/32 y 1/64.
* `HiHat_Open_Long_Sustain.wav` — Hi-hat abierto con caída suave para contratiempos.
* `HiHat_Open_Pedal_Choke.wav` — Hi-hat abierto corto con ahogo de pedal.
* `Crash_Cymbal_Impact.wav` — Platillo de impacto brillante con estéreo reverb envolvente.

### 🥁 06. Percussion
* `Perc_Phonk_Cowbell.wav` — Cencerro sintetizado con doble oscilador para Memphis Rap / Phonk.
* `Perc_Woodblock_Organic.wav` — Bloque de madera percusivo a 1200Hz.
* `Perc_Metal_Pipe_Hit.wav` — Impacto metálico inarmónico industrial.
* `Perc_Bongo_Conga_Slap.wav` — Golpe de membrana percusiva latina.
* `Perc_Shaker_Loop_Hit.wav` — Transitorio de maraca/shaker con textura granular.

### 🌌 07. FX & Chants
* `FX_Vocal_Chant_Hey.wav` — Cántico vocal sintético "Hey!" con formantes humanos.
* `FX_Riser_4Bar_Sweep_140BPM.wav` — Subida de tensión de 4 compases auto-sincronizada a 140 BPM.
* `FX_Sub_Drop_Impact.wav` — Caída cinemática de subgrave de 130Hz a 25Hz.
* `FX_Laser_Zap_Trap.wav` — Disparo láser electrónico retro-futurista.
* `FX_Vinyl_Crackling_Texture.wav` — Ruido y textura de vinilo analógico para fondos.

### 🎹 08. Melody Loops (Auto-Ajustables al Tempo)
| Loop | BPM | Tonalidad (Key) | Estilo |
| :--- | :--- | :--- | :--- |
| `Melody_140BPM_Cm_Astroworld_Space_Synth.wav` | 140 BPM | C Minor (Cm) | Acordes y arpegio espacial estilo Travis Scott / Mike Dean |
| `Melody_130BPM_Am_Tokyo_Night_Pluck.wav` | 130 BPM | A Minor (Am) | Pluck pentatónico melódico con campana shimmer |
| `Melody_96BPM_Fm_Latin_Reggaeton_Vibes.wav` | 96 BPM | F Minor (Fm) | Stabs de acordes y sinte melódico estilo Tainy / Bad Bunny |
| `Melody_142BPM_Em_Dark_Drill_Piano.wav` | 142 BPM | E Minor (Em) | Piano oscuro con arpegios de tensión para UK/NY Drill |

### 🥁 09. Drum Loops (Auto-Ajustables al Tempo)
| Loop | BPM | Descripción |
| :--- | :--- | :--- |
| `DrumLoop_140BPM_Full_Trap_Bounce.wav` | 140 BPM | Beat completo de trap con Kick, Clap en el 3, redobles y 808 |
| `DrumLoop_140BPM_Top_Loop.wav` | 140 BPM | Loop de hi-hats, clap y percusión sin kick ni bajo para mezclar libremente |
| `DrumLoop_142BPM_UK_Drill_Groove.wav` | 142 BPM | Patrón sincopado de drill con ghost snares y bounce deslizante |
| `DrumLoop_96BPM_Reggaeton_Perreo_Beat.wav` | 96 BPM | Base completa de dembow de reggaeton con pegada dura |

### 🎼 10. MIDI Patterns (Arrastra al Piano Roll)
* `MIDI_HiHat_Roll_140BPM_Trap.mid` — Patrón de hi-hats con dinámicas de velocidad y redobles en tresillos.
* `MIDI_HiHat_Bounce_142BPM_Drill.mid` — Saltos rítmicos sincopados de hi-hats para drill.
* `MIDI_808_Trap_Pattern_Cm.mid` — Línea de bajo 808 pegadiza en C menor.
* `MIDI_Snare_Clap_Trap_140BPM.mid` — Aplausos y ghost snares listos para usar.
* `MIDI_Reggaeton_Dembow_Groove.mid` — Patrón MIDI completo de batería dembow.
* `MIDI_Melody_Astroworld_Cm.mid` — Progresión de acordes completa en C menor.
* `MIDI_Melody_Tokyo_Pluck_Am.mid` — Melodía completa en A menor.
* `MIDI_Melody_Drill_Piano_Em.mid` — Arpegio y notas de tensión en E menor.

---

## 🚀 Instalación Rápida

### Opción 1: Instalador Automático en Windows
1. Descarga el paquete ZIP desde **[Releases](../../releases)**.
2. Descomprímelo en tu ordenador.
3. Haz doble clic en **`install.bat`**.
4. El instalador detectará tu carpeta de **FL Studio** y colocará el kit dentro de `Browser ➔ Packs ➔ Supreme Producer Drum Kit`.

### Opción 2: Instalación Manual en cualquier DAW (FL Studio, Ableton, Logic, Reaper)
Simplemente arrastra la carpeta descompresa directamente al navegador de archivos de tu DAW favorito o cópiala en tu librería de samples habitual.

---

## 🎛️ ¿Cómo funciona el Auto-Ajuste de Tempo en FL Studio?

Los loops de este kit contienen **cabeceras ACID WAV**:
1. Arrastra cualquier loop de las carpetas `08_Melody_Loops` o `09_Drum_Loops` a la Playlist de FL Studio.
2. Haz doble clic en el clip de audio.
3. Verás que el modo de **Time Stretching** se activa automáticamente y el tiempo encaja al milímetro con el compás de tu proyecto, sea cual sea el BPM.

---

## 📄 Licencia

Este paquete está distribuido bajo licencia [MIT (100% Royalty Free)](LICENSE). Eres libre de usar estos sonidos en tus producciones comerciales, beats y canciones sin coste adicional.

---

<p align="center">
  Diseñado con pasión para productores, beatmakers y compositores.
</p>
"""

with open(os.path.join(KIT_DIR, "README.md"), "w", encoding="utf-8") as f:
    f.write(readme_text)

print("Installers, License and README created successfully!")
