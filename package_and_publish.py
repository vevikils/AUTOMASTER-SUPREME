import os
import zipfile
import subprocess

REPO_DIR = r"c:\Users\alfaswz\.gemini\antigravity-ide\scratch\agentes-personalizados\SUPREME-VOCAL-CHAINS-FL-STUDIO"
ZIP_PATH = os.path.join(REPO_DIR, "SUPREME-VOCAL-CHAINS-FL-STUDIO-v1.0.0.zip")

print("Creating ZIP file...")
# Zip all files in the repository (excluding any previous zip)
with zipfile.ZipFile(ZIP_PATH, "w", zipfile.ZIP_DEFLATED) as zipf:
    for root, dirs, files in os.walk(REPO_DIR):
        # Exclude .git folder and zip itself
        if ".git" in root:
            continue
        for file in files:
            if file.endswith(".zip"):
                continue
            full_path = os.path.join(root, file)
            rel_path = os.path.relpath(full_path, REPO_DIR)
            zipf.write(full_path, rel_path)
            print(f"  Added {rel_path} ({os.path.getsize(full_path)} bytes)")

print(f"ZIP package created successfully: {ZIP_PATH} ({os.path.getsize(ZIP_PATH)} bytes)")

def run(cmd, cwd=REPO_DIR):
    print(f"\n--- Running: {cmd} ---")
    res = subprocess.run(cmd, shell=True, cwd=cwd, capture_output=True, text=True)
    print("STDOUT:", res.stdout.strip())
    if res.stderr.strip():
        print("STDERR:", res.stderr.strip())
    return res

# 1. git init & commit
run("git init -b main")
run('git config user.name "vevikils"')
run('git config user.email "vevikils@users.noreply.github.com"')
run("git add assets cover.jpg Presets install.bat install.ps1 README.md LICENSE")
run('git commit -m "feat: initial release of Supreme Vocal Chains FL Studio pack"')

# 2. gh repo create
create_cmd = (
    'gh repo create SUPREME-VOCAL-CHAINS-FL-STUDIO --public --source=. '
    '--description "Ultra-Premium Artist Vocal Preset Pack for FL Studio (Travis Scott, Duki, Anuel AA, Bad Bunny, Ozuna) - 100% Stock Plugins" '
    '--push'
)
res = run(create_cmd)

# 3. Create release with ZIP
if os.path.exists(ZIP_PATH):
    rel_cmd = (
        'gh release create v1.0.0 SUPREME-VOCAL-CHAINS-FL-STUDIO-v1.0.0.zip '
        '--title "Supreme Vocal Chains FL Studio v1.0.0" '
        '--notes "🎙️ **Supreme Vocal Chains v1.0.0**\n\n'
        'Ultra-Premium vocal preset chains for FL Studio featuring Travis Scott, Duki, Anuel AA, Bad Bunny, and Ozuna.\n\n'
        '- **100% Stock FL Studio Plugins** (Zero 3rd party plugins required)\n'
        '- 10 Active Processing Slots per Chain\n'
        '- Includes 1-Click Windows Auto-Installer (`install.bat`)\n'
        '- Fully compatible with FL Studio 20, 21, and 24+"'
    )
    run(rel_cmd)
