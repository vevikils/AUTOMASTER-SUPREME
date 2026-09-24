from parse_fl_events import parse_fl_events
import struct
import re

path = r"C:\Users\alfaswz\Desktop\TRAVIS SCOTT - VOCAL CHAIN (PRO).fst"
evs = parse_fl_events(open(path, "rb").read())
print("=== VERIFYING FINAL PRESET ===")
print("Total events:", len(evs))

slots_found = []
current_plugin = "Unknown"
for i, e in enumerate(evs):
    if e[0] == 201:
        try:
            current_plugin = e[2].decode('utf-16le').strip('\x00')
        except Exception:
            current_plugin = e[2].decode('latin-1').strip('\x00')
    elif e[0] == 213:
        txts = re.findall(b"[a-zA-Z0-9_ -]{3,}", e[2])
        vsts = [t.decode('latin-1') for t in txts if any(k in t.lower() for k in [b"antares", b"tune", b"cla", b"rvox", b"filter", b"fresh", b"valhalla", b"supermassive"])]
        slots_found.append((current_plugin, vsts[:2], len(e[2])))
    elif e[0] == 98:
        print(f"Slot {e[2]}: {slots_found[-1]}")
    elif e[0] == 204:
        print("Track Name:", e[2].decode('utf-16le').strip('\x00'))
    elif e[0] == 28:
        print("Unicode Flag (ID=28):", e[2])
