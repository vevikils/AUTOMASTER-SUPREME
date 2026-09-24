import os

log_path = r"C:\Users\alfaswz\Documents\Image-Line\FL Studio\Support\Logs\PluginManager v1.8.1.5589.log"
if os.path.exists(log_path):
    print("Found PluginManager log!")
    with open(log_path, "r", encoding="utf-8", errors="ignore") as f:
        for line in f:
            if "auto-tune" in line.lower() or "autotune" in line.lower():
                print(line.strip())
