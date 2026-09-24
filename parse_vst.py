from parse_fl_events import parse_fl_events

print("--- Auto-Tune Pro.fst ---")
evs = parse_fl_events(open(r"C:\Users\alfaswz\Documents\Image-Line\FL Studio\Presets\Plugin database\Installed\Effects\VST3\Auto-Tune Pro.fst", "rb").read())
for e in evs:
    print(e[0], e[1], e[2] if e[1] != "VAR" else (e[2][:60], len(e[2])))

print("\n--- Pro-Q 4.fst ---")
evs = parse_fl_events(open(r"C:\Users\alfaswz\Documents\Image-Line\FL Studio\Presets\Plugin database\Installed\Effects\VST3\Pro-Q 4.fst", "rb").read())
for e in evs:
    print(e[0], e[1], e[2] if e[1] != "VAR" else (e[2][:60], len(e[2])))
