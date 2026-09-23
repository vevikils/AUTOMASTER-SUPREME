import os
import sys
import shutil
import subprocess

def run_cmd(cmd, cwd=None):
    print(f"\n[EXEC] {cmd} in {cwd or '.'}")
    res = subprocess.run(cmd, shell=True, cwd=cwd)
    if res.returncode != 0:
        print(f"[ERROR] Command failed with code {res.returncode}")
        sys.exit(res.returncode)

def main():
    root_dir = os.path.dirname(os.path.abspath(__file__))
    build_dir = os.path.join(root_dir, "build")

    print("==========================================================")
    print(" BUILDING Supreme Tuner Real Time v3 (VST3 + Standalone)  ")
    print("==========================================================")

    if not os.path.exists(build_dir):
        os.makedirs(build_dir)
    
    # 1. Configure CMake
    cmake_gen = 'cmake -B build -G "Visual Studio 17 2022" -A x64'
    run_cmd(cmake_gen, cwd=root_dir)

    # 2. Build Release
    cmake_build = 'cmake --build build --config Release --parallel'
    run_cmd(cmake_build, cwd=root_dir)

    # 3. Artifact paths
    artefacts_dir = os.path.join(build_dir, "SupremeTunerRealTimeV3_artefacts", "Release")
    plugin_name = "Supreme Tuner Real Time v3.6"
    vst3_src = os.path.join(artefacts_dir, "VST3", f"{plugin_name}.vst3")
    exe_src = os.path.join(artefacts_dir, "Standalone", f"{plugin_name}.exe")

    # Destinations
    common_vst3_dir = r"C:\Program Files\Common Files\VST3"
    vst3_dst = os.path.join(common_vst3_dir, f"{plugin_name}.vst3")
    desktop_dst = os.path.expanduser(rf"~\Desktop\{plugin_name}.exe")

    print("\n---------------- DEPLOYING ----------------")
    if os.path.exists(vst3_src):
        print(f"Deploying VST3 from: {vst3_src}")
        print(f"Destination: {vst3_dst}")
        vst3_bin_src = os.path.join(vst3_src, "Contents", "x86_64-win", f"{plugin_name}.vst3")
        vst3_bin_dst = os.path.join(vst3_dst, "Contents", "x86_64-win", f"{plugin_name}.vst3")
        try:
            if not os.path.exists(vst3_dst):
                shutil.copytree(vst3_src, vst3_dst)
            else:
                try:
                    shutil.copy2(vst3_bin_src, vst3_bin_dst)
                except PermissionError:
                    old_path = vst3_bin_dst + ".old"
                    if os.path.exists(old_path):
                        try: os.remove(old_path)
                        except Exception: pass
                    os.rename(vst3_bin_dst, old_path)
                    shutil.copy2(vst3_bin_src, vst3_bin_dst)
            print("[SUCCESS] VST3 successfully installed to VST3 common directory!")
        except Exception as e:
            print(f"[ERROR] Failed to deploy VST3: {e}")
    else:
        print(f"[WARNING] VST3 build artifact not found at {vst3_src}")

    if os.path.exists(exe_src):
        print(f"\nDeploying Standalone from: {exe_src}")
        print(f"Destination: {desktop_dst}")
        try:
            shutil.copy2(exe_src, desktop_dst)
            print("[SUCCESS] Standalone .exe copied to Desktop for quick testing!")
        except Exception as e:
            print(f"[ERROR] Failed to copy standalone: {e}")
    else:
        print(f"[WARNING] Standalone executable not found at {exe_src}")

    print("\n==========================================================")
    print(" DEPLOYMENT COMPLETE: Supreme Tuner Real Time v3          ")
    print("==========================================================")

if __name__ == "__main__":
    main()
