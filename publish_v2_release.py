import os
import zipfile
import subprocess

KIT_DIR = r"c:\Users\alfaswz\.gemini\antigravity-ide\scratch\agentes-personalizados\SUPREME-PRODUCER-DRUM-KIT"
ZIP_FILE = "SUPREME-PRODUCER-SUITE-v2.0.0.zip"
ZIP_PATH = os.path.join(KIT_DIR, ZIP_FILE)

# Remove old v1 zip if present
v1_zip = os.path.join(KIT_DIR, "SUPREME-PRODUCER-DRUM-KIT-v1.0.0.zip")
if os.path.exists(v1_zip):
    try:
        os.remove(v1_zip)
    except Exception as e:
        print("Could not remove old zip:", e)

print("Creating ZIP package for v2.0.0...")
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
print(f"ZIP package v2.0.0 created: {ZIP_PATH} ({zip_size_mb:.2f} MB)")

def run(cmd, cwd=KIT_DIR):
    print(f"\n--- Running: {' '.join(cmd) if isinstance(cmd, list) else cmd} ---")
    res = subprocess.run(cmd, cwd=cwd, capture_output=True, text=True, encoding="utf-8", shell=isinstance(cmd, str))
    print("STDOUT:", res.stdout.strip())
    if res.stderr.strip():
        print("STDERR:", res.stderr.strip())
    return res

# Git commit & push
run(["git", "add", "."])
run(["git", "commit", "-m", "feat: v2.0.0 - 32 hit-inspired complex melodies and 62 genre drum loops"])
run(["git", "push", "origin", "main"])

# Release notes
notes = """Supreme Producer Suite v2.0.0 (Ultimate Edition)

Massive Expansion:
- 32 Complex, Hit-Inspired Melody Samples (Warm, EQ'd, Studio Reverb, Multi-layered, No harshness)
- 62 Genre Drum Loops (12 Trap, 12 Reggaeton, 12 Drill, 12 Hip-Hop/BoomBap, 14 Afro/R&B)
- All 94 loops include ACID WAV metadata for 100% Automatic Tempo Matching in FL Studio & Any DAW!
- 34 Drum One-Shots (Tuned 808s in C, Kicks, Snares, Claps, Hats, Percs, FX)
- 8 MIDI Patterns for Piano Roll
- 1-Click Windows Auto-Installer (install.bat)
- 100% Royalty-Free for Commercial and Personal Releases
"""

notes_file = os.path.join(KIT_DIR, "release_notes_v2.txt")
with open(notes_file, "w", encoding="utf-8") as f:
    f.write(notes)

rel_cmd = [
    "gh", "release", "create", "v2.0.0",
    ZIP_FILE,
    "--title", "Supreme Producer Suite v2.0.0 (32 Melodies + 62 Drum Loops)",
    "--notes-file", "release_notes_v2.txt"
]
run(rel_cmd)
