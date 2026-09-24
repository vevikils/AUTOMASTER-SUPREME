"""Companion Studio - Native Desktop Application Launcher
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
logger = logging.getLogger("CompanionDesktop")

def find_free_port(start_port=7870, max_port=7920):
    for p in range(start_port, max_port):
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            if s.connect_ex(('127.0.0.1', p)) != 0:
                return p
    return 7870

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
    print("      COMPANION STUDIO - COMPAÑERA VIRTUAL PRIVADA")
    print("      NVIDIA GeForce RTX 4060 Edition • 100% Local y Privado")
    print("=" * 65)

    port = find_free_port()
    server_url = f"http://127.0.0.1:{port}"
    print(f"[*] Iniciando servidor local privado en {server_url}...")

    server_thread = threading.Thread(target=run_server, args=(port,), daemon=True)
    server_thread.start()

    if not wait_for_server(port):
        print("[!] Error: El servidor no respondió a tiempo.")
        sys.exit(1)

    print(f"[✓] Servidor activo. Abriendo ventana de escritorio...")

    has_webview = False
    try:
        import webview
        has_webview = True
    except ImportError:
        pass

    if has_webview:
        try:
            window = webview.create_window(
                title="Companion Studio - Tu Compañera Virtual Privada",
                url=server_url,
                width=1380,
                height=900,
                min_size=(1024, 700),
                background_color="#07090e",
                text_select=True,
                easy_drag=True
            )
            webview.start(gui="edgechromium")
            return
        except Exception as e:
            logger.warning(f"Error pywebview: {e}. Abriendo en navegador por defecto...")

    webbrowser.open(server_url)
    print(f"\n[✓] Aplicación ejecutándose en: {server_url}")
    print("Presiona Ctrl+C en esta consola para detener la aplicación.\n")
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        print("\nCerrando Companion Studio...")

if __name__ == "__main__":
    main()
