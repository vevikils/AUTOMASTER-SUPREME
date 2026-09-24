import os
import shutil
import numpy as np
from wav_writer import write_wav
import generators as gen
import make_loops as loops
from make_midis import generate_all_midis

from scipy import signal
from dsp_utils import butter_bandpass, SR

KIT_DIR = r"c:\Users\alfaswz\.gemini\antigravity-ide\scratch\agentes-personalizados\SUPREME-PRODUCER-DRUM-KIT"
COVER_SRC = r"C:\Users\alfaswz\.gemini\antigravity-ide\brain\6a240be5-03c7-4d57-9466-576318416996\drum_kit_cover_1790098643061.jpg"

def build():
    print("=== BUILDING SUPREME PRODUCER DRUM KIT ===")
    os.makedirs(KIT_DIR, exist_ok=True)
    
    # 0. Copy Cover
    os.makedirs(os.path.join(KIT_DIR, "assets"), exist_ok=True)
    shutil.copy2(COVER_SRC, os.path.join(KIT_DIR, "assets", "cover.jpg"))
    shutil.copy2(COVER_SRC, os.path.join(KIT_DIR, "cover.jpg"))
    print("[+] Cover image copied.")

    # Dictionary to hold one-shots for drum loop creation
    one_shots = {}

    # 1. 808s & Basses
    dir_808 = os.path.join(KIT_DIR, "01_808s_&_Basses")
    print("[*] Generating 808s & Basses (Tuned to C)...")
    s808_spinz = gen.make_808(duration=2.2, fund=32.703, drive=2.8, decay=1.8)
    one_shots['808_spinz'] = s808_spinz
    write_wav(os.path.join(dir_808, "808_Spinz_Classic_C.wav"), s808_spinz, title="808 Spinz Classic (C)")
    
    s808_sub = gen.make_808(duration=2.5, fund=32.703, attack_pitch=110.0, drive=1.5, decay=2.1)
    write_wav(os.path.join(dir_808, "808_Sub_Heavyweight_C.wav"), s808_sub, title="808 Sub Heavyweight (C)")
    
    s808_drill = gen.make_808(duration=2.2, fund=32.703, drive=3.5, decay=1.7, is_slide=True)
    one_shots['808_drill'] = s808_drill
    write_wav(os.path.join(dir_808, "808_Drill_Slide_Distorted_C.wav"), s808_drill, title="808 Drill Slide Distorted (C)")
    
    s808_pluck = gen.make_808(duration=1.2, fund=65.406, attack_pitch=220.0, drive=2.2, decay=0.85)
    write_wav(os.path.join(dir_808, "808_Pluck_Punchy_C.wav"), s808_pluck, title="808 Pluck Punchy (C)")
    
    s808_cyber = gen.make_808(duration=2.0, fund=32.703, drive=3.0, decay=1.6)
    write_wav(os.path.join(dir_808, "808_Cyber_Sub_C.wav"), s808_cyber, title="808 Cyber Sub (C)")

    # 2. Kicks
    dir_kicks = os.path.join(KIT_DIR, "02_Kicks")
    print("[*] Generating Kicks...")
    k_trap = gen.make_kick(duration=0.45, start_f=210.0, end_f=52.0, click_amt=0.4)
    one_shots['kick_trap'] = k_trap
    write_wav(os.path.join(dir_kicks, "Kick_Hard_Hitter_Trap.wav"), k_trap, title="Kick Hard Hitter Trap")
    
    k_ac = gen.make_kick(duration=0.48, start_f=160.0, end_f=48.0, click_amt=0.25, punch_decay=0.16)
    write_wav(os.path.join(dir_kicks, "Kick_Acoustic_Layered.wav"), k_ac, title="Kick Acoustic Layered")
    
    k_dem = gen.make_kick(duration=0.38, start_f=230.0, end_f=58.0, click_amt=0.35, punch_decay=0.10)
    one_shots['kick_dembow'] = k_dem
    write_wav(os.path.join(dir_kicks, "Kick_Dembow_Punch_Latin.wav"), k_dem, title="Kick Dembow Punch Latin")
    
    k_drill = gen.make_kick(duration=0.35, start_f=260.0, end_f=55.0, click_amt=0.5, punch_decay=0.08)
    one_shots['kick_drill'] = k_drill
    write_wav(os.path.join(dir_kicks, "Kick_Drill_Knock.wav"), k_drill, title="Kick Drill Knock")
    
    k_sub = gen.make_kick(duration=0.55, start_f=130.0, end_f=44.0, click_amt=0.15, punch_decay=0.20)
    write_wav(os.path.join(dir_kicks, "Kick_Sub_Thump.wav"), k_sub, title="Kick Sub Thump")

    # 3. Snares & Rims
    dir_snares = os.path.join(KIT_DIR, "03_Snares_&_Rims")
    print("[*] Generating Snares & Rims...")
    s_trap = gen.make_snare(duration=0.35, tone_f=185.0, noise_decay=0.22)
    one_shots['snare_trap'] = s_trap
    write_wav(os.path.join(dir_snares, "Snare_Trap_Classic_Smack.wav"), s_trap, title="Snare Trap Classic Smack")
    
    s_drill = gen.make_snare(duration=0.28, tone_f=240.0, noise_decay=0.16, snappy_lo=1200.0, snappy_hi=7500.0, tone_amt=0.5)
    one_shots['snare_drill'] = s_drill
    write_wav(os.path.join(dir_snares, "Snare_Drill_Ghost_Metallic.wav"), s_drill, title="Snare Drill Ghost Metallic")
    
    s_reg = gen.make_snare(duration=0.32, tone_f=210.0, noise_decay=0.19, snappy_lo=900.0, snappy_hi=6500.0, tone_amt=0.45)
    one_shots['snare_reggaeton'] = s_reg
    write_wav(os.path.join(dir_snares, "Snare_Reggaeton_Dembow.wav"), s_reg, title="Snare Reggaeton Dembow")
    
    rim = gen.make_rimshot(duration=0.12, freq=980.0)
    write_wav(os.path.join(dir_snares, "Rimshot_Clean_Wood.wav"), rim, title="Rimshot Clean Wood")
    
    s_fusion = gen.make_snare(duration=0.38, tone_f=195.0, noise_decay=0.26, tone_amt=0.3)
    write_wav(os.path.join(dir_snares, "Snare_Layer_Clap_Fusion.wav"), s_fusion, title="Snare Layer Clap Fusion")

    # 4. Claps
    dir_claps = os.path.join(KIT_DIR, "04_Claps")
    print("[*] Generating Claps...")
    c_trap = gen.make_clap(duration=0.4, pre_bursts=3, burst_space_ms=13.0)
    one_shots['clap_trap'] = c_trap
    write_wav(os.path.join(dir_claps, "Clap_Trap_Crisp.wav"), c_trap, title="Clap Trap Crisp")
    
    c_wide = gen.make_clap(duration=0.42, pre_bursts=4, burst_space_ms=11.0, is_stereo=True)
    write_wav(os.path.join(dir_claps, "Clap_Stadium_Wide.wav"), c_wide, title="Clap Stadium Wide")
    
    c_short = gen.make_clap(duration=0.25, pre_bursts=2, burst_space_ms=9.0)
    write_wav(os.path.join(dir_claps, "Clap_Short_Dry.wav"), c_short, title="Clap Short Dry")
    
    c_vintage = gen.make_clap(duration=0.38, pre_bursts=3, burst_space_ms=15.0)
    write_wav(os.path.join(dir_claps, "Clap_Vintage_Analog.wav"), c_vintage, title="Clap Vintage Analog")

    # 5. Hi-Hats & Cymbals
    dir_hats = os.path.join(KIT_DIR, "05_HiHats_&_Cymbals")
    print("[*] Generating Hi-Hats & Cymbals...")
    h_c = gen.make_hihat(duration=0.065, is_open=False, decay=0.035)
    one_shots['hat_closed'] = h_c
    write_wav(os.path.join(dir_hats, "HiHat_Closed_Crisp_Clean.wav"), h_c, title="HiHat Closed Crisp Clean")
    
    h_drill = gen.make_hihat(duration=0.045, is_open=False, decay=0.022)
    write_wav(os.path.join(dir_hats, "HiHat_Closed_Metallic_Drill.wav"), h_drill, title="HiHat Closed Metallic Drill")
    
    h_o = gen.make_hihat(duration=0.55, is_open=True, decay=0.35)
    one_shots['hat_open'] = h_o
    write_wav(os.path.join(dir_hats, "HiHat_Open_Long_Sustain.wav"), h_o, title="HiHat Open Long Sustain")
    
    h_pedal = gen.make_hihat(duration=0.25, is_open=True, decay=0.12)
    write_wav(os.path.join(dir_hats, "HiHat_Open_Pedal_Choke.wav"), h_pedal, title="HiHat Open Pedal Choke")
    
    crash = gen.make_crash(duration=2.2)
    write_wav(os.path.join(dir_hats, "Crash_Cymbal_Impact.wav"), crash, title="Crash Cymbal Impact")

    # 6. Percussion
    dir_percs = os.path.join(KIT_DIR, "06_Percussion")
    print("[*] Generating Percussion...")
    cowbell = gen.make_cowbell(duration=0.32)
    write_wav(os.path.join(dir_percs, "Perc_Phonk_Cowbell.wav"), cowbell, title="Perc Phonk Cowbell")
    
    wood = gen.make_woodblock(duration=0.06)
    write_wav(os.path.join(dir_percs, "Perc_Woodblock_Organic.wav"), wood, title="Perc Woodblock Organic")
    
    pipe = gen.make_metal_pipe(duration=0.45)
    write_wav(os.path.join(dir_percs, "Perc_Metal_Pipe_Hit.wav"), pipe, title="Perc Metal Pipe Hit")
    
    bongo = gen.make_bongo(duration=0.25)
    write_wav(os.path.join(dir_percs, "Perc_Bongo_Conga_Slap.wav"), bongo, title="Perc Bongo Conga Slap")
    
    shaker = gen.make_shaker(duration=0.14)
    one_shots['perc_shaker'] = shaker
    write_wav(os.path.join(dir_percs, "Perc_Shaker_Loop_Hit.wav"), shaker, title="Perc Shaker Loop Hit")

    # 7. FX & Chants
    dir_fx = os.path.join(KIT_DIR, "07_FX_&_Chants")
    print("[*] Generating FX & Chants...")
    hey = gen.make_chant_hey(duration=0.32)
    write_wav(os.path.join(dir_fx, "FX_Vocal_Chant_Hey.wav"), hey, title="FX Vocal Chant Hey")
    
    riser = gen.make_riser(duration=6.857, bpm=140)
    write_wav(os.path.join(dir_fx, "FX_Riser_4Bar_Sweep_140BPM.wav"), riser, tempo=140.0, num_beats=16, title="FX Riser 4-Bar Sweep (140 BPM)")
    
    subdrop = gen.make_sub_drop(duration=2.5)
    write_wav(os.path.join(dir_fx, "FX_Sub_Drop_Impact.wav"), subdrop, title="FX Sub Drop Impact")
    
    laser = gen.make_laser(duration=0.18)
    write_wav(os.path.join(dir_fx, "FX_Laser_Zap_Trap.wav"), laser, title="FX Laser Zap Trap")

    # Vinyl texture
    np.random.seed(333)
    t_v = np.linspace(0, 3.0, int(SR * 3.0), endpoint=False)
    v_noise = np.random.uniform(-1, 1, len(t_v))
    b, a = butter_bandpass(400, 4500, SR)
    v_crack = signal.lfilter(b, a, v_noise) * 0.15
    # Add random clicks
    for _ in range(60):
        pos = np.random.randint(0, len(t_v) - 50)
        v_crack[pos:pos+50] += np.random.uniform(-0.8, 0.8, 50)
    write_wav(os.path.join(dir_fx, "FX_Vinyl_Crackling_Texture.wav"), v_crack, title="FX Vinyl Crackling Texture")

    # 8. Melody Loops (With ACID chunks for Auto-Tempo!)
    dir_mel = os.path.join(KIT_DIR, "08_Melody_Loops_(Auto_Tempo)")
    print("[*] Generating Melody Loops (WAV with ACID Chunks)...")
    m1, bpm1, beats1, root1 = loops.make_melody_astroworld_140()
    write_wav(os.path.join(dir_mel, "Melody_140BPM_Cm_Astroworld_Space_Synth.wav"), m1, tempo=bpm1, num_beats=beats1, root_note=root1, title="Astroworld Space Synth (140 BPM Cm)")
    
    m2, bpm2, beats2, root2 = loops.make_melody_tokyo_pluck_130()
    write_wav(os.path.join(dir_mel, "Melody_130BPM_Am_Tokyo_Night_Pluck.wav"), m2, tempo=bpm2, num_beats=beats2, root_note=root2, title="Tokyo Night Pluck (130 BPM Am)")
    
    m3, bpm3, beats3, root3 = loops.make_melody_reggaeton_96()
    write_wav(os.path.join(dir_mel, "Melody_96BPM_Fm_Latin_Reggaeton_Vibes.wav"), m3, tempo=bpm3, num_beats=beats3, root_note=root3, title="Latin Reggaeton Vibes (96 BPM Fm)")
    
    m4, bpm4, beats4, root4 = loops.make_melody_drill_piano_142()
    write_wav(os.path.join(dir_mel, "Melody_142BPM_Em_Dark_Drill_Piano.wav"), m4, tempo=bpm4, num_beats=beats4, root_note=root4, title="Dark Drill Piano (142 BPM Em)")

    # 9. Drum Loops (With ACID chunks for Auto-Tempo!)
    dir_dloops = os.path.join(KIT_DIR, "09_Drum_Loops_(Auto_Tempo)")
    print("[*] Generating Drum Loops (WAV with ACID Chunks)...")
    dloops = loops.make_drum_loops(one_shots)
    
    d1, bpm_d1, beats_d1 = dloops['trap_full_140']
    write_wav(os.path.join(dir_dloops, "DrumLoop_140BPM_Full_Trap_Bounce.wav"), d1, tempo=bpm_d1, num_beats=beats_d1, title="Trap Full Bounce (140 BPM)")
    
    d2, bpm_d2, beats_d2 = dloops['trap_top_140']
    write_wav(os.path.join(dir_dloops, "DrumLoop_140BPM_Top_Loop.wav"), d2, tempo=bpm_d2, num_beats=beats_d2, title="Trap Top Loop (140 BPM)")
    
    d3, bpm_d3, beats_d3 = dloops['reggaeton_96']
    write_wav(os.path.join(dir_dloops, "DrumLoop_96BPM_Reggaeton_Perreo_Beat.wav"), d3, tempo=bpm_d3, num_beats=beats_d3, title="Reggaeton Perreo Dembow (96 BPM)")
    
    d4, bpm_d4, beats_d4 = dloops['drill_142']
    write_wav(os.path.join(dir_dloops, "DrumLoop_142BPM_UK_Drill_Groove.wav"), d4, tempo=bpm_d4, num_beats=beats_d4, title="UK Drill Groove (142 BPM)")

    # 10. MIDI Patterns
    dir_midi = os.path.join(KIT_DIR, "10_MIDI_Patterns")
    print("[*] Generating MIDI Patterns...")
    generate_all_midis(dir_midi)

    print("[SUCCESS] All sound assets synthesized and saved successfully!")

if __name__ == "__main__":
    build()
