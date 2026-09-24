import os
from parse_fl_events import parse_fl_events

dir_path = r"C:\Program Files\Image-Line\FL Studio 2026\Data\Patches\Mixer presets"
for f in os.listdir(dir_path):
    if f.endswith(".fst"):
        path = os.path.join(dir_path, f)
        evs = parse_fl_events(open(path, "rb").read())
        has_28 = any(e[0] == 28 for e in evs)
        has_199 = [e[2] for e in evs if e[0] == 199]
        names = [e[2] for e in evs if e[0] == 201]
        print(f"{f}: has_28={has_28}, ver={has_199}, plugins={names[:2]}")
