from parse_fl_events import parse_fl_events
import struct

evs = parse_fl_events(open(r"C:\Program Files\Image-Line\FL Studio 2026\Data\Patches\Mixer presets\Guitar stuff.fst", "rb").read())
print("--- Guitar stuff.fst plugins & event 212 ---")
plugin_name = ""
for i, e in enumerate(evs):
    if e[0] == 201:
        plugin_name = e[2]
    elif e[0] == 212:
        ints = struct.unpack(f"<{len(e[2])//4}I", e[2])
        print(f"Plugin: {plugin_name} (len={len(e[2])}): {ints}")
