from parse_fl_events import parse_fl_events
import struct
import re

path = r"C:\Users\alfaswz\Documents\Image-Line\FL Studio\Presets\Mixer presets\VOCES TUNE + FRESH AIR - ABRIL 2026.fst"
print("=== VOCES TUNE + FRESH AIR - ABRIL 2026.fst ===")
evs = parse_fl_events(open(path, "rb").read())

# Track slots
slot_plugins = []
current_slot = None
current_plugin = "Unknown"

for i, e in enumerate(evs):
    if e[0] == 98: # Slot marker
        current_slot = e[2]
    elif e[0] == 201: # Plugin name
        try:
            name = e[2].decode('utf-16le').strip('\x00')
        except Exception:
            name = e[2].decode('latin-1').strip('\x00')
        current_plugin = name
    elif e[0] == 203: # Display name
        try:
            dname = e[2].decode('utf-16le').strip('\x00')
        except Exception:
            dname = e[2].decode('latin-1').strip('\x00')
        current_plugin += f" [{dname}]"
    elif e[0] == 213: # VST state
        txts = re.findall(b"[a-zA-Z0-9_ -]{4,}", e[2])
        vsts = [t.decode('latin-1') for t in txts if any(k in t.lower() for k in [b"fabfilter", b"waves", b"antares", b"cla", b"pro-q", b"pro-c", b"pro-ds", b"rvox", b"fresh air"])]
        info = f"Slot {current_slot}: {current_plugin} (VSTs: {vsts[:3]}) (len={len(e[2])})"
        slot_plugins.append(info)

for s in slot_plugins:
    print("  ", s)
