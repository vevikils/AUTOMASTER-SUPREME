"""Wan 2.1 Video Studio - Native Desktop Application Launcher
Launches FastAPI backend in a background thread and opens a native Windows Edge WebView2 desktop window.
"""

import warnings
warnings.filterwarnings("ignore", category=FutureWarning)

import os
import sys
import time
import socket
import threading
import webbrowser
import logging
import uvicorn

# Setup paths
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
if BASE_DIR not in sys.path:
    sys.path.insert(0, BASE_DIR)

from server.app import app

logging.basicConfig(level=logging.INFO, format="%(asctime)s [%(levelname)s] %(name)s: %(message)s")
logger = logging.getLogger("WanDesktop")

def find_free_port(start_port=7860, max_port=7900):
    for p in range(start_port, max_port):
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            if s.connect_ex(('127.0.0.1', p)) != 0:
                return p
    return 7860

def run_server(port):
    uvicorn.run(app, host="127.0.0.1", port=port, log_level="warning")

def wait_for_server(port, timeout=15):
    start = time.time()
    while time.time() - start < timeout:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            if s.connect_ex(('127.0.0.1', port)) == 0:
                return True
        time.sleep(0.3)
    return False

def main():
    print("=" * 65)
    print("      WAN 2.1 VIDEO STUDIO - RTX 4060 EDITION")
    print("      Optimizaciones activas: CPU Offload, VAE Tiling, TF32")
    print("=" * 65)

    port = find_free_port()
    server_url = f"http://127.0.0.1:{port}"
    print(f"[*] Iniciando servidor local en {server_url}...")

    # Start FastAPI server in background thread
    server_thread = threading.Thread(target=run_server, args=(port,), daemon=True)
    server_thread.start()

    if not wait_for_server(port):
        print("[!] Error: El servidor no respondió a tiempo.")
        sys.exit(1)

    print(f"[+] Servidor en línea. Abriendo ventana de escritorio...")

    # Try PyWebView for native window
    has_webview = False
    try:
        import webview
        has_webview = True
    except ImportError:
        logger.info("pywebview no disponible, se abrirá en navegador.")

    if has_webview:
        try:
            window = webview.create_window(
                title="Wan 2.1 Video Studio - RTX 4060 Edition",
                url=server_url,
                width=1400,
                height=920,
                min_size=(1024, 700),
                background_color="#080a0f",
                text_select=True,
                easy_drag=True
            )
            webview.start(gui="edgechromium")
            return
        except Exception as e:
            logger.warning(f"Error al abrir ventana pywebview: {e}. Abriendo en navegador por defecto...")

    # Fallback to browser
    webbrowser.open(server_url)
    print(f"\n[+] Aplicación ejecutándose en: {server_url}")
    print("Presiona Ctrl+C en esta consola para detener la aplicación.\n")
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        print("\nCerrando Wan 2.1 Video Studio...")

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        import traceback
        print(f"\n[!] Error crítico en la aplicación: {e}")
        traceback.print_exc()
        input("\nPresiona Enter para continuar...")
        sys.exit(1)
