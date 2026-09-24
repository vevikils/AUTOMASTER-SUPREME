content = """# Instalador unificado de Mods de Oviedo para Cities: Skylines II
# Pack de Oviedo: Catedral de San Salvador, Ayuntamiento, Estadio Carlos Tartiere y Santa Maria del Naranco

$ErrorActionPreference = "Stop"

Write-Host "==========================================================" -ForegroundColor Cyan
Write-Host " Instalador del Pack de Oviedo para Cities: Skylines II" -ForegroundColor Yellow
Write-Host " Autor: vevikils" -ForegroundColor White
Write-Host "==========================================================" -ForegroundColor Cyan

Write-Host "[1/4] Instalando Catedral de Oviedo..." -ForegroundColor White
& "$PSScriptRoot\\install_to_game.ps1"

Write-Host "`n[2/4] Instalando Ayuntamiento de Oviedo..." -ForegroundColor White
& "$PSScriptRoot\\install_ayuntamiento.ps1"

Write-Host "`n[3/4] Instalando Estadio Municipal Carlos Tartiere..." -ForegroundColor White
& "$PSScriptRoot\\install_tartiere.ps1"

Write-Host "`n[4/4] Instalando Santa Maria del Naranco..." -ForegroundColor White
& "$PSScriptRoot\\install_naranco.ps1"

Write-Host "`n==========================================================" -ForegroundColor Green
Write-Host " [TODO INSTALADO] Pack de Oviedo completo listo para jugar en CS2!" -ForegroundColor Yellow
Write-Host "==========================================================" -ForegroundColor Green
"""

target = r"C:\Users\alfaswz\Desktop\MOODS CITYES SKYLINES\05_Cities_Skylines_Asset\install_all_oviedo_mods.ps1"
with open(target, "w", encoding="utf-8") as f:
    f.write(content)
print(f"Updated {target}")
