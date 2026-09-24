from parse_fl_events import parse_fl_events

for name in ["CLA-76 Stereo.fst", "CLA-2A Stereo.fst", "Pro-DS.fst", "Saturn 2.fst", "RVox Stereo.fst", "REVERB SUPREME PRO.fst", "Pro-C 2.fst"]:
    path = rf"C:\Users\alfaswz\Documents\Image-Line\FL Studio\Presets\Plugin database\Installed\Effects\VST3\{name}"
    evs = parse_fl_events(open(path, "rb").read())
    print(f"\n--- {name} ---")
    for e in evs:
        if e[0] in [201, 203]:
            print(f"  ID={e[0]}: {repr(e[2])}")
        elif e[0] == 212:
            print(f"  ID=212: len={len(e[2])}")
        elif e[0] == 213:
            print(f"  ID=213: len={len(e[2])}")
