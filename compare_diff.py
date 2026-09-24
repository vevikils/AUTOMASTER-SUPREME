from parse_fl_events import parse_fl_events
from test_roundtrip import encode_fl_events

path = r"C:\Program Files\Image-Line\FL Studio 2026\Data\Patches\Mixer presets\Default.fst"
orig = open(path, "rb").read()
evs = parse_fl_events(orig)
reencoded = encode_fl_events(evs)
for i in range(len(orig)):
    if orig[i] != reencoded[i]:
        print(f"Diff at byte {i}: orig={orig[i]:02x} re={reencoded[i]:02x}")
        print("Surrounding orig:", orig[max(0, i-5):i+10])
        print("Surrounding re:  ", reencoded[max(0, i-5):i+10])
        break
