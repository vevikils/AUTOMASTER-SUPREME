import os

paths_to_check = [
    r"C:\Program Files\Image-Line",
    r"C:\Users\alfaswz\Documents\Image-Line"
]

found_ansi = []
found_utf16 = []

for p in paths_to_check:
    for root, dirs, files in os.walk(p):
        for f in files:
            if f.endswith(".fst"):
                full = os.path.join(root, f)
                try:
                    data = open(full, "rb").read()
                    if b"Fruity Wrapper\x00" in data:
                        found_ansi.append(full)
                    if b"F\x00r\x00u\x00i\x00t\x00y\x00 \x00W\x00r\x00a\x00p\x00p\x00e\x00r\x00" in data:
                        found_utf16.append(full)
                except Exception:
                    pass

print("ANSI Fruity Wrapper count:", len(found_ansi))
if found_ansi:
    print("Sample ANSI:", found_ansi[0])
print("UTF16 Fruity Wrapper count:", len(found_utf16))
if found_utf16:
    print("Sample UTF16:", found_utf16[0])
