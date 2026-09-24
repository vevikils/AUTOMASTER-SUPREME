@echo off
title Companion Studio - Tu Compañera Virtual Privada
cd /d "%~dp0"

echo ========================================================
echo       COMPANION STUDIO - COMPAÑERA VIRTUAL PRIVADA
echo             NVIDIA GeForce RTX 4060 Edition
echo ========================================================
echo.

set PYTHON_EXEC=..\wan-desktop-studio\wan_env\Scripts\python.exe

if not exist "%PYTHON_EXEC%" (
    set PYTHON_EXEC=c:\Users\alfaswz\.gemini\antigravity-ide\scratch\agentes-personalizados\wan-desktop-studio\wan_env\Scripts\python.exe
)

if not exist "%PYTHON_EXEC%" (
    echo [!] No se encontro el entorno virtual Python en wan-desktop-studio.
    pause
    exit /b 1
)

echo [*] Iniciando Companion Studio...
echo [*] Conectando motores de chat y generacion de imagenes en RTX 4060...
echo.

"%PYTHON_EXEC%" main_desktop.py

if %ERRORLEVEL% neq 0 (
    echo.
    echo [!] La aplicacion se ha cerrado.
    pause
)
