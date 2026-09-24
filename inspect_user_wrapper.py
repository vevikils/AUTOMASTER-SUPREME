from parse_fl_events import parse_fl_events

path = r"C:\Users\alfaswz\Documents\Image-Line\FL Studio\Presets\Mixer presets\2026 TUNE TS - IA.fst"
evs = parse_fl_events(open(path, "rb").read())

# Check slots 0 and 4
for i in [9, 14, 45, 50]:
    e = evs[i]
    print(f"[{i}] ID={e[0]} len={len(e[2]) if e[1] == 'VAR' else 0}")
    if e[0] == 213:
        # Search strings inside 213
        s = [c for c in e[2] if 32 <= c <= 126]
        # find ascii substrings
        import re
        txts = re.findall(b"[a-zA-Z0-9_ -]{4,}", e[2])
        print("  Strings in 213:", txts)
