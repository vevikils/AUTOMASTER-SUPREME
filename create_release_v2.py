import os
import subprocess

KIT_DIR = r"c:\Users\alfaswz\.gemini\antigravity-ide\scratch\agentes-personalizados\SUPREME-PRODUCER-DRUM-KIT"
ZIP_FILE = "SUPREME-PRODUCER-SUITE-v2.0.0.zip"

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

cmd = [
    "gh", "release", "create", "v2.0.0",
    ZIP_FILE,
    "--title", "Supreme Producer Suite v2.0.0 (32 Melodies + 62 Drum Loops)",
    "--notes-file", "release_notes_v2.txt"
]

print("Publishing GitHub Release v2.0.0...")
res = subprocess.run(cmd, cwd=KIT_DIR, capture_output=True, text=True, encoding="utf-8")
print("Return code:", res.returncode)
print("STDOUT:", res.stdout.strip())
if res.stderr.strip():
    print("STDERR:", res.stderr.strip())
