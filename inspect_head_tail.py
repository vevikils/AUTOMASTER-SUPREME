from parse_fl_events import parse_fl_events

evs = parse_fl_events(open(r"C:\Program Files\Image-Line\FL Studio 2026\Data\Patches\Mixer presets\Guitar stuff.fst", "rb").read())
print("Total events in Guitar stuff:", len(evs))
print("First 10 events:")
for i in range(min(10, len(evs))):
    print(f"[{i}] ID={evs[i][0]} TYPE={evs[i][1]}")
    if evs[i][1] == "VAR":
        print(f"     len={len(evs[i][2])} val={repr(evs[i][2][:50])}")
    else:
        print(f"     val={evs[i][2]}")

print("\nLast 10 events:")
for i in range(max(0, len(evs)-10), len(evs)):
    print(f"[{i}] ID={evs[i][0]} TYPE={evs[i][1]}")
    if evs[i][1] == "VAR":
        print(f"     len={len(evs[i][2])} val={repr(evs[i][2][:50])}")
    else:
        print(f"     val={evs[i][2]}")
