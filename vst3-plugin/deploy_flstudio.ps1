# Deployment script for AUTOMASTER SUPREME 3.3 VST3
$ErrorActionPreference = "Stop"

$source = "c:\Users\alfaswz\.gemini\antigravity-ide\scratch\agentes-personalizados\vst3-plugin\build\AutomasterSupreme_artefacts\Release\VST3\AUTOMASTER SUPREME 3.3.vst3"
$targetDir = "C:\Program Files\Common Files\VST3"
$target = Join-Path $targetDir "AUTOMASTER SUPREME 3.3.vst3"
$oldTarget = Join-Path $targetDir "AUTOMASTER SUPREME 3.3.vst3.old"

Write-Host "Source: $source"
Write-Host "Target: $target"

if (-not (Test-Path $source)) {
    Write-Error "Source VST3 not found at $source"
    exit 1
}

if (-not (Test-Path $targetDir)) {
    New-Item -ItemType Directory -Path $targetDir -Force | Out-Null
}

# Clean old backup if exists
if (Test-Path $oldTarget) {
    Remove-Item -Path $oldTarget -Recurse -Force -ErrorAction SilentlyContinue
}

# If target exists, rename it (works even if loaded by FL Studio)
if (Test-Path $target) {
    Write-Host "Moving existing VST3 to backup..."
    try {
        Remove-Item -Path $target -Recurse -Force -ErrorAction Stop
    } catch {
        Write-Host "File locked by DAW, renaming to .old..."
        Move-Item -Path $target -Destination $oldTarget -Force
    }
}

Write-Host "Copying fresh build to $targetDir..."
Copy-Item -Path $source -Destination $targetDir -Recurse -Force

if (Test-Path $target) {
    Write-Host "SUCCESS: AUTOMASTER SUPREME 3.3 VST3 installed to $target"
    Get-ChildItem -Path $target -Recurse | Select-Object FullName, Length
} else {
    Write-Error "Failed to copy VST3 bundle"
}
