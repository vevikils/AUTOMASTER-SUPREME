from parse_fl_events import parse_fl_events
from test_roundtrip import encode_fl_events

path = r"C:\Program Files\Image-Line\FL Studio 2026\Data\Patches\Mixer presets\Default.fst"
orig = open(path, "rb").read()
flhd_payload = orig[8:14]
evs = parse_fl_events(orig)
reencoded = encode_fl_events(evs, flhd_payload=flhd_payload)
print("Exact match with matching flhd:", orig == reencoded)
