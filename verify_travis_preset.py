from parse_fl_events import parse_fl_events
import struct

path = r"C:\Users\alfaswz\Desktop\TRAVIS SCOTT - VOCAL CHAIN (PRO).fst"
evs = parse_fl_events(open(path, "rb").read())
print("--- Parsing TRAVIS SCOTT - VOCAL CHAIN (PRO).fst ---")
print("Total events:", len(evs))

slots_found = []
for i, e in enumerate(evs):
    if e[0] == 204:
        print("Track Name:", e[2])
    elif e[0] == 149:
        print(f"Track Color: #{e[2]:06X}")
    elif e[0] == 27:
        print("Track Icon:", e[2])
    elif e[0] == 212:
        slot = struct.unpack("<IIII", e[2][:16])[1]
        active = struct.unpack("<IIII", e[2][:16])[2]
        mix = struct.unpack("<IIIII", e[2][:20])[4]
        slots_found.append((slot, active, mix))
    elif e[0] == 203:
        pname = e[2].decode('utf-16le').strip('\x00')
        print(f"  Slot {slots_found[-1][0]}: {pname} (Active={slots_found[-1][1]}, Mix={slots_found[-1][2]}%)")

print("Total slots configured:", len(slots_found))
