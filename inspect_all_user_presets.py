import os
from parse_fl_events import parse_fl_events
import re

preset_dir = r"C:\Users\alfaswz\Documents\Image-Line\FL Studio\Presets\Mixer presets"
for f in os.listdir(preset_dir):
    if f.endswith(".fst") and f != "TRAVIS SCOTT - VOCAL CHAIN (PRO).fst":
        full = os.path.join(preset_dir, f)
        evs = parse_fl_events(open(full, "rb").read())
        plugins = []
        for e in evs:
            if e[0] == 201:
                try:
                    name = e[2].decode('utf-16le').strip('\x00')
                except Exception:
                    name = e[2].decode('latin-1').strip('\x00')
                plugins.append(name)
            elif e[0] == 213:
                txts = re.findall(b"[a-zA-Z0-9_ -]{4,}", e[2])
                # look for plugin names or paths
                vst_names = [t.decode('latin-1') for t in txts if any(k in t.lower() for k in [b"fabfilter", b"waves", b"antares", b"valhalla", b"cla", b"pro-q", b"pro-c", b"pro-ds", b"rvox", b"saturn"])]
                if vst_names:
                    plugins.append(f"-> VST: {vst_names[:2]}")
        print(f"Preset: {f} ({len(plugins)} items)")
        for p in plugins:
            print("  ", p)
