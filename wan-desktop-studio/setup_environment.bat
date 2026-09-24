@echo off
title Setup Wan 2.1 Video Studio Environment
cd /d "%~dp0"

echo ========================================================
echo   CONFIGURANDO ENTORNO WAN 2.1 PARA RTX 4060
echo ========================================================
echo.

set PYTHON_EXE=C:\Users\alfaswz\AppData\Local\Programs\Python\Python311\python.exe

if not exist "%PYTHON_EXE%" (
    echo [!] No se encontro Python 3.11 en:
    echo     %PYTHON_EXE%
    echo     Por favor verifica la instalacion de Python.
    pause
    exit /b 1
)

echo [*] Creando entorno virtual wan_env con Python 3.11...
"%PYTHON_EXE%" -m venv wan_env

echo [*] Actualizando pip...
wan_env\Scripts\python.exe -m pip install --upgrade pip

echo [*] Instalando PyTorch con aceleracion CUDA 12.4...
wan_env\Scripts\python.exe -m pip install torch torchvision --index-url https://download.pytorch.org/whl/cu124

echo [*] Instalando librerias de inferencia y entorno de escritorio...
wan_env\Scripts\python.exe -m pip install -r requirements.txt

echo.
echo ========================================================
echo   [OK] Entorno configurado correctamente!
echo   Ya puedes ejecutar Iniciar_Wan_Studio.bat
echo ========================================================
echo.
pause
