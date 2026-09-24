from parse_fl_events import parse_fl_events
import struct

def show_212(label, path):
    evs = parse_fl_events(open(path, "rb").read())
    for e in evs:
        if e[0] == 212:
            raw = e[2]
            ints = struct.unpack(f"<{len(raw)//4}I", raw)
            print(f"{label:25s} len={len(raw)}: {ints}")

show_212("Auto-Tune Pro", r"C:\Users\alfaswz\Documents\Image-Line\FL Studio\Presets\Plugin database\Installed\Effects\VST3\Auto-Tune Pro.fst")
show_212("Pro-Q 4", r"C:\Users\alfaswz\Documents\Image-Line\FL Studio\Presets\Plugin database\Installed\Effects\VST3\Pro-Q 4.fst")
show_212("Guitar Slot 1", r"C:\Program Files\Image-Line\FL Studio 2026\Data\Patches\Mixer presets\Guitar stuff.fst")
