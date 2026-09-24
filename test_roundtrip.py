import struct
from parse_fl_events import parse_fl_events

def encode_var_len(length):
    res = bytearray()
    while True:
        b = length & 0x7F
        length >>= 7
        if length > 0:
            res.append(b | 0x80)
        else:
            res.append(b)
            break
    return bytes(res)

def encode_fl_events(events, flhd_payload=b'0\x00\x02\x000\x00'):
    dt_body = bytearray()
    for e_id, e_type, val in events:
        if e_type == "BYTE":
            dt_body.append(e_id)
            dt_body.append(val & 0xFF)
        elif e_type == "WORD":
            dt_body.append(e_id)
            dt_body.extend(struct.pack("<H", val))
        elif e_type == "DWORD":
            dt_body.append(e_id)
            dt_body.extend(struct.pack("<I", val))
        elif e_type == "VAR":
            dt_body.append(e_id)
            dt_body.extend(encode_var_len(len(val)))
            dt_body.extend(val)
        else:
            raise ValueError(f"Unknown type {e_type}")
            
    header = b'FLhd' + struct.pack("<I", len(flhd_payload)) + flhd_payload
    data_chunk = b'FLdt' + struct.pack("<I", len(dt_body)) + bytes(dt_body)
    return header + data_chunk

# Test roundtrip on Default.fst and Chorus send.fst
for path in [
    r"C:\Program Files\Image-Line\FL Studio 2026\Data\Patches\Mixer presets\Default.fst",
    r"C:\Program Files\Image-Line\FL Studio 2026\Data\Patches\Mixer presets\Chorus send.fst",
    r"C:\Users\alfaswz\Documents\Image-Line\FL Studio\Presets\Plugin database\Installed\Effects\VST3\Auto-Tune Pro.fst"
]:
    orig = open(path, "rb").read()
    evs = parse_fl_events(orig)
    reencoded = encode_fl_events(evs)
    if orig == reencoded:
        print(f"SUCCESS: Exact bit-for-bit match on {path} ({len(orig)} bytes)")
    else:
        print(f"MISMATCH on {path}: orig={len(orig)} reencoded={len(reencoded)}")
