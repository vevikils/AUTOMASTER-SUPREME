# Deployment script for AUTOMASTER SUPREME 2 VST3
$ErrorActionPreference = "Stop"

$source = "c:\Users\alfaswz\.gemini\antigravity-ide\scratch\agentes-personalizados\vst3-plugin\build\AutomasterSupreme_artefacts\Release\VST3\AUTOMASTER SUPREME 2.vst3"
$targetDir = "C:\Program Files\Common Files\VST3"
$target = Join-Path $targetDir "AUTOMASTER SUPREME 2.vst3"

Write-Host "Source: $source"
Write-Host "Target: $target"

if (-not (Test-Path $source)) {
    Write-Error "Source VST3 not found at $source"
    exit 1
}

if (-not (Test-Path $targetDir)) {
    New-Item -ItemType Directory -Path $targetDir -Force | Out-Null
}

# If target exists, try to remove or replace
if (Test-Path $target) {
    Write-Host "Removing existing VST3 at $target..."
    Remove-Item -Path $target -Recurse -Force -ErrorAction SilentlyContinue
}

Write-Host "Copying fresh build to $target..."
Copy-Item -Path $source -Destination $target -Recurse -Force

if (Test-Path $target) {
    Write-Host "SUCCESS: AUTOMASTER SUPREME VST3 installed to $target"
    Get-ChildItem -Path $target -Recurse | Select-Object FullName, Length
} else {
    Write-Error "Failed to copy VST3 bundle"
}
