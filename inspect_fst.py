import os
import struct

def inspect_fst(path):
    print("=== Inspecting:", path)
    with open(path, "rb") as f:
        data = f.read()
    print("Total size:", len(data))
    magic = data[:4]
    print("Magic:", magic)
    # Check strings inside data
    strings = []
    current = []
    for b in data:
        if 32 <= b <= 126:
            current.append(chr(b))
        else:
            if len(current) >= 4:
                strings.append("".join(current))
            current = []
    print("Strings sample (first 40):")
    for s in strings[:40]:
        print("  ", s)

inspect_fst(r"C:\Program Files\Image-Line\FL Studio 2026\Data\Patches\Mixer presets\Default.fst")
inspect_fst(r"C:\Program Files\Image-Line\FL Studio 2026\Data\Patches\Mixer presets\Chorus send.fst")
