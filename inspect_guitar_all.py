from parse_fl_events import parse_fl_events

evs = parse_fl_events(open(r"C:\Program Files\Image-Line\FL Studio 2026\Data\Patches\Mixer presets\Guitar stuff.fst", "rb").read())
print("All event IDs in Guitar stuff.fst:")
for i, e in enumerate(evs):
    print(f"[{i:2d}] ID={e[0]:3d} TYPE={e[1]:5s} {e[2][:40] if e[1] == 'VAR' else e[2]}")
