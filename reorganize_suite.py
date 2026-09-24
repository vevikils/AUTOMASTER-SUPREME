import os
import shutil

KIT_DIR = r"c:\Users\alfaswz\.gemini\antigravity-ide\scratch\agentes-personalizados\SUPREME-PRODUCER-DRUM-KIT"

# 1. Clean up old repetitive/generic loop folders
old_folders = [
    os.path.join(KIT_DIR, "08_Melody_Loops_(Auto_Tempo)"),
    os.path.join(KIT_DIR, "09_Drum_Loops_(Auto_Tempo)"),
    os.path.join(KIT_DIR, "09_Drum_Loops_62_(Genre_Beats_Auto_Tempo)")
]

for f in old_folders:
    if os.path.exists(f):
        print(f"Removing old generic folder: {f}")
        shutil.rmtree(f, ignore_errors=True)

# 2. Check structure
print("\nActive Folders in Supreme Producer Suite:")
for item in sorted(os.listdir(KIT_DIR)):
    p = os.path.join(KIT_DIR, item)
    if os.path.isdir(p) and not item.startswith("."):
        count = sum(len(files) for _, _, files in os.walk(p))
        print(f"  [Folder] {item} ({count} files)")

print("\nReorganization complete.")
