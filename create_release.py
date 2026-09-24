import os
import subprocess

REPO_DIR = r"c:\Users\alfaswz\.gemini\antigravity-ide\scratch\agentes-personalizados\SUPREME-VOCAL-CHAINS-FL-STUDIO"
ZIP_FILE = "SUPREME-VOCAL-CHAINS-FL-STUDIO-v1.0.0.zip"

notes = """Supreme Vocal Chains FL Studio (Pro Artist Edition) v1.0.0

Ultra-Premium vocal preset chains for FL Studio featuring Travis Scott, Duki, Anuel AA, Bad Bunny, and Ozuna.

Key Features:
- 100% Stock FL Studio Plugins (Zero 3rd party plugins required)
- 10 Active Processing Slots per Chain
- Automated 1-Click Windows Installer (install.bat)
- High-resolution 3D Album Artwork included
- Compatible with FL Studio 20, 21, and 24+ (Windows & macOS)

Presets Included:
1. TRAVIS SCOTT - VOCAL CHAIN (PRO).fst
2. DUKI - SUPER ROBOTIC TRAP (PRO).fst
3. ANUEL AA - MELODIC TRAP (PRO).fst
4. BAD BUNNY - PRIME ERA (PRO).fst
5. OZUNA - VOZ AGUDA (PRO).fst
"""

notes_file = os.path.join(REPO_DIR, "release_notes.txt")
with open(notes_file, "w", encoding="utf-8") as f:
    f.write(notes)

cmd = [
    "gh", "release", "create", "v1.0.0",
    ZIP_FILE,
    "--title", "Supreme Vocal Chains FL Studio v1.0.0",
    "--notes-file", "release_notes.txt"
]

res = subprocess.run(cmd, cwd=REPO_DIR, capture_output=True, text=True, encoding="utf-8")
print("Return code:", res.returncode)
print("STDOUT:", res.stdout)
print("STDERR:", res.stderr)
