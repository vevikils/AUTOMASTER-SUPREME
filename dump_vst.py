from parse_fl_events import parse_fl_events

def dump_file(p):
    print("=== DUMP:", p)
    evs = parse_fl_events(open(p, "rb").read())
    for i, e in enumerate(evs):
        print(f"[{i}] ID={e[0]} TYPE={e[1]}:")
        if e[1] == "VAR":
            print(f"     len={len(e[2])} content={repr(e[2][:80])}")
        else:
            print(f"     val={e[2]}")

dump_file(r"C:\Users\alfaswz\Documents\Image-Line\FL Studio\Presets\Plugin database\Installed\Effects\VST3\Auto-Tune Pro.fst")
dump_file(r"C:\Users\alfaswz\Documents\Image-Line\FL Studio\Presets\Plugin database\Installed\Effects\VST3\Pro-Q 4.fst")
