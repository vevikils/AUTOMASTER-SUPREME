from parse_fl_events import parse_fl_events
import struct

path = r"C:\Users\alfaswz\Documents\Image-Line\FL Studio\Presets\Mixer presets\VOCES TUNE + FRESH AIR - ABRIL 2026.fst"
evs = parse_fl_events(open(path, "rb").read())
for i, e in enumerate(evs):
    if e[0] == 212:
        ints = struct.unpack(f"<{len(e[2])//4}I", e[2])
        print(f"[{i:3d}] 212: {ints}")
