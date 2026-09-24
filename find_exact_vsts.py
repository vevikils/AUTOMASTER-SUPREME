from parse_fl_events import parse_fl_events
import re

for preset in [r"C:\Users\alfaswz\Documents\Image-Line\FL Studio\Presets\Mixer presets\VOCES TUNE + FRESH AIR - ABRIL 2026.fst",
               r"C:\Users\alfaswz\Documents\Image-Line\FL Studio\Presets\Mixer presets\Voces de rap jaf y diego - abril 2026.fst"]:
    print("=== File:", preset)
    evs = parse_fl_events(open(preset, "rb").read())
    for i, e in enumerate(evs):
        if e[0] == 213:
            # find all ascii words
            words = re.findall(b"[a-zA-Z0-9_ -]{3,}", e[2])
            interesting = [w.decode('latin-1') for w in words if any(x in w.lower() for x in [b"cla", b"rvox", b"vox", b"renaissance", b"comp", b"q4", b"ds", b"fresh", b"tune", b"antares"])]
            if interesting:
                print(f"  Event[{i}] len={len(e[2])}: {interesting[:6]}")
