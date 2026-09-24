from parse_fl_events import parse_fl_events

evs = parse_fl_events(open(r"C:\Program Files\Image-Line\FL Studio 2026\Data\Patches\Mixer presets\1-bar overdub recording.fst", "rb").read())
print("--- 1-bar overdub recording.fst events ---")
for i, e in enumerate(evs):
    if e[0] in [201, 204, 199, 149, 212, 28]:
        print(f"[{i}] ID={e[0]} TYPE={e[1]}: {e[2] if e[1] != 'VAR' else e[2][:30]}")
    elif e[1] in ["BYTE", "WORD", "DWORD"]:
        print(f"[{i}] ID={e[0]} TYPE={e[1]}: {e[2]}")
    else:
        print(f"[{i}] ID={e[0]} TYPE={e[1]} LEN={len(e[2])}: {e[2][:20]}")
