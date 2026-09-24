import struct

def parse_fl_events(data):
    # Skip FLhd
    if not data.startswith(b'FLhd'):
        print("Not FLhd")
        return
    hd_size = struct.unpack("<I", data[4:8])[0]
    pos = 8 + hd_size
    if data[pos:pos+4] != b'FLdt':
        print("Expected FLdt at", pos)
        return
    dt_size = struct.unpack("<I", data[pos+4:pos+8])[0]
    pos += 8
    end = pos + dt_size
    print(f"Parsing FLdt size={dt_size} from {pos} to {end}")
    
    events = []
    while pos < end:
        event_id = data[pos]
        pos += 1
        # Determine payload type from event_id
        # In FL format:
        # 0..63: Byte (1 byte)
        # 64..127: Word (2 bytes)
        # 128..191: DWord (4 bytes)
        # 192..255: Text / Variable length
        if event_id < 64:
            val = data[pos]
            pos += 1
            events.append((event_id, "BYTE", val))
        elif event_id < 128:
            val = struct.unpack("<H", data[pos:pos+2])[0]
            pos += 2
            events.append((event_id, "WORD", val))
        elif event_id < 192:
            val = struct.unpack("<I", data[pos:pos+4])[0]
            pos += 4
            events.append((event_id, "DWORD", val))
        else:
            # Variable length
            # Length is encoded with variable byte (like MIDI):
            # while byte & 0x80: len = (len << 7) | (b & 0x7F)
            length = 0
            shift = 0
            while True:
                b = data[pos]
                pos += 1
                length |= (b & 0x7F) << shift
                shift += 7
                if not (b & 0x80):
                    break
            payload = data[pos:pos+length]
            pos += length
            events.append((event_id, "VAR", payload))
            
    return events

print("--- Chorus send.fst ---")
evs = parse_fl_events(open(r"C:\Program Files\Image-Line\FL Studio 2026\Data\Patches\Mixer presets\Chorus send.fst", "rb").read())
for e in evs:
    print(e[0], e[1], e[2] if e[1] != "VAR" else (e[2][:50], len(e[2])))
