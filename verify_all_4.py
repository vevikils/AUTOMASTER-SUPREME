from parse_fl_events import parse_fl_events
import os

files = [
    "DUKI - SUPER ROBOTIC TRAP (PRO).fst",
    "ANUEL AA - MELODIC TRAP (PRO).fst",
    "BAD BUNNY - PRIME ERA (PRO).fst",
    "OZUNA - VOZ AGUDA (PRO).fst"
]

desktop = r"C:\Users\alfaswz\Desktop"

for f in files:
    path = os.path.join(desktop, f)
    evs = parse_fl_events(open(path, "rb").read())
    name = [e[2].decode('utf-16le').strip('\x00') for e in evs if e[0] == 204][0]
    color = [f"#{e[2]:06X}" for e in evs if e[0] == 149][0]
    slots = [e[2] for e in evs if e[0] == 98]
    print(f"Verified: {f:36s} -> Name: '{name:18s}' Color: {color} Slots: {len(slots)}")
