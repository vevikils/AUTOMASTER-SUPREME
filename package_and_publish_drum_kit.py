import os
import zipfile
import subprocess

KIT_DIR = r"c:\Users\alfaswz\.gemini\antigravity-ide\scratch\agentes-personalizados\SUPREME-PRODUCER-DRUM-KIT"
ZIP_FILE = "SUPREME-PRODUCER-DRUM-KIT-v1.0.0.zip"
ZIP_PATH = os.path.join(KIT_DIR, ZIP_FILE)

print("Creating ZIP file...")
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
            print(f"  Added {rel_path} ({os.path.getsize(full_path)} bytes)")

zip_size_mb = os.path.getsize(ZIP_PATH) / (1024 * 1024)
print(f"ZIP package created: {ZIP_PATH} ({zip_size_mb:.2f} MB)")

def run(cmd, cwd=KIT_DIR):
    print(f"\n--- Running: {' '.join(cmd) if isinstance(cmd, list) else cmd} ---")
    res = subprocess.run(cmd, cwd=cwd, capture_output=True, text=True, encoding="utf-8", shell=isinstance(cmd, str))
    print("STDOUT:", res.stdout.strip())
    if res.stderr.strip():
        print("STDERR:", res.stderr.strip())
    return res

# 1. git init & commit
run(["git", "init", "-b", "main"])
run(["git", "config", "user.name", "vevikils"])
run(["git", "config", "user.email", "vevikils@users.noreply.github.com"])
run(["git", "add", "."])
run(["git", "commit", "-m", "feat: initial release of Supreme Producer Drum Kit & Melody Suite"])

# 2. gh repo create
create_cmd = [
    "gh", "repo", "create", "SUPREME-PRODUCER-DRUM-KIT-FL-STUDIO",
    "--public",
    "--source=.",
    "--description", "Supreme Producer Drum Kit & Melody Suite for FL Studio & Any DAW (808s, Kicks, Snares, HiHats, Claps, Percs, Auto-Tempo Loops, MIDIs) - 100% Royalty Free",
    "--push"
]
run(create_cmd)

# 3. Create release notes and publish release
notes = """Supreme Producer Drum Kit & Melody Suite v1.0.0

A complete, high-definition sound library for FL Studio, Ableton Live, Logic Pro, and all DAWs.

Contents:
- 52 High-Definition WAV & MIDI files
- 01 808s & Basses (Tuned to C for effortless melody pitching)
- 02 Kicks (Punchy transients, sub-cutting punch)
- 03 Snares & Rims (Acoustic, Trap, and Drill)
- 04 Claps (Multi-burst and wide stereo spreads)
- 05 Hi-Hats & Cymbals (Closed, Open, Crash, Sizzle)
- 06 Percussion (Phonk Cowbell, Woodblock, Bongos, Shakers)
- 07 FX & Chants (Vocal Vox 'Hey!', 4-Bar Risers, Sub Drops, Lasers, Vinyl Textures)
- 08 Melody Loops (ACIDized WAV with Auto-Tempo Time-Stretching & BPM/Key tags)
- 09 Drum Loops (Full Trap, Top Loops, Reggaeton Dembow, UK Drill)
- 10 MIDI Patterns (Hi-Hat rolls, 808 basslines, chord progressions, dembow grooves)
- 1-Click Windows Auto-Installer (install.bat)
- 100% Royalty-Free for Commercial & Personal Releases
"""

notes_file = os.path.join(KIT_DIR, "release_notes.txt")
with open(notes_file, "w", encoding="utf-8") as f:
    f.write(notes)

rel_cmd = [
    "gh", "release", "create", "v1.0.0",
    ZIP_FILE,
    "--title", "Supreme Producer Drum Kit v1.0.0",
    "--notes-file", "release_notes.txt"
]
run(rel_cmd)
