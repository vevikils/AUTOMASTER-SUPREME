import os

ps1_content = """# Script de instalacion automatica de Santa Maria del Naranco para Cities: Skylines II

$ErrorActionPreference = "Stop"

$cs2DataPath = "$env:USERPROFILE\\AppData\\LocalLow\\Colossal Order\\Cities Skylines II"
$eaiPath = "$cs2DataPath\\ModsData\\ExtraAssetsImporter"
$customAssetsPath = "$cs2DataPath\\CustomAssets\\SantaMariaDelNaranco"
$modsPath = "$cs2DataPath\\Mods\\SantaMariaDelNaranco"

Write-Host "==========================================================" -ForegroundColor Cyan
Write-Host " Instalador del Asset: Santa Maria del Naranco (CS2)" -ForegroundColor Yellow
Write-Host " Arte Prerromanico Asturiano - Patrimonio de la Humanidad" -ForegroundColor White
Write-Host "==========================================================" -ForegroundColor Cyan

if (-not (Test-Path $cs2DataPath)) {
    Write-Warning "No se ha encontrado la carpeta de datos de Cities: Skylines II en $cs2DataPath"
    exit 1
}

# Crear destinos
$targets = @($customAssetsPath, $modsPath)
if (Test-Path $eaiPath) {
    $targets += "$eaiPath\\CustomBuildings\\SantaMariaDelNaranco"
}

foreach ($t in $targets) {
    New-Item -ItemType Directory -Force -Path $t | Out-Null
    Write-Host "[+] Desplegando en: $t" -ForegroundColor Green

    # Copiar Modelos FBX (LOD0, LOD1, LOD2 y Collider)
    Copy-Item "$PSScriptRoot\\..\\04_Export_FBX\\Santa_Maria_Naranco_*.fbx" -Destination $t -Force

    # Copiar Texturas PBR (BaseColor, Normal, Mask, Emissive)
    Copy-Item "$PSScriptRoot\\..\\03_Textures_PBR\\Santa_Maria_Naranco_*.png" -Destination $t -Force
    Copy-Item "$PSScriptRoot\\..\\03_Textures_PBR\\Santa_Maria_Naranco_Mask.png" -Destination "$t\\Santa_Maria_Naranco_MaskMap.png" -Force

    # Copiar Configuracion del Prefab JSON e Iconos
    Copy-Item "$PSScriptRoot\\SantaMariaDelNaranco.json" -Destination $t -Force
    Copy-Item "$PSScriptRoot\\Santa_Maria_Naranco.png" -Destination $t -Force
    Copy-Item "$PSScriptRoot\\thumbnail_naranco.png" -Destination "$t\\thumbnail.png" -Force
}

Write-Host "`n[OK] ¡Santa Maria del Naranco se ha instalado correctamente en Cities: Skylines II!" -ForegroundColor Green
Write-Host "Para colocarlo en tu ciudad:" -ForegroundColor Yellow
Write-Host "1. Abre Cities: Skylines II."
Write-Host "2. En tu menu de Monumentos / Edificios Singulares / Parques / Extra Assets, busca 'Santa Maria del Naranco'."
Write-Host "3. ¡Disfruta de las arquerias, los miradores y la iluminacion calida nocturna!" -ForegroundColor White
"""

target_file = r"C:\Users\alfaswz\Desktop\MOODS CITYES SKYLINES\05_Cities_Skylines_Asset\install_naranco.ps1"
with open(target_file, "w", encoding="utf-8") as f:
    f.write(ps1_content)
print(f"Written {target_file}")
