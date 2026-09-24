import os
import shutil
import zipfile
import subprocess

REPO_DIR = r"c:\Users\alfaswz\.gemini\antigravity-ide\scratch\agentes-personalizados\SUPREME-VOCAL-CHAINS-FL-STUDIO"
DESKTOP = r"C:\Users\alfaswz\Desktop"
COVER_SRC = r"C:\Users\alfaswz\.gemini\antigravity-ide\brain\6a240be5-03c7-4d57-9466-576318416996\vocal_pack_cover_1790098006716.jpg"

PRESET_FILES = [
    "TRAVIS SCOTT - VOCAL CHAIN (PRO).fst",
    "DUKI - SUPER ROBOTIC TRAP (PRO).fst",
    "ANUEL AA - MELODIC TRAP (PRO).fst",
    "BAD BUNNY - PRIME ERA (PRO).fst",
    "OZUNA - VOZ AGUDA (PRO).fst"
]

# Ensure directories
os.makedirs(os.path.join(REPO_DIR, "assets"), exist_ok=True)
os.makedirs(os.path.join(REPO_DIR, "Presets"), exist_ok=True)

# Copy cover
cover_dst = os.path.join(REPO_DIR, "assets", "cover.jpg")
shutil.copy2(COVER_SRC, cover_dst)
# Also copy cover as cover.jpg in root for instant visibility
shutil.copy2(COVER_SRC, os.path.join(REPO_DIR, "cover.jpg"))
print("Cover copied successfully.")

# Copy presets
for p in PRESET_FILES:
    src = os.path.join(DESKTOP, p)
    dst_presets = os.path.join(REPO_DIR, "Presets", p)
    shutil.copy2(src, dst_presets)
    # also in root Presets/
    print(f"Copied {p} -> {dst_presets}")

# Create install.bat
install_bat_content = """@echo off
chcp 65001 >nul
title Supreme Vocal Chains - FL Studio Auto-Installer
cls
echo =====================================================================
echo       🎙️ SUPREME VOCAL CHAINS — FL STUDIO AUTO-INSTALLER
echo       Designed for Travis Scott, Duki, Anuel AA, Bad Bunny & Ozuna
echo =====================================================================
echo.

set "TARGET_DIR=%USERPROFILE%\\Documents\\Image-Line\\FL Studio\\Presets\\Mixer presets\\Supreme Vocal Chains"

:: Check for OneDrive Documents fallback
if not exist "%USERPROFILE%\\Documents\\Image-Line" (
    if exist "%USERPROFILE%\\OneDrive\\Documents\\Image-Line" (
        set "TARGET_DIR=%USERPROFILE%\\OneDrive\\Documents\\Image-Line\\FL Studio\\Presets\\Mixer presets\\Supreme Vocal Chains"
    )
)

echo [*] Target Directory: "%TARGET_DIR%"
echo.

if not exist "%TARGET_DIR%" (
    echo [*] Creating target preset directory...
    mkdir "%TARGET_DIR%"
)

echo [*] Installing Supreme Vocal Chains...
copy /Y "%~dp0Presets\\*.fst" "%TARGET_DIR%\\" >nul

if %errorlevel% equ 0 (
    echo.
    echo =====================================================================
    echo   [SUCCESS] All 5 Vocal Presets successfully installed into FL Studio!
    echo =====================================================================
    echo.
    echo   HOW TO USE IN FL STUDIO:
    echo   1. Open FL Studio and open your Mixer (F9).
    echo   2. Right-click any Mixer Track (e.g., Track 1 or Vocal Track).
    echo   3. Navigate to: File -^> Open mixer track state...
    echo   4. Select 'Supreme Vocal Chains' and choose your artist preset!
    echo.
    echo   ALTERNATIVE:
    echo   Drag and drop any .fst file directly from the 'Presets' folder
    echo   straight onto your FL Studio mixer track.
    echo.
) else (
    echo.
    echo [ERROR] Could not automatically copy presets.
    echo Please manually copy the contents of the 'Presets' folder to:
    echo "%USERPROFILE%\\Documents\\Image-Line\\FL Studio\\Presets\\Mixer presets\\Supreme Vocal Chains\\"
    echo.
)

pause
"""

with open(os.path.join(REPO_DIR, "install.bat"), "w", encoding="utf-8") as f:
    f.write(install_bat_content)

# Create install.ps1
install_ps1_content = """# Supreme Vocal Chains - PowerShell Installer
Write-Host "=====================================================================" -ForegroundColor Cyan
Write-Host "      🎙️ SUPREME VOCAL CHAINS — FL STUDIO AUTO-INSTALLER" -ForegroundColor Yellow
Write-Host "=====================================================================" -ForegroundColor Cyan

$docs = [Environment]::GetFolderPath('MyDocuments')
$targetDir = Join-Path $docs "Image-Line\\FL Studio\\Presets\\Mixer presets\\Supreme Vocal Chains"

if (-not (Test-Path -Path (Split-Path $targetDir -Parent))) {
    $oneDriveDocs = Join-Path $env:USERPROFILE "OneDrive\\Documents"
    $oneDriveTarget = Join-Path $oneDriveDocs "Image-Line\\FL Studio\\Presets\\Mixer presets\\Supreme Vocal Chains"
    if (Test-Path -Path (Split-Path $oneDriveTarget -Parent)) {
        $targetDir = $oneDriveTarget
    }
}

Write-Host "[*] Target Directory: $targetDir" -ForegroundColor Gray

if (-not (Test-Path -Path $targetDir)) {
    New-Item -ItemType Directory -Path $targetDir -Force | Out-Null
    Write-Host "[*] Created directory: $targetDir" -ForegroundColor Gray
}

$presetSource = Join-Path $PSScriptRoot "Presets\\*.fst"
Copy-Item -Path $presetSource -Destination $targetDir -Force

Write-Host "`n[SUCCESS] All 5 Vocal Presets successfully installed!" -ForegroundColor Green
Write-Host "`nHOW TO USE IN FL STUDIO:" -ForegroundColor Yellow
Write-Host "1. Press F9 in FL Studio to open the Mixer."
Write-Host "2. Right-click any Mixer track -> File -> Open mixer track state..."
Write-Host "3. Open 'Supreme Vocal Chains' and select your preset!"
Write-Host "`nPress any key to exit..."
[void][System.Console]::ReadKey()
"""

with open(os.path.join(REPO_DIR, "install.ps1"), "w", encoding="utf-8") as f:
    f.write(install_ps1_content)

# Create LICENSE
license_content = """MIT License

Copyright (c) 2026 vevikils

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
"""

with open(os.path.join(REPO_DIR, "LICENSE"), "w", encoding="utf-8") as f:
    f.write(license_content)

print("Installer scripts and license created successfully.")
