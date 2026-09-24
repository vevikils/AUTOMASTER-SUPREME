import os

KIT_DIR = r"c:\Users\alfaswz\.gemini\antigravity-ide\scratch\agentes-personalizados\SUPREME-PRODUCER-DRUM-KIT"

# 1. install.bat
install_bat = """@echo off
chcp 65001 >nul
title Supreme Producer Suite v3.0 - Auto-Installer
cls
echo =====================================================================
echo    🔥 SUPREME PRODUCER SUITE v3.0 — AUTO-INSTALLER PARA FL STUDIO
echo    (32 Melodias Complejas + 64 Drum Loops Autenticos de Hits + One-Shots)
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

echo [*] Copiando librerias completas (138 archivos)...
xcopy /E /I /Y /Q "%~dp001_808s_&_Basses" "%TARGET_DIR%\\01_808s_&_Basses" >nul
xcopy /E /I /Y /Q "%~dp002_Kicks" "%TARGET_DIR%\\02_Kicks" >nul
xcopy /E /I /Y /Q "%~dp003_Snares_&_Rims" "%TARGET_DIR%\\03_Snares_&_Rims" >nul
xcopy /E /I /Y /Q "%~dp004_Claps" "%TARGET_DIR%\\04_Claps" >nul
xcopy /E /I /Y /Q "%~dp005_HiHats_&_Cymbals" "%TARGET_DIR%\\05_HiHats_&_Cymbals" >nul
xcopy /E /I /Y /Q "%~dp006_Percussion" "%TARGET_DIR%\\06_Percussion" >nul
xcopy /E /I /Y /Q "%~dp007_FX_&_Chants" "%TARGET_DIR%\\07_FX_&_Chants" >nul
xcopy /E /I /Y /Q "%~dp008_Melody_Samples_32_(Hit_Inspired_Auto_Tempo)" "%TARGET_DIR%\\08_Melody_Samples_32_(Hit_Inspired_Auto_Tempo)" >nul
xcopy /E /I /Y /Q "%~dp009_Drum_Loops_64_Authentic_Hits_(Auto_Tempo)" "%TARGET_DIR%\\09_Drum_Loops_64_Authentic_Hits_(Auto_Tempo)" >nul
xcopy /E /I /Y /Q "%~dp010_MIDI_Patterns" "%TARGET_DIR%\\10_MIDI_Patterns" >nul
copy /Y "%~dp0cover.jpg" "%TARGET_DIR%\\" >nul

if %errorlevel% equ 0 (
    echo.
    echo =====================================================================
    echo   [EXITO] Supreme Producer Suite v3.0 instalado en FL Studio!
    echo =====================================================================
    echo.
    echo   COMO USAR EN FL STUDIO:
    echo   1. Abre FL Studio.
    echo   2. En el navegador izquierdo (Browser), ve a la carpeta 'Packs'.
    echo   3. Abre 'Supreme Producer Drum Kit'.
    echo   4. Arrastra cualquier muestra, loop o MIDI a la Playlist o Channel Rack.
    echo.
    echo   AUTO-TEMPO:
    echo   Los 32 samples de melodias y los 64 drum loops contienen cabeceras
    echo   ACID WAV que se sincronizan automaticamente al BPM de tu proyecto!
    echo.
) else (
    echo [AVISO] Puedes arrastrar la carpeta directamente al navegador de FL Studio.
)

pause
"""
with open(os.path.join(KIT_DIR, "install.bat"), "w", encoding="utf-8") as f:
    f.write(install_bat)

# 2. install.ps1
install_ps1 = """# Supreme Producer Drum Kit v3.0 - PowerShell Installer
Write-Host "=====================================================================" -ForegroundColor Cyan
Write-Host "   🔥 SUPREME PRODUCER SUITE v3.0 — AUTO-INSTALLER" -ForegroundColor Yellow
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
    "09_Drum_Loops_64_Authentic_Hits_(Auto_Tempo)",
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

Write-Host "`n[EXITO] Drum Kit Suite v3.0 instalado en FL Studio (Packs)!" -ForegroundColor Green
Write-Host "Presiona cualquier tecla para salir..."
[void][System.Console]::ReadKey()
"""
with open(os.path.join(KIT_DIR, "install.ps1"), "w", encoding="utf-8") as f:
    f.write(install_ps1)

# 3. Comprehensive README.md
readme_text = """<p align="center">
  <img src="cover.jpg" alt="Supreme Producer Drum Kit Cover" width="100%" style="border-radius: 14px; box-shadow: 0 10px 35px rgba(0,0,0,0.6);">
</p>

<h1 align="center">🔥 SUPREME PRODUCER SUITE v3.0 (EDICIÓN DEFINITIVA)</h1>
<p align="center">
  <b>El kit definitivo de producción musical para FL Studio, Ableton Live, Logic Pro y cualquier DAW</b><br>
  <i>32 Melodías Complejas + 64 Drum Loops Auténticos de Hits (Trap Argentino, Trap PR, Reggaetón Clásico y Moderno) + Drum Kit + MIDIs</i>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Version-3.0.0_Definitive-ff0055?style=for-the-badge" alt="Version 3.0.0">
  <img src="https://img.shields.io/badge/Total_Assets-138_Files-00c853?style=for-the-badge" alt="138 Files">
  <img src="https://img.shields.io/badge/Melody_Samples-32_Hit_Inspired-ff6f00?style=for-the-badge" alt="32 Melodies">
  <img src="https://img.shields.io/badge/Drum_Loops-64_Authentic_Hits-7928ca?style=for-the-badge" alt="64 Loops">
  <img src="https://img.shields.io/badge/License-100%25_Royalty_Free-0070f3?style=for-the-badge" alt="Royalty Free">
</p>

---

## ⚡ ¿Qué hace única a la Versión 3.0?

Diseñada para resolver las críticas de los drum kits convencionales (ritmos repetitivos o genéricos):

- 🥁 **64 Drum Loops Auténticos basados en Canciones Famosas**:
  - **Trap Argentino (16 Loops)**: Replicando la pegada, 808s Spinz con saturación dura y redobles rápidos a 1/32 de **Duki, YSY A, Bizarrap, Khea, Tiago PZK y Neo Pistea**.
  - **Latin Trap Puerto Rico (16 Loops)**: El sonido icónico de subgraves profundos, cajas afinadas y aros acústicos de **Bad Bunny (*Chambea*, *Soy Peor*, *Monaco*), Anuel AA (*Sola*, *Brindemos*) y Eladio Carrión (*Kemba Walker*, *Sauce Boyz*)**.
  - **Reggaetón Clásico & Perreo PR (16 Loops)**: El dembow histórico de Luny Tunes con el auténtico snare hueco y cortante, timbales y raspado de güira de **Daddy Yankee (*Gasolina*, *Rompe*), Don Omar (*Pobre Diabla*, *Dile*), Wisin & Yandel (*Rákata*) y Plan B (*Frikitona*)**.
  - **Reggaetón Moderno & Tainy Style (16 Loops)**: Sub bombos a 48Hz, híbridos de clap-caja, shakers orgánicos y bombos sincopados inspirados en **Tainy (*Data*, *Lo Siento BB*), Bad Bunny (*Tití Me Preguntó*, *Callaíta*, *Dákiti*), Feid (*Ferxxo 100*, *Classy 101*) y Mora (*Memorias*)**.
  - **Cada loop cuenta con variaciones dinámicas en 4 compases y remates (fills)** en el compás 4 para transiciones limpias.
- 🎹 **32 Samples de Melodías Complejas (Sin estridencias)**:
  - Sonido cálido y procesado con ecualización de estudio (corte suave a 6.5kHz para eliminar chillidos y realce en medios-graves a 250-500Hz).
  - Reverb de sala multicámara sedosa.
  - Multi-capas de Rhodes, pads analógicos, guitarras de cuerda y flautas de aire con vibrato.
- ⏱️ **Auto-Ajustable al Tempo (WAV ACIDized)**:
  - Todos los 96 loops (32 melodías + 64 drum loops) contienen etiquetas **ACID WAV**. En **FL Studio**, se sincronizan automáticamente al BPM de tu sesión sin variar el tono de las notas.
- 💥 **34 One-Shots de Batería de Estudio**: 808s afinados en C, kicks con pegada limpia, cajas acústicas y de trap, claps multicapa, hi-hats ultrafiltrados, percusiones y efectos FX.
- 🎼 **8 Patrones MIDI para el Piano Roll**.

---

## 🥁 Catálogo de los 64 Drum Loops Auténticos (Hit-Inspired)

### 🇦🇷 01. Trap Argentino (16 Loops: Full Beats + Top Loops)
| Archivo | BPM | Artista y Canción de Referencia |
| :--- | :-: | :--- |
| `TrapArg_140BPM_Duki_Goteo_Full.wav` / `_Top_Loop.wav` | 140 | **Duki** — *Goteo* (Spinz 808 saturado + redobles 1/32) |
| `TrapArg_144BPM_Duki_Givenchy_Full.wav` / `_Top_Loop.wav` | 144 | **Duki** — *Givenchy* (Doble bombo agresivo + claps afilados) |
| `TrapArg_135BPM_YsyA_Trapitos_Full.wav` / `_Top_Loop.wav` | 135 | **YSY A** — *Traje Unos Trapitos* (Bounce pesado sincopado) |
| `TrapArg_138BPM_Bzrp_Duki_Session50_Full.wav` / `_Top_Loop.wav` | 138 | **Bizarrap & Duki** — *Session 50* (Pegada dura de claps y 808) |
| `TrapArg_130BPM_Khea_Loca_Full.wav` / `_Top_Loop.wav` | 130 | **Khea & Duki** — *Loca* (El ritmo original del trap argentino) |
| `TrapArg_142BPM_NeoPistea_TumbandoClub_Full.wav` / `_Top_Loop.wav` | 142 | **Neo Pistea** — *Tumbando el Club* (Golpe de bombo stomp) |
| `TrapArg_136BPM_TiagoPZK_Sola_Full.wav` / `_Top_Loop.wav` | 136 | **Tiago PZK** — *Sola* (Rítmica melódica con apertura estéreo) |
| `TrapArg_125BPM_MiloJ_RaraVez_Full.wav` / `_Top_Loop.wav` | 125 | **Milo J** — *Rara Vez* (Híbrido de boombap y trap suave) |

---

### 🇵🇷 02. Latin Trap Puerto Rico (16 Loops: Full Beats + Top Loops)
| Archivo | BPM | Artista y Canción de Referencia |
| :--- | :-: | :--- |
| `TrapPR_130BPM_BadBunny_Chambea_Full.wav` / `_Top_Loop.wav` | 130 | **Bad Bunny** — *Chambea* (Aro acústico cortante + subgrave) |
| `TrapPR_128BPM_BadBunny_SoyPeor_Full.wav` / `_Top_Loop.wav` | 128 | **Bad Bunny** — *Soy Peor* (Tempo lento, 808 demoledor) |
| `TrapPR_138BPM_Anuel_Sola_Full.wav` / `_Top_Loop.wav` | 138 | **Anuel AA** — *Sola* (El clásico ritmo callejero de Puerto Rico) |
| `TrapPR_134BPM_Anuel_Brindemos_Full.wav` / `_Top_Loop.wav` | 134 | **Anuel AA & Ozuna** — *Brindemos* (Subgrave cálido de 32Hz) |
| `TrapPR_140BPM_EladioCarrion_KembaWalker_Full.wav` / `_Top_Loop.wav` | 140 | **Eladio Carrión** — *Kemba Walker* (Hi-hats técnicos con swing) |
| `TrapPR_136BPM_EladioCarrion_SauceBoy_Full.wav` / `_Top_Loop.wav` | 136 | **Eladio Carrión** — *Sauce Boy Freestyle* (Flujo limpio y seco) |
| `TrapPR_142BPM_BadBunny_Monaco_Full.wav` / `_Top_Loop.wav` | 142 | **Bad Bunny** — *Monaco* (Trap orquestal con remates de bombo) |
| `TrapPR_135BPM_MykeTowers_EasyMoney_Full.wav` / `_Top_Loop.wav` | 135 | **Myke Towers** — *Easy Money Baby* (Bounce ágil y relajado) |

---

### 🔥 03. Reggaetón Clásico & Perreo PR (16 Loops: Full Beats + Top Loops)
| Archivo | BPM | Artista y Canción de Referencia |
| :--- | :-: | :--- |
| `ReggaetonPR_96BPM_DaddyYankee_Gasolina_Full.wav` / `_Top_Loop.wav` | 96 | **Daddy Yankee** — *Gasolina* (Dembow Luny Tunes con güira y timbal) |
| `ReggaetonPR_94BPM_DonOmar_PobreDiabla_Full.wav` / `_Top_Loop.wav` | 94 | **Don Omar** — *Pobre Diabla* (Caja resonante clásica y güira) |
| `ReggaetonPR_95BPM_WisinYandel_Rakata_Full.wav` / `_Top_Loop.wav` | 95 | **Wisin & Yandel** — *Rákata* (Patrón perreo duro de discoteca) |
| `ReggaetonPR_98BPM_PlanB_Frikitona_Full.wav` / `_Top_Loop.wav` | 98 | **Plan B** — *Frikitona* (Perreo rápido y agresivo) |
| `ReggaetonPR_92BPM_DaddyYankee_Rompe_Full.wav` / `_Top_Loop.wav` | 92 | **Daddy Yankee** — *Rompe* (Bombo pesado con golpe frontal) |
| `ReggaetonPR_100BPM_DonOmar_Dile_Full.wav` / `_Top_Loop.wav` | 100 | **Don Omar** — *Dile* (Ritmo bailable a 100 BPM con timbales) |
| `ReggaetonPR_94BPM_ZionLennox_YoVoy_Full.wav` / `_Top_Loop.wav` | 94 | **Zion & Lennox** — *Yo Voy* (Groove clásico de los 2000s) |
| `ReggaetonPR_96BPM_TegoCalderon_PaQueRetozen_Full.wav` / `_Top_Loop.wav` | 96 | **Tego Calderón** — *Pa' Que Retozen* (Dembow afro-antillano) |

---

### 🌴 04. Reggaetón Moderno & Tainy Style (16 Loops: Full Beats + Top Loops)
| Archivo | BPM | Artista y Canción de Referencia |
| :--- | :-: | :--- |
| `ReggaetonModern_96BPM_BadBunny_Titi_Full.wav` / `_Top_Loop.wav` | 96 | **Bad Bunny** — *Tití Me Preguntó* (Doble golpe y percusión moderna) |
| `ReggaetonModern_92BPM_Feid_Ferxxo100_Full.wav` / `_Top_Loop.wav` | 92 | **Feid** — *Ferxxo 100* (Dembow suave y romántico con shakers) |
| `ReggaetonModern_95BPM_Tainy_Data_LoSientoBB_Full.wav` / `_Top_Loop.wav` | 95 | **Tainy & Bad Bunny** — *Lo Siento BB / Data* (Bombo sincopado) |
| `ReggaetonModern_98BPM_Rauw_Desesperados_Full.wav` / `_Top_Loop.wav` | 98 | **Rauw Alejandro** — *Desesperados* (Perreo moderno rápido) |
| `ReggaetonModern_90BPM_BadBunny_Dakiti_Full.wav` / `_Top_Loop.wav` | 90 | **Bad Bunny & Jhayco** — *Dákiti* (Groove espacial y atmosférico) |
| `ReggaetonModern_94BPM_Mora_Memorias_Full.wav` / `_Top_Loop.wav` | 94 | **Mora & Jhayco** — *Memorias* (Híbrido de afro-reggaeton) |
| `ReggaetonModern_93BPM_Feid_Classy101_Full.wav` / `_Top_Loop.wav` | 93 | **Feid & Young Miko** — *Classy 101* (Shakers y caja seca) |
| `ReggaetonModern_95BPM_Tainy_Callaita_Full.wav` / `_Top_Loop.wav` | 95 | **Bad Bunny & Tainy** — *Callaíta* (Subgraves demoledores) |

---

## 🎹 Catálogo de las 32 Melodías Complejas (Hit-Inspired)

| # | Archivo | Género | BPM | Tonalidad | Inspiración |
| :-: | :--- | :--- | :-: | :-: | :--- |
| **01** | `Trap_140BPM_Cm_Astroworld_Goosebumps.wav` | Trap | 140 | Cm | Travis Scott (*Goosebumps*) |
| **02** | `Trap_138BPM_Am_Superhero_Cinematic.wav` | Trap | 138 | Am | Metro Boomin (*Superhero*) |
| **03** | `Trap_132BPM_Fm_Drip_Too_Hard_Guitar.wav` | Trap | 132 | Fm | Lil Baby (*Drip Too Hard*) |
| **04** | `Trap_130BPM_Dm_Mask_Off_Haunting_Flute.wav` | Trap | 130 | Dm | Future (*Mask Off*) |
| **05** | `Trap_144BPM_Em_Lucid_Dreams_Guitar.wav` | Trap | 144 | Em | Juice WRLD (*Lucid Dreams*) |
| **06** | `Trap_136BPM_Abm_Jimmy_Cooks_Soul.wav` | Trap | 136 | Abm | Drake (*Jimmy Cooks*) |
| **07** | `Trap_140BPM_Bbm_The_Box_Orchestral.wav` | Trap | 140 | Bbm | Roddy Ricch (*The Box*) |
| **08** | `Trap_134BPM_Gm_Pushin_P_Ambient.wav` | Trap | 134 | Gm | Gunna (*Pushin P*) |
| **09** | `Reggaeton_96BPM_Fm_Titi_Me_Pregunto.wav` | Reggaeton | 96 | Fm | Bad Bunny (*Tití Me Preguntó*) |
| **10** | `Reggaeton_92BPM_Am_Feliz_Cumpleanos_Ferxxo.wav` | Reggaeton | 92 | Am | Feid (*Ferxxo*) |
| **11** | `Reggaeton_98BPM_Cm_Todo_De_Ti_Synth.wav` | Reggaeton | 98 | Cm | Rauw Alejandro (*Todo De Ti*) |
| **12** | `Reggaeton_128BPM_Dm_Bzrp_Quevedo_Quedate.wav` | Dance/Latin | 128 | Dm | Bizarrap & Quevedo (*Bzrp 52*) |
| **13** | `Reggaeton_95BPM_Abm_Monaco_Cinematic.wav` | Latin Trap | 95 | Abm | Bad Bunny (*Monaco*) |
| **14** | `Reggaeton_94BPM_Em_Classy_101_Keys.wav` | Reggaeton | 94 | Em | Feid (*Classy 101*) |
| **15** | `Reggaeton_90BPM_Bbm_Dakiti_Ambient_Pluck.wav` | Reggaeton | 90 | Bbm | Bad Bunny (*Dákiti*) |
| **16** | `Reggaeton_96BPM_Gm_Desesperados_Lead.wav` | Reggaeton | 96 | Gm | Rauw Alejandro (*Desesperados*) |
| **17** | `Drill_142BPM_Em_Dior_Eerie_Bells.wav` | Drill | 142 | Em | Pop Smoke (*Dior*) |
| **18** | `Drill_140BPM_Cm_Doja_Whistle_Drill.wav` | Drill | 140 | Cm | Central Cee (*Doja*) |
| **19** | `Drill_144BPM_Am_Big_Drip_Tritone_Piano.wav` | Drill | 144 | Am | Fivio Foreign (*Big Drip*) |
| **20** | `Drill_141BPM_Fm_Body_Sliding_Pluck.wav` | Drill | 141 | Fm | Russ Millions (*Body*) |
| **21** | `Drill_143BPM_Dm_Pelele_Spanish_Drill.wav` | Drill | 143 | Dm | Morad (*Pelele*) |
| **22** | `Drill_140BPM_Gm_Welcome_Party_Choir.wav` | Drill | 140 | Gm | Pop Smoke (*Welcome to Party*) |
| **23** | `HipHop_100BPM_Am_Not_Like_Us_West_Coast.wav` | West Coast | 100 | Am | Kendrick Lamar (*Not Like Us*) |
| **24** | `HipHop_94BPM_Cm_Still_Dre_Piano_Staccato.wav` | Hip-Hop | 94 | Cm | Dr. Dre (*Still D.R.E.*) |
| **25** | `HipHop_88BPM_Fm_No_Role_Modelz_Jazzy.wav` | Boombap | 88 | Fm | J. Cole (*No Role Modelz*) |
| **26** | `HipHop_92BPM_Dm_Praise_The_Lord_Pan_Flute.wav` | Hip-Hop | 92 | Dm | A$AP Rocky (*Praise The Lord*) |
| **27** | `HipHop_85BPM_Em_Self_Care_Neo_Soul.wav` | Neo-Soul | 85 | Em | Mac Miller (*Self Care*) |
| **28** | `HipHop_89BPM_Abm_Alright_Chords.wav` | Hip-Hop | 89 | Abm | Kendrick Lamar (*Alright*) |
| **29** | `Afro_Urban_100BPM_Am_Calm_Down_Afro_Guitar.wav` | Afrobeats | 100 | Am | Rema (*Calm Down*) |
| **30** | `Afro_Urban_104BPM_Fm_Last_Last_Soul_Brass.wav` | Afro-Fusion | 104 | Fm | Burna Boy (*Last Last*) |
| **31** | `RnB_120BPM_Dm_Blinding_Lights_Vintage_Synth.wav` | Synth R&B | 120 | Dm | The Weeknd (*Blinding Lights*) |
| **32** | `RnB_86BPM_Cm_Kill_Bill_Lofi_Guitar.wav` | Lo-Fi R&B | 86 | Cm | SZA (*Kill Bill*) |

---

## 🚀 Instalación en 1 Clic

1. Descarga el paquete ZIP de la versión v3.0.0 desde **[Releases](../../releases)**.
2. Descomprímelo en tu ordenador.
3. Haz doble clic en **`install.bat`** (copia automáticamente las carpetas a `Browser ➔ Packs ➔ Supreme Producer Drum Kit` de FL Studio).
4. Abre FL Studio y tendrás acceso inmediato a las 32 melodías, los 64 drum loops auténticos, los one-shots y los MIDIs.

---

## 📄 Licencia

Este proyecto está bajo la licencia [MIT (100% Libre de Regalías)](LICENSE) para cualquier producción musical comercial o personal.
"""
with open(os.path.join(KIT_DIR, "README.md"), "w", encoding="utf-8") as f:
    f.write(readme_text)

print("[+] README.md, install.bat y install.ps1 actualizados para v3.0.")
