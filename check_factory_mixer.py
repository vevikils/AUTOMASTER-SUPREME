import os
from parse_fl_events import parse_fl_events

dir_path = r"C:\Program Files\Image-Line\FL Studio 2026\Data\Patches\Mixer presets"
for f in os.listdir(dir_path):
    if f.endswith(".fst"):
        full = os.path.join(dir_path, f)
        data = open(full, "rb").read()
        evs = parse_fl_events(data)
        # Check how many 201 or 212 events exist
        plugins = [e[2] for e in evs if e[0] == 201]
        print(f"{f}: {len(plugins)} plugins -> {plugins}")
