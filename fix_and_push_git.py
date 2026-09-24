import os
import subprocess

KIT_DIR = r"c:\Users\alfaswz\.gemini\antigravity-ide\scratch\agentes-personalizados\SUPREME-PRODUCER-DRUM-KIT"

# 1. Create .gitignore
gitignore_path = os.path.join(KIT_DIR, ".gitignore")
with open(gitignore_path, "w", encoding="utf-8") as f:
    f.write("*.zip\nrelease_notes*.txt\n")

def run(cmd):
    print(f"\n--- Running: {' '.join(cmd) if isinstance(cmd, list) else cmd} ---")
    res = subprocess.run(cmd, cwd=KIT_DIR, capture_output=True, text=True, encoding="utf-8", shell=isinstance(cmd, str))
    print("STDOUT:", res.stdout.strip())
    if res.stderr.strip():
        print("STDERR:", res.stderr.strip())
    return res

# Reset the previous commit that included the zip
run(["git", "reset", "HEAD~1"])

# Stage .gitignore and all content files
run(["git", "add", ".gitignore"])
run(["git", "add", "01_808s_&_Basses", "02_Kicks", "03_Snares_&_Rims", "04_Claps", "05_HiHats_&_Cymbals"])
run(["git", "add", "06_Percussion", "07_FX_&_Chants", "08_Melody_Loops_(Auto_Tempo)"])
run(["git", "add", "08_Melody_Samples_32_(Hit_Inspired_Auto_Tempo)"])
run(["git", "add", "09_Drum_Loops_(Auto_Tempo)"])
run(["git", "add", "09_Drum_Loops_62_(Genre_Beats_Auto_Tempo)"])
run(["git", "add", "10_MIDI_Patterns", "assets", "cover.jpg", "install.bat", "install.ps1", "LICENSE", "README.md"])

# Remove any tracked zip
run(["git", "rm", "--cached", "*.zip"])

# Commit
run(["git", "commit", "-m", "feat: v2.0.0 - 32 hit-inspired complex melodies and 62 genre drum loops"])

# Push
res = run(["git", "push", "origin", "main"])

print("Git push completed.")
