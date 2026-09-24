from parse_fl_events import parse_fl_events
import struct
import re

path = r"C:\Users\alfaswz\Documents\Image-Line\FL Studio\Presets\Mixer presets\JAF - voz final junio.fst"
print("=== JAF - voz final junio.fst slots ===")
evs = parse_fl_events(open(path, "rb").read())

slots = []
cur_p = "Unknown"
for i, e in enumerate(evs):
    if e[0] == 201:
        try:
            cur_p = e[2].decode('utf-16le').strip('\x00')
        except Exception:
            cur_p = e[2].decode('latin-1').strip('\x00')
    elif e[0] == 203:
        try:
            d = e[2].decode('utf-16le').strip('\x00')
            cur_p += f" ({d})"
        except Exception:
            pass
    elif e[0] == 213:
        txts = re.findall(b"[a-zA-Z0-9_ -]{3,}", e[2])
        vsts = [t.decode('latin-1') for t in txts if any(k in t.lower() for k in [b"antares", b"tune", b"cla", b"rvox", b"vox", b"filter", b"fresh", b"waves", b"waveshell", b"eq"])]
        slots.append((cur_p, vsts[:4], len(e[2])))
    elif e[0] == 98:
        print(f"Slot {e[2]}: {slots[-1] if slots else 'None'}")
