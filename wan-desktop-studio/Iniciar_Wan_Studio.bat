@echo off
title Wan 2.1 Video Studio - RTX 4060 Edition
cd /d "%~dp0"

echo ========================================================
echo       WAN 2.1 VIDEO STUDIO - RTX 4060 EDITION
echo ========================================================
echo.

if not exist "wan_env\Scripts\python.exe" (
    echo [!] No se encontro el entorno virtual wan_env.
    echo     Ejecuta setup_environment.bat para configurarlo.
    pause
    exit /b 1
)

echo [*] Iniciando aplicacion de escritorio Wan 2.1...
echo [*] Optimizando para NVIDIA GeForce RTX 4060...
echo.

wan_env\Scripts\python.exe main_desktop.py

if %ERRORLEVEL% neq 0 (
    echo.
    echo [!] La aplicacion se ha cerrado con error.
    pause
)
