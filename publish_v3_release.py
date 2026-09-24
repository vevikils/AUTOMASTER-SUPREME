import os
import zipfile
import subprocess

KIT_DIR = r"c:\Users\alfaswz\.gemini\antigravity-ide\scratch\agentes-personalizados\SUPREME-PRODUCER-DRUM-KIT"
ZIP_FILE = "SUPREME-PRODUCER-SUITE-v3.0.0.zip"
ZIP_PATH = os.path.join(KIT_DIR, ZIP_FILE)

# Remove any old zip files in root
for f in os.listdir(KIT_DIR):
    if f.endswith(".zip") and f != ZIP_FILE:
        try:
            os.remove(os.path.join(KIT_DIR, f))
            print("Removed old zip:", f)
        except Exception as e:
            print("Could not remove:", f, e)

print("Creating ZIP package for v3.0.0...")
with zipfile.ZipFile(ZIP_PATH, "w", zipfile.ZIP_DEFLATED) as zipf:
    for root, dirs, files in os.walk(KIT_DIR):
        if ".git" in root:
            continue
        for file in files:
            if file.endswith(".zip"):
                continue
            full_path = os.path.join(root, file)
            rel_path = os.path.relpath(full_path, KIT_DIR)
            zipf.write(full_path, rel_path)

zip_size_mb = os.path.getsize(ZIP_PATH) / (1024 * 1024)
print(f"ZIP package v3.0.0 created: {ZIP_PATH} ({zip_size_mb:.2f} MB)")

def run(cmd, cwd=KIT_DIR):
    print(f"\n--- Running: {' '.join(cmd) if isinstance(cmd, list) else cmd} ---")
    res = subprocess.run(cmd, cwd=cwd, capture_output=True, text=True, encoding="utf-8", shell=isinstance(cmd, str))
    print("STDOUT:", res.stdout.strip())
    if res.stderr.strip():
        print("STDERR:", res.stderr.strip())
    return res

# Git add, commit, push (remember *.zip is in .gitignore so only trackable code/audio is pushed)
run(["git", "add", "."])
run(["git", "commit", "-m", "feat: v3.0.0 - 64 authentic hit drum loops (Argentine Trap, PR Trap, Classic & Modern Dembow) + 32 complex melodies"])
run(["git", "push", "origin", "main"])

# Release notes
notes = """Supreme Producer Suite v3.0.0 (Definitive Edition)

Authentic Hit Track Drum Loops & Complex Melodies:
- 64 Authentic Hit Drum Loops with dynamic 4-bar variations and fills:
  * 16 Trap Argentino (Duki, YSY A, Bizarrap, Khea, Tiago PZK, Neo Pistea)
  * 16 Latin Trap Puerto Rico (Bad Bunny, Anuel AA, Eladio Carrion, Myke Towers)
  * 16 Reggaeton Clasico PR (Daddy Yankee, Don Omar, Wisin & Yandel, Plan B, Luny Tunes)
  * 16 Reggaeton Moderno & Tainy Style (Tainy, Bad Bunny, Feid, Mora, Rauw Alejandro)
- 32 Complex Hit-Inspired Melodies (Warm analog pads, Rhodes, acoustic guitars, flutes - No harshness)
- 34 Drum One-Shots (Tuned 808s in C, punch kicks, claps, snares, hats, percs, FX)
- 8 MIDI Patterns for Piano Roll
- 1-Click Windows Auto-Installer (install.bat)
- 100% Royalty-Free for Commercial and Personal Releases
"""

notes_file = os.path.join(KIT_DIR, "release_notes_v3.txt")
with open(notes_file, "w", encoding="utf-8") as f:
    f.write(notes)

rel_cmd = [
    "gh", "release", "create", "v3.0.0",
    ZIP_FILE,
    "--title", "Supreme Producer Suite v3.0.0 (64 Authentic Drum Loops + 32 Melodies)",
    "--notes-file", "release_notes_v3.txt"
]
run(rel_cmd)
