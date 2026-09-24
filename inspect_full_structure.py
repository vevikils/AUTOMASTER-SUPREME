from parse_fl_events import parse_fl_events

path = r"C:\Users\alfaswz\Documents\Image-Line\FL Studio\Presets\Mixer presets\VOCES TUNE + FRESH AIR - ABRIL 2026.fst"
evs = parse_fl_events(open(path, "rb").read())
print("Events in VOCES TUNE + FRESH AIR - ABRIL 2026.fst:")
for i, e in enumerate(evs):
    if e[1] == "VAR":
        preview = repr(e[2][:40])
        try:
            u = e[2].decode('utf-16le').strip('\x00')
            if len(u) > 0 and all(32 <= ord(c) < 128 for c in u):
                preview = f"u16: '{u}'"
        except Exception:
            pass
        print(f"[{i:2d}] ID={e[0]:3d} TYPE={e[1]:5s} len={len(e[2]):5d} val={preview}")
    else:
        print(f"[{i:2d}] ID={e[0]:3d} TYPE={e[1]:5s} val={e[2]}")
