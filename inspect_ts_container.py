from parse_fl_events import parse_fl_events

path = r"C:\Users\alfaswz\Documents\Image-Line\FL Studio\Presets\Mixer presets\2026 TUNE TS - IA.fst"
evs = parse_fl_events(open(path, "rb").read())
print("Header events (before first slot):")
for i in range(9):
    print(f"[{i}] ID={evs[i][0]} TYPE={evs[i][1]} val={evs[i][2] if evs[i][1] != 'VAR' else repr(evs[i][2])}")

print("\nFooter events (after last slot):")
for i in range(len(evs)-5, len(evs)):
    print(f"[{i}] ID={evs[i][0]} TYPE={evs[i][1]} val={evs[i][2] if evs[i][1] != 'VAR' else repr(evs[i][2][:50])}")
