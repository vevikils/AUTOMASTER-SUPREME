from parse_fl_events import parse_fl_events
import struct

path = r"C:\Users\alfaswz\Documents\Image-Line\FL Studio\Presets\Mixer presets\2026 TUNE TS - IA.fst"
print("=== Inspecting 2026 TUNE TS - IA.fst ===")
evs = parse_fl_events(open(path, "rb").read())
print("Total events:", len(evs))
for i, e in enumerate(evs):
    if e[1] == "VAR":
        val_str = repr(e[2][:50])
        # try utf-16 decode
        try:
            u_str = e[2].decode('utf-16le').strip('\x00')
            if len(u_str) > 0 and all(32 <= ord(c) < 128 for c in u_str):
                val_str += f" (u16: '{u_str}')"
        except Exception:
            pass
        print(f"[{i:2d}] ID={e[0]:3d} TYPE={e[1]:5s} len={len(e[2]):4d} val={val_str}")
    else:
        print(f"[{i:2d}] ID={e[0]:3d} TYPE={e[1]:5s} val={e[2]}")
