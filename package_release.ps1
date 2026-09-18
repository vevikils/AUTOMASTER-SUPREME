$ErrorActionPreference = "Stop"

$workspace = "c:\Users\alfaswz\.gemini\antigravity-ide\scratch\agentes-personalizados"
$pkgDir = Join-Path $workspace "Release_Package"
$zipPath = Join-Path $workspace "AUTOMASTER_SUPREME_v3.2.0_Windows.zip"

if (Test-Path $pkgDir) { Remove-Item -Path $pkgDir -Recurse -Force }
if (Test-Path $zipPath) { Remove-Item -Path $zipPath -Force }

New-Item -ItemType Directory -Path $pkgDir -Force | Out-Null
New-Item -ItemType Directory -Path (Join-Path $pkgDir "VST3") -Force | Out-Null
New-Item -ItemType Directory -Path (Join-Path $pkgDir "Standalone") -Force | Out-Null

Copy-Item -Path "$workspace\vst3-plugin\build\AutomasterSupreme_artefacts\Release\VST3\AUTOMASTER SUPREME 3.2.vst3" -Destination (Join-Path $pkgDir "VST3") -Recurse -Force
Copy-Item -Path "$workspace\vst3-plugin\build\AutomasterSupreme_artefacts\Release\Standalone\AUTOMASTER SUPREME 3.2.exe" -Destination (Join-Path $pkgDir "Standalone") -Force
Copy-Item -Path "$workspace\vst3-plugin\GUIA_FL_STUDIO.md" -Destination $pkgDir -Force
Copy-Item -Path "$workspace\MANUAL_DE_USUARIO.md" -Destination $pkgDir -Force
Copy-Item -Path "$workspace\README.md" -Destination $pkgDir -Force
Copy-Item -Path "$workspace\LICENSE" -Destination $pkgDir -Force

Compress-Archive -Path "$pkgDir\*" -DestinationPath $zipPath -Force
Remove-Item -Path $pkgDir -Recurse -Force

Write-Host "ZIP created successfully:"
Get-Item $zipPath | Select-Object Name, Length
