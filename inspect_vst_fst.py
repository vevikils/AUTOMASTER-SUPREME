import os

def inspect_file(p):
    print("=== File:", p)
    if not os.path.exists(p):
        print("Does not exist!")
        return
    with open(p, "rb") as f:
        data = f.read()
    print("Size:", len(data))
    print("Header:", data[:16])

inspect_file(r"C:\Users\alfaswz\Documents\Image-Line\FL Studio\Presets\Plugin database\Installed\Effects\VST3\Auto-Tune Pro.fst")
inspect_file(r"C:\Users\alfaswz\Documents\Image-Line\FL Studio\Presets\Plugin database\Installed\Effects\VST3\Pro-Q 4.fst")
inspect_file(r"C:\Users\alfaswz\Documents\Image-Line\FL Studio\Presets\Plugin database\Installed\Effects\VST3\CLA-76 Stereo.fst")
inspect_file(r"C:\Users\alfaswz\Documents\Image-Line\FL Studio\Presets\Plugin database\Installed\Effects\VST3\REVERB SUPREME PRO.fst")
