import os, shutil, sys

quarantine_root = r'C:\Users\alfaswz\Desktop\FL_Studio_Plugin_Cleanup_Backup_20260922'
db_backup_dir = os.path.join(quarantine_root, 'Plugin_Database_Full_Backup')
vst3_errors_dir = os.path.join(quarantine_root, 'VST3_Errors')
vst3_old_dir = os.path.join(quarantine_root, 'VST3_Old_Versions')
vst_x86_dir = os.path.join(quarantine_root, 'VST_x86_Duplicates')
fav_backup_dir = os.path.join(quarantine_root, 'Favorites_Duplicates_Backup')
inst_backup_dir = os.path.join(quarantine_root, 'Installed_Database_Backup')

for d in [quarantine_root, db_backup_dir, vst3_errors_dir, vst3_old_dir, vst_x86_dir, fav_backup_dir, inst_backup_dir]:
    os.makedirs(d, exist_ok=True)

log = []

def record(action, source, target=None, status="OK", detail=""):
    msg = f"[{status}] {action}: {source}"
    if target:
        msg += f" -> {target}"
    if detail:
        msg += f" ({detail})"
    print(msg)
    log.append(msg)

# 1. FULL BACKUP OF PLUGIN DATABASE
plugin_db_dir = r'C:\Users\alfaswz\Documents\Image-Line\FL Studio\Presets\Plugin database'
if os.path.exists(plugin_db_dir):
    try:
        for item in os.listdir(plugin_db_dir):
            s = os.path.join(plugin_db_dir, item)
            d = os.path.join(db_backup_dir, item)
            if os.path.isdir(s):
                if not os.path.exists(d):
                    shutil.copytree(s, d)
            else:
                shutil.copy2(s, d)
        record("FULL BACKUP", plugin_db_dir, db_backup_dir, "OK", "All presets and metadata backed up safely")
    except Exception as e:
        record("FULL BACKUP", plugin_db_dir, db_backup_dir, "WARN", str(e))

# Helper to safely move file or directory
def safe_move(src, dest_dir):
    if not os.path.exists(src):
        return
    base_name = os.path.basename(src)
    dest = os.path.join(dest_dir, base_name)
    try:
        if os.path.exists(dest):
            if os.path.isdir(dest):
                shutil.rmtree(dest)
            else:
                os.remove(dest)
        shutil.move(src, dest)
        record("QUARANTINE MOVE", src, dest, "OK")
    except Exception as e:
        record("QUARANTINE MOVE", src, dest, "ERROR", str(e))

# 2. VST3 ERROR PLUGINS
vst3_root = r'C:\Program Files\Common Files\VST3'
error_files = [
    r'C:\Program Files\Common Files\VST3\WaveShell1-VST3 15.0_x64.vst3',
    r'C:\Program Files\Common Files\VST3\WaveShell1-VST3 15.2_x64.vst3',
    r'C:\Program Files\Common Files\VST3\WaveShell1-VST3 15.3_x64.vst3',
    r'C:\Program Files\Common Files\VST3\WaveShell2-VST3 15.0_x64.vst3',
    r'C:\Program Files\Common Files\VST3\Slate Digital\Heatwave.vst3'
]
for p in error_files:
    safe_move(p, vst3_errors_dir)

# 3. OLD / REDUNDANT VST3 ITERATIONS
old_vst3 = [
    r'C:\Program Files\Common Files\VST3\AUTOMASTER SUPREME 2.vst3',
    r'C:\Program Files\Common Files\VST3\AUTOMASTER SUPREME 3.vst3',
    r'C:\Program Files\Common Files\VST3\AUTOMASTER SUPREME 3.1.vst3',
    r'C:\Program Files\Common Files\VST3\AUTOMASTER SUPREME 3.1.vst3.old',
    r'C:\Program Files\Common Files\VST3\AUTOMASTER SUPREME 3.2.vst3',
    r'C:\Program Files\Common Files\VST3\AUTOMASTER SUPREME 3.3.vst3',
    r'C:\Program Files\Common Files\VST3\Supreme Tuner BPM V.2',
    r'C:\Program Files\Common Files\VST3\Supreme Tuner BPM V.2.1',
    r'C:\Program Files\Common Files\VST3\Supreme Tuner BPM V.2.1.vst3',
    r'C:\Program Files\Common Files\VST3\Supreme Tuner BPM V.2.vst3',
    r'C:\Program Files\Common Files\VST3\SupremeTunerBPM',
    r'C:\Program Files\Common Files\VST3\SupremeTunerBPM.vst3',
    r'C:\Program Files\Common Files\VST3\TunerBPMPlugin',
    r'C:\Program Files\Common Files\VST3\TunerBPMPlugin.vst3'
]
for p in old_vst3:
    safe_move(p, vst3_old_dir)

# 4. VST3 DUPLICATES IN X86 VST FOLDERS
x86_dupes = [
    r'C:\Program Files (x86)\VstPlugins\TunerBPMPlugin.vst3',
    r'C:\Program Files (x86)\VstPlugins\SupremeTunerBPM.vst3',
    r'C:\Program Files (x86)\Steinberg\VstPlugins\TunerBPMPlugin.vst3'
]
for p in x86_dupes:
    safe_move(p, vst_x86_dir)

# 5. FAVORITES CLEANUP (Effects)
effects_dir = r'C:\Users\alfaswz\Documents\Image-Line\FL Studio\Presets\Plugin database\Effects'

# 5a. Remove literal duplicate S1 Imager Stereo_2.fst
s1_dup = os.path.join(effects_dir, 'S1 Imager Stereo_2.fst')
if os.path.exists(s1_dup):
    safe_move(s1_dup, fav_backup_dir)

# 5b. Old Automaster and Tuner favorites in Effects
old_favs = [
    'AUTOMASTER SUPREME 2.fst',
    'AUTOMASTER SUPREME 3.fst',
    'AUTOMASTER SUPREME 3.1.fst',
    'AUTOMASTER SUPREME 3.2.fst',
    'AUTOMASTER SUPREME 3.3.fst',
    'Supreme Tuner BPM.fst',
    'Supreme Tuner BPM V.2_2.fst'
]
for of in old_favs:
    p = os.path.join(effects_dir, of)
    if os.path.exists(p):
        safe_move(p, fav_backup_dir)

# 5c. Normalize _2 favorites where base doesn't exist (clean naming)
clean_renames = [
    ('CLA-3A Stereo_2.fst', 'CLA-3A Stereo.fst'),
    ('CLA-76 Stereo_2.fst', 'CLA-76 Stereo.fst'),
    ('DeEsser Stereo_2.fst', 'DeEsser Stereo.fst'),
    ('H-Comp Stereo_2.fst', 'H-Comp Stereo.fst'),
    ('L3 UltraMaximizer Stereo_2.fst', 'L3 UltraMaximizer Stereo.fst'),
    ('PuigChild 670 Stereo_2.fst', 'PuigChild 670 Stereo.fst'),
    ('RDeEsser Stereo_2.fst', 'RDeEsser Stereo.fst'),
    ('Vitamin Stereo_2.fst', 'Vitamin Stereo.fst'),
    ('Waves Tune Real-Time Stereo_2.fst', 'Waves Tune Real-Time Stereo.fst'),
    ('WLM Meter Stereo_2.fst', 'WLM Meter Stereo.fst'),
    ('Z-Noise Stereo_2.fst', 'Z-Noise Stereo.fst'),
    ('Supreme Tuner BPM V.2.2.fst', 'Supreme Tuner BPM V.2.2.fst'),
]

for old_name, clean_name in clean_renames:
    old_p = os.path.join(effects_dir, old_name)
    clean_p = os.path.join(effects_dir, clean_name)
    if os.path.exists(old_p):
        # Also backup a copy to quarantine
        shutil.copy2(old_p, os.path.join(fav_backup_dir, old_name))
        try:
            if os.path.exists(clean_p) and old_p != clean_p:
                os.remove(old_p)
                record("REMOVE REDUNDANT _2", old_p, status="OK")
            elif old_p != clean_p:
                os.rename(old_p, clean_p)
                record("RENAME TO CLEAN", old_p, clean_p, "OK")
        except Exception as e:
            record("RENAME", old_p, clean_p, "ERROR", str(e))

# Check VeviDynamicEQ1_2.fst in Fx subfolder
vevi_fx_2 = os.path.join(effects_dir, 'Fx', 'VeviDynamicEQ1_2.fst')
vevi_fx_clean = os.path.join(effects_dir, 'Fx', 'VeviDynamicEQ1.fst')
if os.path.exists(vevi_fx_2):
    shutil.copy2(vevi_fx_2, os.path.join(fav_backup_dir, 'VeviDynamicEQ1_2.fst'))
    try:
        if os.path.exists(vevi_fx_clean):
            os.remove(vevi_fx_2)
            record("REMOVE REDUNDANT _2", vevi_fx_2, status="OK")
        else:
            os.rename(vevi_fx_2, vevi_fx_clean)
            record("RENAME TO CLEAN", vevi_fx_2, vevi_fx_clean, "OK")
    except Exception as e:
        record("RENAME", vevi_fx_2, vevi_fx_clean, "ERROR", str(e))

# 6. INSTALLED VST3 DATABASE CLEANUP
inst_vst3_dir = r'C:\Users\alfaswz\Documents\Image-Line\FL Studio\Presets\Plugin database\Installed\Effects\VST3'
if os.path.exists(inst_vst3_dir):
    for f in os.listdir(inst_vst3_dir):
        # Match old Automaster, old Tuner, WaveShell 15, Heatwave
        should_quarantine = False
        lower_f = f.lower()
        if any(w in lower_f for w in ['waveshell1-vst3 15.', 'waveshell2-vst3 15.', 'heatwave']):
            should_quarantine = True
        elif 'automaster supreme' in lower_f and not 'automaster supreme 3.4' in lower_f:
            should_quarantine = True
        elif 'supreme tuner bpm' in lower_f and not 'supreme tuner bpm v.2.2' in lower_f:
            should_quarantine = True
        elif 'tunebpm' in lower_f or 'tunerbpmplugin' in lower_f or 'supremetunerbpm' in lower_f:
            should_quarantine = True
            
        if should_quarantine:
            src = os.path.join(inst_vst3_dir, f)
            safe_move(src, inst_backup_dir)

# Write summary log to quarantine root
log_file = os.path.join(quarantine_root, 'CLEANUP_EXECUTION_LOG.txt')
with open(log_file, 'w', encoding='utf-8') as lf:
    lf.write('\n'.join(log))

print('\nCleanup execution completed successfully! Log saved to:', log_file)
