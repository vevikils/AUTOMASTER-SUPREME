import os
from drum_loop_engine import DrumGrooveRenderer

BASE_DIR = r"c:\Users\alfaswz\.gemini\antigravity-ide\scratch\agentes-personalizados\SUPREME-PRODUCER-DRUM-KIT\09_Drum_Loops_62_(Genre_Beats_Auto_Tempo)"

def build_trap_events(bpm, kick_pattern, full=True):
    events = []
    # 4 bars = 16 beats
    for bar in range(4):
        b = bar * 4.0
        # Clap on beat 3 (index 2.0)
        events.append(('c_trap', b + 2.0, 0.95, 0.0))
        # Hi-hats: 8th notes + roll in bar 2 & 4
        for step in range(8):
            pos = b + step * 0.5
            events.append(('h_c', pos, 0.65 if step % 2 == 0 else 0.48, -0.15 if step % 2 == 0 else 0.15))
        if bar % 2 == 1:
            for r in range(6):
                pos = b + 3.0 + r * (1.0 / 6.0)
                events.append(('h_c', pos, 0.55 + r * 0.06, 0.2))
        # Open hat on upbeat 1.5
        events.append(('h_o', b + 1.5, 0.5, -0.3))
        # Ghost snare
        events.append(('s_trap', b + 3.75, 0.45, 0.25))
        
    if full:
        # Add kicks
        for kp in kick_pattern:
            events.append(('k_trap', kp, 0.95, 0.0))
        # 808s on main bar starts
        for bp in [0.0, 4.0, 8.0, 12.0]:
            events.append(('808_spinz', bp, 0.85, 0.0))
    return events

def build_reggaeton_events(bpm, full=True, syncopated_perc=True):
    events = []
    for bar in range(4):
        b = bar * 4.0
        for beat in range(4):
            pos = b + beat
            if full:
                # 4-on-the-floor kick
                events.append(('k_dem', pos, 0.95, 0.0))
            # Dembow snare on beat + 0.75
            events.append(('s_reg', pos + 0.75, 0.88, 0.0))
            # Dembow second snare on 2nd and 4th beats + 0.5
            if beat % 2 == 1:
                events.append(('s_reg', pos - 0.5, 0.82, -0.1))
            # Shaker 16ths
            for s in range(4):
                events.append(('shaker', pos + s * 0.25, 0.45 if s % 2 == 0 else 0.32, 0.25 if s % 2 == 0 else -0.25))
        if syncopated_perc:
            events.append(('bongo', b + 1.25, 0.55, -0.35))
            events.append(('bongo', b + 3.25, 0.55, 0.35))
    return events

def build_drill_events(bpm, kick_pattern, full=True):
    events = []
    for bar in range(4):
        b = bar * 4.0
        # Drill snare on beat 3 (beat 2.0 in half time)
        events.append(('s_drill', b + 2.0, 0.95, 0.0))
        # Ghost snare
        events.append(('s_drill', b + 3.66, 0.5, 0.2))
        # Drill hi-hat bounce (steps 0, 0.75, 1.5, 2.0, 2.75, 3.5)
        for s in [0.0, 0.75, 1.5, 2.0, 2.75, 3.5]:
            events.append(('h_drill', b + s, 0.65, -0.15 if s < 2.0 else 0.15))
        # Open hat on 1.75
        events.append(('h_o', b + 1.75, 0.45, -0.25))
    if full:
        for kp in kick_pattern:
            events.append(('k_drill', kp, 0.95, 0.0))
        # Sliding 808
        events.append(('808_drill', 0.0, 0.85, 0.0))
        events.append(('808_drill', 8.0, 0.85, 0.0))
    return events

def build_boombap_events(bpm, full=True):
    events = []
    for bar in range(4):
        b = bar * 4.0
        # Snare on 2 and 4
        events.append(('s_boom', b + 1.0, 0.92, 0.0))
        events.append(('s_boom', b + 3.0, 0.92, 0.0))
        # Swung 16th hats
        for s in range(8):
            swing_offset = 0.03 if s % 2 == 1 else 0.0
            events.append(('h_c', b + s * 0.5 + swing_offset, 0.6 if s % 2 == 0 else 0.42, -0.2 if s % 2 == 0 else 0.2))
        events.append(('h_o', b + 3.5, 0.45, 0.3))
        if full:
            # 90s classic kick groove
            events.append(('k_boom', b + 0.0, 0.95, 0.0))
            events.append(('k_boom', b + 1.75, 0.85, 0.0))
            events.append(('k_boom', b + 2.5, 0.9, 0.0))
    return events

def build_afro_events(bpm, full=True):
    events = []
    for bar in range(4):
        b = bar * 4.0
        # 3:2 Clave rimshot accents
        rim_positions = [0.75, 1.5, 2.25, 3.0, 3.75]
        for rp in rim_positions:
            events.append(('rim', b + rp, 0.85, 0.1))
        # Shakers 16ths continuously
        for s in range(16):
            events.append(('shaker', b + s * 0.25, 0.48 if s % 2 == 0 else 0.35, 0.25 if s % 4 == 0 else -0.25))
        # Congas / bongos
        events.append(('bongo', b + 0.5, 0.6, -0.3))
        events.append(('bongo', b + 2.0, 0.65, 0.3))
        if full:
            events.append(('k_afro', b + 0.0, 0.95, 0.0))
            events.append(('k_afro', b + 1.75, 0.85, 0.0))
            events.append(('k_afro', b + 2.5, 0.9, 0.0))
            events.append(('808_sub', b + 0.0, 0.8, 0.0))
    return events

def generate_all():
    print("=== RENDERING 62 HIT-INSPIRED DRUM LOOPS (AUTO-TEMPO) ===")
    renderer = DrumGrooveRenderer()

    # 1. TRAP (12 Loops)
    dir_trap = os.path.join(BASE_DIR, "01_Trap_Loops_(12)")
    os.makedirs(dir_trap, exist_ok=True)
    trap_configs = [
        (140.0, "Astroworld_Sicko_Bounce", [0.0, 1.5, 2.75, 4.0, 5.5, 6.75, 7.25, 8.0, 9.5, 10.75, 12.0, 13.5, 14.75, 15.25]),
        (138.0, "Metro_Superhero_Hard", [0.0, 1.75, 3.0, 4.0, 5.5, 7.0, 8.0, 9.75, 11.0, 12.0, 13.5, 15.0]),
        (132.0, "Baby_Gunna_Drip_Bounce", [0.0, 1.5, 2.5, 4.0, 5.5, 6.5, 8.0, 9.5, 10.5, 12.0, 13.5, 14.5]),
        (144.0, "Juice_Lucid_Trap", [0.0, 1.25, 2.5, 4.0, 5.5, 7.25, 8.0, 9.25, 10.5, 12.0, 13.5, 15.25]),
        (136.0, "Drake_21_Rich_Flex", [0.0, 1.5, 2.75, 4.0, 5.75, 7.0, 8.0, 9.5, 10.75, 12.0, 13.75, 15.0]),
        (130.0, "Future_Mask_Off_Rolls", [0.0, 1.5, 3.0, 4.0, 5.5, 6.75, 8.0, 9.5, 11.0, 12.0, 13.5, 14.75])
    ]
    for bpm, name, kicks in trap_configs:
        ev_full = build_trap_events(bpm, kicks, full=True)
        ev_top = build_trap_events(bpm, kicks, full=False)
        renderer.render_loop(bpm, 4, ev_full, f"Trap_Loop_{int(bpm)}BPM_{name}_Full.wav", dir_trap, f"Trap {name} Full ({int(bpm)} BPM)")
        renderer.render_loop(bpm, 4, ev_top, f"Trap_Loop_{int(bpm)}BPM_{name}_Top_Loop.wav", dir_trap, f"Trap {name} Top Loop ({int(bpm)} BPM)")

    # 2. REGGAETON (12 Loops)
    dir_reg = os.path.join(BASE_DIR, "02_Reggaeton_Loops_(12)")
    os.makedirs(dir_reg, exist_ok=True)
    reg_configs = [
        (96.0, "BadBunny_Titi_Dembow"),
        (92.0, "Feid_Ferxxo_Crisp"),
        (98.0, "Rauw_TodoDeTi_Bounce"),
        (95.0, "Monaco_Heavy_Dembow"),
        (90.0, "Dakiti_Smooth_Groove"),
        (102.0, "Gasolina_Classic_Perreo")
    ]
    for bpm, name in reg_configs:
        ev_full = build_reggaeton_events(bpm, full=True)
        ev_top = build_reggaeton_events(bpm, full=False)
        renderer.render_loop(bpm, 4, ev_full, f"Reggaeton_Loop_{int(bpm)}BPM_{name}_Full.wav", dir_reg, f"Reggaeton {name} Full ({int(bpm)} BPM)")
        renderer.render_loop(bpm, 4, ev_top, f"Reggaeton_Loop_{int(bpm)}BPM_{name}_Top.wav", dir_reg, f"Reggaeton {name} Top Loop ({int(bpm)} BPM)")

    # 3. DRILL (12 Loops)
    dir_drill = os.path.join(BASE_DIR, "03_Drill_Loops_(12)")
    os.makedirs(dir_drill, exist_ok=True)
    drill_configs = [
        (142.0, "PopSmoke_Dior_Slide", [0.0, 1.75, 4.0, 5.75, 7.25, 8.0, 9.75, 12.0, 13.75, 15.25]),
        (140.0, "CentralCee_Doja_Bounce", [0.0, 1.5, 4.0, 5.5, 7.0, 8.0, 9.5, 12.0, 13.5, 15.0]),
        (144.0, "Fivio_BigDrip_Aggressive", [0.0, 1.75, 3.25, 4.0, 5.75, 8.0, 9.75, 11.25, 12.0, 13.75]),
        (141.0, "Russ_Body_UK", [0.0, 1.5, 4.0, 5.75, 7.25, 8.0, 9.5, 12.0, 13.75, 15.25]),
        (143.0, "Morad_Spanish_Drill", [0.0, 1.75, 4.0, 5.5, 7.0, 8.0, 9.75, 12.0, 13.5, 15.0]),
        (146.0, "NY_Fast_Drill_Stomp", [0.0, 1.5, 3.5, 4.0, 5.5, 7.5, 8.0, 9.5, 11.5, 12.0, 13.5, 15.5])
    ]
    for bpm, name, kicks in drill_configs:
        ev_full = build_drill_events(bpm, kicks, full=True)
        ev_top = build_drill_events(bpm, kicks, full=False)
        renderer.render_loop(bpm, 4, ev_full, f"Drill_Loop_{int(bpm)}BPM_{name}_Full.wav", dir_drill, f"Drill {name} Full ({int(bpm)} BPM)")
        renderer.render_loop(bpm, 4, ev_top, f"Drill_Loop_{int(bpm)}BPM_{name}_Top_Loop.wav", dir_drill, f"Drill {name} Top Loop ({int(bpm)} BPM)")

    # 4. HIP-HOP & BOOMBAP (12 Loops)
    dir_hip = os.path.join(BASE_DIR, "04_HipHop_Loops_(12)")
    os.makedirs(dir_hip, exist_ok=True)
    hip_configs = [
        (100.0, "Kendrick_NotLikeUs_WestCoast"),
        (94.0, "Dre_StillDRE_Classic"),
        (88.0, "Jole_NoRoleModelz_Vinyl"),
        (92.0, "Asap_PraiseTheLord_Bounce"),
        (85.0, "MacMiller_SelfCare_Laidback"),
        (90.0, "BoomBap_90s_GoldenEra")
    ]
    for bpm, name in hip_configs:
        ev_full = build_boombap_events(bpm, full=True)
        ev_top = build_boombap_events(bpm, full=False)
        renderer.render_loop(bpm, 4, ev_full, f"HipHop_Loop_{int(bpm)}BPM_{name}_Full.wav", dir_hip, f"HipHop {name} Full ({int(bpm)} BPM)")
        renderer.render_loop(bpm, 4, ev_top, f"HipHop_Loop_{int(bpm)}BPM_{name}_Top_Loop.wav", dir_hip, f"HipHop {name} Top Loop ({int(bpm)} BPM)")

    # 5. AFRO-URBAN & R&B (14 Loops)
    dir_afro = os.path.join(BASE_DIR, "05_Afro_RnB_Loops_(14)")
    os.makedirs(dir_afro, exist_ok=True)
    afro_configs = [
        (100.0, "Rema_CalmDown_Afrobeats"),
        (104.0, "Burna_LastLast_Groove"),
        (108.0, "Wizkid_Essence_Warm"),
        (112.0, "Amapiano_LogDrum_Bounce"),
        (120.0, "Weeknd_BlindingLights_80s"),
        (86.0, "SZA_KillBill_Smooth"),
        (95.0, "Modern_TrapSoul_Chill")
    ]
    for bpm, name in afro_configs:
        ev_full = build_afro_events(bpm, full=True)
        ev_top = build_afro_events(bpm, full=False)
        renderer.render_loop(bpm, 4, ev_full, f"Afro_RnB_Loop_{int(bpm)}BPM_{name}_Full.wav", dir_afro, f"Afro/R&B {name} Full ({int(bpm)} BPM)")
        renderer.render_loop(bpm, 4, ev_top, f"Afro_RnB_Loop_{int(bpm)}BPM_{name}_Top_Loop.wav", dir_afro, f"Afro/R&B {name} Top Loop ({int(bpm)} BPM)")

    print("[SUCCESS] All 62 Drum Loops Rendered Across 5 Genres!")

if __name__ == "__main__":
    generate_all()
