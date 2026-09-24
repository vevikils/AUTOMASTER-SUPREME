from parse_fl_events import parse_fl_events

def inspect_events(path):
    print(f"=== {path} ===")
    evs = parse_fl_events(open(path, "rb").read())
    for i, e in enumerate(evs):
        if e[0] in [199, 28, 201, 203, 212]:
            print(f"[{i}] ID={e[0]} TYPE={e[1]}: {repr(e[2]) if e[1] == 'VAR' else e[2]}")

inspect_events(r"C:\Program Files\Image-Line\FL Studio 2026\Data\Patches\Plugin presets\Effects\Patcher\Special\One knob multi effects.fst")
