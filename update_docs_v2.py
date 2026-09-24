import os
import shutil
import zipfile
import subprocess

KIT_DIR = r"c:\Users\alfaswz\.gemini\antigravity-ide\scratch\agentes-personalizados\SUPREME-PRODUCER-DRUM-KIT"

# 1. Update install.bat
install_bat = """@echo off
chcp 65001 >nul
title Supreme Producer Drum Kit v2.0 - Auto-Installer
cls
echo =====================================================================
echo    🔥 SUPREME PRODUCER SUITE v2.0 — AUTO-INSTALLER PARA FL STUDIO
echo    (32 Melodias + 62 Drum Loops de Hits + One-Shots + MIDIs)
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

echo [*] Copiando librerias completas (150+ archivos)...
xcopy /E /I /Y /Q "%~dp001_808s_&_Basses" "%TARGET_DIR%\\01_808s_&_Basses" >nul
xcopy /E /I /Y /Q "%~dp002_Kicks" "%TARGET_DIR%\\02_Kicks" >nul
xcopy /E /I /Y /Q "%~dp003_Snares_&_Rims" "%TARGET_DIR%\\03_Snares_&_Rims" >nul
xcopy /E /I /Y /Q "%~dp004_Claps" "%TARGET_DIR%\\04_Claps" >nul
xcopy /E /I /Y /Q "%~dp005_HiHats_&_Cymbals" "%TARGET_DIR%\\05_HiHats_&_Cymbals" >nul
xcopy /E /I /Y /Q "%~dp006_Percussion" "%TARGET_DIR%\\06_Percussion" >nul
xcopy /E /I /Y /Q "%~dp007_FX_&_Chants" "%TARGET_DIR%\\07_FX_&_Chants" >nul
xcopy /E /I /Y /Q "%~dp008_Melody_Samples_32_(Hit_Inspired_Auto_Tempo)" "%TARGET_DIR%\\08_Melody_Samples_32_(Hit_Inspired_Auto_Tempo)" >nul
xcopy /E /I /Y /Q "%~dp009_Drum_Loops_62_(Genre_Beats_Auto_Tempo)" "%TARGET_DIR%\\09_Drum_Loops_62_(Genre_Beats_Auto_Tempo)" >nul
xcopy /E /I /Y /Q "%~dp010_MIDI_Patterns" "%TARGET_DIR%\\10_MIDI_Patterns" >nul
copy /Y "%~dp0cover.jpg" "%TARGET_DIR%\\" >nul

if %errorlevel% equ 0 (
    echo.
    echo =====================================================================
    echo   [EXITO] Supreme Producer Suite v2.0 instalado en FL Studio!
    echo =====================================================================
    echo.
    echo   COMO USAR EN FL STUDIO:
    echo   1. Abre FL Studio.
    echo   2. En el navegador izquierdo (Browser), ve a la carpeta 'Packs'.
    echo   3. Abre 'Supreme Producer Drum Kit'.
    echo   4. Arrastra cualquier muestra, loop o MIDI a la Playlist o Channel Rack.
    echo.
    echo   AUTO-TEMPO:
    echo   Los 32 samples de melodias y los 62 drum loops contienen cabeceras
    echo   ACID WAV que se sincronizan automaticamente al BPM de tu proyecto!
    echo.
) else (
    echo [AVISO] Puedes arrastrar la carpeta directamente al navegador de FL Studio.
)

pause
"""
with open(os.path.join(KIT_DIR, "install.bat"), "w", encoding="utf-8") as f:
    f.write(install_bat)

# 2. Update install.ps1
install_ps1 = """# Supreme Producer Drum Kit v2.0 - PowerShell Installer
Write-Host "=====================================================================" -ForegroundColor Cyan
Write-Host "   🔥 SUPREME PRODUCER SUITE v2.0 — AUTO-INSTALLER" -ForegroundColor Yellow
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
    "08_Melody_Samples_32_(Hit_Inspired_Auto_Tempo)",
    "09_Drum_Loops_62_(Genre_Beats_Auto_Tempo)",
    "10_MIDI_Patterns"
)

foreach ($f in $folders) {
    $src = Join-Path $PSScriptRoot $f
    if (Test-Path $src) {
        Copy-Item -Path $src -Destination $targetDir -Recurse -Force
        Write-Host "  [+] Copiado: $f" -ForegroundColor Green
    }
}

Copy-Item -Path (Join-Path $PSScriptRoot "cover.jpg") -Destination $targetDir -Force

Write-Host "`n[EXITO] Drum Kit Suite v2.0 instalado en FL Studio (Packs)!" -ForegroundColor Green
Write-Host "Presiona cualquier tecla para salir..."
[void][System.Console]::ReadKey()
"""
with open(os.path.join(KIT_DIR, "install.ps1"), "w", encoding="utf-8") as f:
    f.write(install_ps1)

# 3. Comprehensive README.md
readme_text = """<p align="center">
  <img src="cover.jpg" alt="Supreme Producer Drum Kit Cover" width="100%" style="border-radius: 14px; box-shadow: 0 10px 35px rgba(0,0,0,0.6);">
</p>

<h1 align="center">🔥 SUPREME PRODUCER SUITE v2.0</h1>
<p align="center">
  <b>El arsenal definitivo de producción musical para FL Studio, Ableton Live, Logic Pro y cualquier DAW</b><br>
  <i>32 Melodías Complejas inspiradas en Hits Mundiales + 62 Drum Loops de Género + Drum Kit Completo + MIDIs</i>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Version-2.0.0_Ultimate-ff0055?style=for-the-badge" alt="Version 2.0.0">
  <img src="https://img.shields.io/badge/Total_Assets-150+_Files-00c853?style=for-the-badge" alt="150+ Files">
  <img src="https://img.shields.io/badge/Melody_Samples-32_Hit_Inspired-ff6f00?style=for-the-badge" alt="32 Melodies">
  <img src="https://img.shields.io/badge/Drum_Loops-62_Auto--Tempo-7928ca?style=for-the-badge" alt="62 Loops">
  <img src="https://img.shields.io/badge/License-100%25_Royalty_Free-0070f3?style=for-the-badge" alt="Royalty Free">
</p>

---

## ⚡ ¿Qué hay de nuevo en la Versión 2.0?

Esta actualización masiva responde a la máxima exigencia de producción profesional:
- 🎹 **32 Samples de Melodías Complejas y Post-Procesadas**:
  - Sonido cálido, analógico y orgánico: **Cero estridencias ni frecuencias chillonas**.
  - Ecualización quirúrgica (corte suave a 6.5kHz, realce cálido en medios-graves a 250-500Hz).
  - Reverb de estudio multicámara Freeverb/Schroeder con amortiguación sedosa.
  - Multi-capas completas: Progresiones de acordes (Rhodes vintage, pads analógicos, guitarras fingerpicked), hooks melódicos (flautas de aire, campanas shimmer) y líneas de subgrave de soporte.
  - Inspiradas en los mayores éxitos mundiales de **Travis Scott, Metro Boomin, Bad Bunny, Feid, Rauw Alejandro, Pop Smoke, Central Cee, Kendrick Lamar, Dr. Dre, Rema, Burna Boy, The Weeknd y SZA**, con variaciones creativas y pegadizas.
- 🥁 **62 Drum Loops de Género (Full Beats + Top Loops)**:
  - 12 Loops de **Trap** (Astroworld, Superhero, Drip Too Hard, Lucid Dreams, Rich Flex, Mask Off).
  - 12 Loops de **Reggaeton & Dembow** (Tití Me Preguntó, Ferxxo, Todo De Ti, Monaco, Dákiti, Gasolina).
  - 12 Loops de **UK / NY Drill** (Dior, Doja, Big Drip, Body, Pelele, Stomp).
  - 12 Loops de **Hip-Hop & BoomBap / West Coast** (Not Like Us, Still D.R.E., No Role Modelz, Praise The Lord, Self Care, 90s Golden Era).
  - 14 Loops de **Afro-Urban & Modern R&B** (Calm Down, Last Last, Essence, Amapiano Log Drum, Blinding Lights, Kill Bill, TrapSoul).
- ⏱️ **Tecnología Auto-Ajustable al Tempo (WAV ACIDized)**:
  - Todos los 94 loops (32 melodías + 62 drum loops) contienen cabeceras **RIFF `acid`**. Al arrastrarlos a la Playlist de **FL Studio**, se sincronizan automáticamente al BPM de tu sesión sin perder afinación ni calidad.
- 🎯 **34 One-Shots de Batería de Alta Definición**: 808s afinados en C, kicks con pegada sin cancelaciones, cajas, claps multicapa, hi-hats ultrafiltrados, percusiones phonk/latinas y efectos FX.
- 🎼 **Patrones MIDI**: Redobles de hi-hats en tresillos, bajos 808, ritmos dembow y progresiones de acordes.

---

## 🎹 Catálogo de las 32 Melodías Complejas (Hit-Inspired)

| # | Nombre de Archivo | Género | BPM | Tonalidad | Inspiración / Vibra |
| :-: | :--- | :--- | :-: | :-: | :--- |
| **01** | `Trap_140BPM_Cm_Astroworld_Goosebumps.wav` | Trap | 140 | C Minor | Travis Scott (*Goosebumps / Astroworld*) |
| **02** | `Trap_138BPM_Am_Superhero_Cinematic.wav` | Trap | 138 | A Minor | Metro Boomin & Future (*Superhero*) |
| **03** | `Trap_132BPM_Fm_Drip_Too_Hard_Guitar.wav` | Trap | 132 | F Minor | Lil Baby & Gunna (*Drip Too Hard*) |
| **04** | `Trap_130BPM_Dm_Mask_Off_Haunting_Flute.wav` | Trap | 130 | D Minor | Future (*Mask Off*) |
| **05** | `Trap_144BPM_Em_Lucid_Dreams_Guitar.wav` | Trap | 144 | E Minor | Juice WRLD (*Lucid Dreams*) |
| **06** | `Trap_136BPM_Abm_Jimmy_Cooks_Soul.wav` | Trap | 136 | Ab Minor | Drake & 21 Savage (*Jimmy Cooks*) |
| **07** | `Trap_140BPM_Bbm_The_Box_Orchestral.wav` | Trap | 140 | Bb Minor | Roddy Ricch (*The Box*) |
| **08** | `Trap_134BPM_Gm_Pushin_P_Ambient.wav` | Trap | 134 | G Minor | Gunna & Young Thug (*Pushin P*) |
| **09** | `Reggaeton_96BPM_Fm_Titi_Me_Pregunto.wav` | Reggaeton | 96 | F Minor | Bad Bunny (*Tití Me Preguntó*) |
| **10** | `Reggaeton_92BPM_Am_Feliz_Cumpleanos_Ferxxo.wav` | Reggaeton | 92 | A Minor | Feid (*Feliz Cumpleaños Ferxxo*) |
| **11** | `Reggaeton_98BPM_Cm_Todo_De_Ti_Synth.wav` | Reggaeton | 98 | C Minor | Rauw Alejandro (*Todo De Ti*) |
| **12** | `Reggaeton_128BPM_Dm_Bzrp_Quevedo_Quedate.wav` | Latin/Dance | 128 | D Minor | Bizarrap & Quevedo (*Bzrp Session 52*) |
| **13** | `Reggaeton_95BPM_Abm_Monaco_Cinematic.wav` | Latin Trap | 95 | Ab Minor | Bad Bunny (*Monaco*) |
| **14** | `Reggaeton_94BPM_Em_Classy_101_Keys.wav` | Reggaeton | 94 | E Minor | Feid & Young Miko (*Classy 101*) |
| **15** | `Reggaeton_90BPM_Bbm_Dakiti_Ambient_Pluck.wav` | Reggaeton | 90 | Bb Minor | Bad Bunny & Jhayco (*Dákiti*) |
| **16** | `Reggaeton_96BPM_Gm_Desesperados_Lead.wav` | Reggaeton | 96 | G Minor | Rauw Alejandro (*Desesperados*) |
| **17** | `Drill_142BPM_Em_Dior_Eerie_Bells.wav` | Drill | 142 | E Minor | Pop Smoke (*Dior*) |
| **18** | `Drill_140BPM_Cm_Doja_Whistle_Drill.wav` | Drill | 140 | C Minor | Central Cee (*Doja*) |
| **19** | `Drill_144BPM_Am_Big_Drip_Tritone_Piano.wav` | Drill | 144 | A Minor | Fivio Foreign (*Big Drip*) |
| **20** | `Drill_141BPM_Fm_Body_Sliding_Pluck.wav` | Drill | 141 | F Minor | Russ Millions & Tion Wayne (*Body*) |
| **21** | `Drill_143BPM_Dm_Pelele_Spanish_Drill.wav` | Drill | 143 | D Minor | Morad (*Pelele*) |
| **22** | `Drill_140BPM_Gm_Welcome_Party_Choir.wav` | Drill | 140 | G Minor | Pop Smoke (*Welcome to the Party*) |
| **23** | `HipHop_100BPM_Am_Not_Like_Us_West_Coast.wav` | West Coast | 100 | A Minor | Kendrick Lamar (*Not Like Us*) |
| **24** | `HipHop_94BPM_Cm_Still_Dre_Piano_Staccato.wav` | Hip-Hop | 94 | C Minor | Dr. Dre & Snoop Dogg (*Still D.R.E.*) |
| **25** | `HipHop_88BPM_Fm_No_Role_Modelz_Jazzy.wav` | Boombap | 88 | F Minor | J. Cole (*No Role Modelz*) |
| **26** | `HipHop_92BPM_Dm_Praise_The_Lord_Pan_Flute.wav` | Hip-Hop | 92 | D Minor | A$AP Rocky & Skepta (*Praise The Lord*) |
| **27** | `HipHop_85BPM_Em_Self_Care_Neo_Soul.wav` | Neo-Soul | 85 | E Minor | Mac Miller (*Self Care*) |
| **28** | `HipHop_89BPM_Abm_Alright_Chords.wav` | Hip-Hop | 89 | Ab Minor | Kendrick Lamar (*Alright*) |
| **29** | `Afro_Urban_100BPM_Am_Calm_Down_Afro_Guitar.wav` | Afrobeats | 100 | A Minor | Rema (*Calm Down*) |
| **30** | `Afro_Urban_104BPM_Fm_Last_Last_Soul_Brass.wav` | Afro-Fusion | 104 | F Minor | Burna Boy (*Last Last*) |
| **31** | `RnB_120BPM_Dm_Blinding_Lights_Vintage_Synth.wav` | Synth R&B | 120 | D Minor | The Weeknd (*Blinding Lights*) |
| **32** | `RnB_86BPM_Cm_Kill_Bill_Lofi_Guitar.wav` | Lo-Fi R&B | 86 | C Minor | SZA (*Kill Bill*) |

---

## 🥁 Catálogo de los 62 Drum Loops de Género

Organizados en 5 subcarpetas con versiones **Full Beat** y **Top Loop** (sin kick ni bajo para mezclar):

### 1. Trap Loops (12 Loops)
* `Trap_Loop_140BPM_Astroworld_Sicko_Bounce_Full.wav` / `_Top_Loop.wav` (140 BPM)
* `Trap_Loop_138BPM_Metro_Superhero_Hard_Full.wav` / `_Top_Loop.wav` (138 BPM)
* `Trap_Loop_132BPM_Baby_Gunna_Drip_Bounce_Full.wav` / `_Top_Loop.wav` (132 BPM)
* `Trap_Loop_144BPM_Juice_Lucid_Trap_Full.wav` / `_Top_Loop.wav` (144 BPM)
* `Trap_Loop_136BPM_Drake_21_Rich_Flex_Full.wav` / `_Top_Loop.wav` (136 BPM)
* `Trap_Loop_130BPM_Future_Mask_Off_Rolls_Full.wav` / `_Top_Loop.wav` (130 BPM)

### 2. Reggaeton Loops (12 Loops)
* `Reggaeton_Loop_96BPM_BadBunny_Titi_Dembow_Full.wav` / `_Top.wav` (96 BPM)
* `Reggaeton_Loop_92BPM_Feid_Ferxxo_Crisp_Full.wav` / `_Top.wav` (92 BPM)
* `Reggaeton_Loop_98BPM_Rauw_TodoDeTi_Bounce_Full.wav` / `_Top.wav` (98 BPM)
* `Reggaeton_Loop_95BPM_Monaco_Heavy_Dembow_Full.wav` / `_Top.wav` (95 BPM)
* `Reggaeton_Loop_90BPM_Dakiti_Smooth_Groove_Full.wav` / `_Top.wav` (90 BPM)
* `Reggaeton_Loop_102BPM_Gasolina_Classic_Perreo_Full.wav` / `_Top.wav` (102 BPM)

### 3. Drill Loops (12 Loops)
* `Drill_Loop_142BPM_PopSmoke_Dior_Slide_Full.wav` / `_Top_Loop.wav` (142 BPM)
* `Drill_Loop_140BPM_CentralCee_Doja_Bounce_Full.wav` / `_Top_Loop.wav` (140 BPM)
* `Drill_Loop_144BPM_Fivio_BigDrip_Aggressive_Full.wav` / `_Top_Loop.wav` (144 BPM)
* `Drill_Loop_141BPM_Russ_Body_UK_Full.wav` / `_Top_Loop.wav` (141 BPM)
* `Drill_Loop_143BPM_Morad_Spanish_Drill_Full.wav` / `_Top_Loop.wav` (143 BPM)
* `Drill_Loop_146BPM_NY_Fast_Drill_Stomp_Full.wav` / `_Top_Loop.wav` (146 BPM)

### 4. Hip-Hop & BoomBap Loops (12 Loops)
* `HipHop_Loop_100BPM_Kendrick_NotLikeUs_WestCoast_Full.wav` / `_Top_Loop.wav` (100 BPM)
* `HipHop_Loop_94BPM_Dre_StillDRE_Classic_Full.wav` / `_Top_Loop.wav` (94 BPM)
* `HipHop_Loop_88BPM_Jole_NoRoleModelz_Vinyl_Full.wav` / `_Top_Loop.wav` (88 BPM)
* `HipHop_Loop_92BPM_Asap_PraiseTheLord_Bounce_Full.wav` / `_Top_Loop.wav` (92 BPM)
* `HipHop_Loop_85BPM_MacMiller_SelfCare_Laidback_Full.wav` / `_Top_Loop.wav` (85 BPM)
* `HipHop_Loop_90BPM_BoomBap_90s_GoldenEra_Full.wav` / `_Top_Loop.wav` (90 BPM)

### 5. Afro-Urban & R&B Loops (14 Loops)
* `Afro_RnB_Loop_100BPM_Rema_CalmDown_Afrobeats_Full.wav` / `_Top_Loop.wav` (100 BPM)
* `Afro_RnB_Loop_104BPM_Burna_LastLast_Groove_Full.wav` / `_Top_Loop.wav` (104 BPM)
* `Afro_RnB_Loop_108BPM_Wizkid_Essence_Warm_Full.wav` / `_Top_Loop.wav` (108 BPM)
* `Afro_RnB_Loop_112BPM_Amapiano_LogDrum_Bounce_Full.wav` / `_Top_Shakers.wav` (112 BPM)
* `Afro_RnB_Loop_120BPM_Weeknd_BlindingLights_80s_Full.wav` / `_Top_Loop.wav` (120 BPM)
* `Afro_RnB_Loop_86BPM_SZA_KillBill_Smooth_Full.wav` / `_Top_Loop.wav` (86 BPM)
* `Afro_RnB_Loop_95BPM_Modern_TrapSoul_Chill_Full.wav` / `_Top_Loop.wav` (95 BPM)

---

## 🚀 Instalación en 1 Clic

1. Descarga el paquete ZIP de la versión v2.0.0 desde **[Releases](../../releases)**.
2. Descomprímelo en tu ordenador.
3. Haz doble clic en **`install.bat`** (copia automáticamente las carpetas a `Browser ➔ Packs ➔ Supreme Producer Drum Kit` de FL Studio).
4. ¡Listo! Abre FL Studio y tendrás acceso inmediato a las 32 melodías, los 62 drum loops, los one-shots y los MIDIs.

---

## 📄 Licencia

Este proyecto está bajo la licencia [MIT (100% Libre de Regalías)](LICENSE) para cualquier producción musical comercial o personal.
"""
with open(os.path.join(KIT_DIR, "README.md"), "w", encoding="utf-8") as f:
    f.write(readme_text)

print("[+] README.md, install.bat y install.ps1 actualizados.")
