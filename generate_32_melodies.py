import os
from melody_renderer import render_melody

OUT_DIR = r"c:\Users\alfaswz\.gemini\antigravity-ide\scratch\agentes-personalizados\SUPREME-PRODUCER-DRUM-KIT\08_Melody_Samples_32_(Hit_Inspired_Auto_Tempo)"
os.makedirs(OUT_DIR, exist_ok=True)

def generate_all():
    print("=== RENDERING 32 HIT-INSPIRED MELODY SAMPLES (AUTO-TEMPO) ===")
    
    # ------------------ TRAP (1 - 8) ------------------
    # 1. Travis Scott - Goosebumps (Cm, 140)
    render_melody(
        bpm=140.0, num_bars=4,
        chord_events=[
            (0.0, 3.8, [60, 63, 67], 'pad', 0.8), (4.0, 3.8, [56, 60, 63], 'pad', 0.8),
            (8.0, 3.8, [63, 67, 70], 'pad', 0.8), (12.0, 3.8, [58, 62, 65], 'pad', 0.8)
        ],
        lead_events=[
            (0.0, 1.5, 72, 'flute', 0.9), (2.0, 1.5, 70, 'flute', 0.85),
            (4.0, 1.5, 68, 'flute', 0.9), (6.0, 1.5, 67, 'flute', 0.85),
            (8.0, 1.5, 75, 'flute', 0.95), (10.0, 1.5, 74, 'flute', 0.9),
            (12.0, 1.5, 72, 'flute', 0.85), (14.0, 1.5, 70, 'flute', 0.8)
        ],
        bass_events=[(0.0, 3.9, 36, 0.9), (4.0, 3.9, 32, 0.9), (8.0, 3.9, 39, 0.9), (12.0, 3.9, 34, 0.9)],
        title="Astroworld Goosebumps (140 BPM Cm)",
        filename="Trap_140BPM_Cm_Astroworld_Goosebumps.wav",
        out_dir=OUT_DIR, root_note=60
    )

    # 2. Metro Boomin - Superhero (Am, 138)
    render_melody(
        bpm=138.0, num_bars=4,
        chord_events=[
            (0.0, 3.8, [57, 60, 64], 'pad', 0.85), (4.0, 3.8, [53, 57, 60], 'pad', 0.85),
            (8.0, 3.8, [50, 53, 57], 'pad', 0.85), (12.0, 3.8, [52, 56, 59], 'pad', 0.85)
        ],
        lead_events=[
            (0.0, 0.5, 69, 'bell', 0.9), (0.5, 0.5, 72, 'bell', 0.85), (1.0, 0.5, 76, 'bell', 0.9), (1.5, 1.5, 77, 'bell', 0.95),
            (4.0, 0.5, 69, 'bell', 0.9), (4.5, 0.5, 72, 'bell', 0.85), (5.0, 0.5, 76, 'bell', 0.9), (5.5, 1.5, 74, 'bell', 0.9),
            (8.0, 0.5, 69, 'bell', 0.9), (8.5, 0.5, 72, 'bell', 0.85), (9.0, 0.5, 74, 'bell', 0.9), (9.5, 1.5, 72, 'bell', 0.9),
            (12.0, 0.5, 71, 'bell', 0.9), (12.5, 0.5, 72, 'bell', 0.85), (13.0, 1.0, 71, 'bell', 0.9), (14.0, 1.5, 69, 'bell', 0.95)
        ],
        bass_events=[(0.0, 3.9, 33, 0.9), (4.0, 3.9, 29, 0.9), (8.0, 3.9, 26, 0.9), (12.0, 3.9, 28, 0.9)],
        title="Superhero Cinematic (138 BPM Am)",
        filename="Trap_138BPM_Am_Superhero_Cinematic.wav",
        out_dir=OUT_DIR, root_note=57
    )

    # 3. Lil Baby - Drip Too Hard (Fm, 132)
    render_melody(
        bpm=132.0, num_bars=4,
        chord_events=[
            (0.0, 3.8, [53, 56, 60], 'guitar', 0.75), (4.0, 3.8, [49, 53, 56], 'guitar', 0.75),
            (8.0, 3.8, [46, 50, 53], 'guitar', 0.75), (12.0, 3.8, [48, 52, 55], 'guitar', 0.75)
        ],
        lead_events=[
            (0.0, 1.0, 65, 'guitar', 0.95), (1.0, 0.5, 68, 'guitar', 0.9), (1.5, 1.5, 72, 'guitar', 0.95), (3.0, 1.0, 70, 'guitar', 0.85),
            (4.0, 1.0, 65, 'guitar', 0.95), (5.0, 0.5, 68, 'guitar', 0.9), (5.5, 1.5, 73, 'guitar', 0.95), (7.0, 1.0, 72, 'guitar', 0.85),
            (8.0, 1.0, 65, 'guitar', 0.95), (9.0, 0.5, 68, 'guitar', 0.9), (9.5, 1.5, 70, 'guitar', 0.95), (11.0, 1.0, 68, 'guitar', 0.85),
            (12.0, 1.0, 67, 'guitar', 0.9), (13.0, 0.5, 68, 'guitar', 0.85), (13.5, 1.5, 67, 'guitar', 0.9), (15.0, 1.0, 65, 'guitar', 0.95)
        ],
        bass_events=[(0.0, 3.9, 29, 0.85), (4.0, 3.9, 25, 0.85), (8.0, 3.9, 34, 0.85), (12.0, 3.9, 36, 0.85)],
        title="Drip Too Hard Guitar (132 BPM Fm)",
        filename="Trap_132BPM_Fm_Drip_Too_Hard_Guitar.wav",
        out_dir=OUT_DIR, root_note=53
    )

    # 4. Future - Mask Off (Dm, 130)
    render_melody(
        bpm=130.0, num_bars=4,
        chord_events=[
            (0.0, 3.8, [50, 53, 57], 'rhodes', 0.8), (4.0, 3.8, [46, 50, 53], 'rhodes', 0.8),
            (8.0, 3.8, [43, 46, 50], 'rhodes', 0.8), (12.0, 3.8, [45, 49, 52], 'rhodes', 0.8)
        ],
        lead_events=[
            (0.0, 0.75, 74, 'flute', 0.95), (0.75, 0.5, 72, 'flute', 0.85), (1.25, 1.0, 74, 'flute', 0.9),
            (2.25, 0.75, 77, 'flute', 0.95), (3.0, 1.0, 76, 'flute', 0.85),
            (4.0, 0.75, 74, 'flute', 0.95), (4.75, 0.5, 72, 'flute', 0.85), (5.25, 1.0, 74, 'flute', 0.9),
            (6.25, 0.75, 79, 'flute', 0.95), (7.0, 1.0, 77, 'flute', 0.85),
            (8.0, 0.75, 74, 'flute', 0.95), (8.75, 0.5, 72, 'flute', 0.85), (9.25, 1.0, 74, 'flute', 0.9),
            (10.25, 0.75, 76, 'flute', 0.95), (11.0, 1.0, 74, 'flute', 0.85),
            (12.0, 0.75, 73, 'flute', 0.9), (12.75, 0.5, 74, 'flute', 0.85), (13.25, 1.5, 73, 'flute', 0.9), (15.0, 1.0, 74, 'flute', 0.95)
        ],
        bass_events=[(0.0, 3.9, 26, 0.9), (4.0, 3.9, 34, 0.9), (8.0, 3.9, 31, 0.9), (12.0, 3.9, 33, 0.9)],
        title="Mask Off Haunting Flute (130 BPM Dm)",
        filename="Trap_130BPM_Dm_Mask_Off_Haunting_Flute.wav",
        out_dir=OUT_DIR, root_note=50
    )

    # 5. Juice WRLD - Lucid Dreams (Em, 144)
    render_melody(
        bpm=144.0, num_bars=4,
        chord_events=[
            (0.0, 3.8, [52, 55, 59], 'guitar', 0.8), (4.0, 3.8, [48, 52, 55], 'guitar', 0.8),
            (8.0, 3.8, [43, 47, 50], 'guitar', 0.8), (12.0, 3.8, [45, 48, 52], 'guitar', 0.8)
        ],
        lead_events=[
            (0.0, 0.5, 64, 'guitar', 0.9), (0.5, 0.5, 67, 'guitar', 0.85), (1.0, 0.5, 71, 'guitar', 0.9), (1.5, 1.5, 74, 'guitar', 0.95), (3.0, 1.0, 71, 'guitar', 0.85),
            (4.0, 0.5, 64, 'guitar', 0.9), (4.5, 0.5, 67, 'guitar', 0.85), (5.0, 0.5, 72, 'guitar', 0.9), (5.5, 1.5, 76, 'guitar', 0.95), (7.0, 1.0, 72, 'guitar', 0.85),
            (8.0, 0.5, 62, 'guitar', 0.9), (8.5, 0.5, 67, 'guitar', 0.85), (9.0, 0.5, 71, 'guitar', 0.9), (9.5, 1.5, 74, 'guitar', 0.95), (11.0, 1.0, 71, 'guitar', 0.85),
            (12.0, 0.5, 64, 'guitar', 0.9), (12.5, 0.5, 69, 'guitar', 0.85), (13.0, 0.5, 71, 'guitar', 0.9), (13.5, 1.5, 69, 'guitar', 0.9), (15.0, 1.0, 67, 'guitar', 0.85)
        ],
        bass_events=[(0.0, 3.9, 28, 0.85), (4.0, 3.9, 24, 0.85), (8.0, 3.9, 31, 0.85), (12.0, 3.9, 33, 0.85)],
        title="Lucid Dreams Emo Guitar (144 BPM Em)",
        filename="Trap_144BPM_Em_Lucid_Dreams_Guitar.wav",
        out_dir=OUT_DIR, root_note=52
    )

    # 6. Drake & 21 Savage - Jimmy Cooks (Abm, 136)
    render_melody(
        bpm=136.0, num_bars=4,
        chord_events=[
            (0.0, 3.8, [56, 59, 63], 'rhodes', 0.85), (4.0, 3.8, [52, 56, 59], 'rhodes', 0.85),
            (8.0, 3.8, [47, 51, 54], 'rhodes', 0.85), (12.0, 3.8, [54, 58, 61], 'rhodes', 0.85)
        ],
        lead_events=[
            (0.0, 1.0, 68, 'bell', 0.9), (1.5, 1.0, 71, 'bell', 0.85), (3.0, 1.0, 75, 'bell', 0.95),
            (4.0, 1.0, 68, 'bell', 0.9), (5.5, 1.0, 71, 'bell', 0.85), (7.0, 1.0, 76, 'bell', 0.95),
            (8.0, 1.0, 66, 'bell', 0.9), (9.5, 1.0, 71, 'bell', 0.85), (11.0, 1.0, 75, 'bell', 0.95),
            (12.0, 1.0, 66, 'bell', 0.9), (13.5, 1.0, 70, 'bell', 0.85), (15.0, 1.0, 68, 'bell', 0.9)
        ],
        bass_events=[(0.0, 3.9, 32, 0.9), (4.0, 3.9, 28, 0.9), (8.0, 3.9, 35, 0.9), (12.0, 3.9, 30, 0.9)],
        title="Jimmy Cooks Soul Chords (136 BPM Abm)",
        filename="Trap_136BPM_Abm_Jimmy_Cooks_Soul.wav",
        out_dir=OUT_DIR, root_note=56
    )

    # 7. Roddy Ricch - The Box (Bbm, 140)
    render_melody(
        bpm=140.0, num_bars=4,
        chord_events=[
            (0.0, 3.8, [58, 61, 65], 'pad', 0.8), (4.0, 3.8, [54, 58, 61], 'pad', 0.8),
            (8.0, 3.8, [51, 54, 58], 'pad', 0.8), (12.0, 3.8, [53, 57, 60], 'pad', 0.8)
        ],
        lead_events=[
            (0.0, 0.5, 70, 'bell', 0.95), (0.75, 0.5, 70, 'bell', 0.9), (1.5, 1.0, 73, 'bell', 0.95), (3.0, 1.0, 72, 'bell', 0.85),
            (4.0, 0.5, 70, 'bell', 0.95), (4.75, 0.5, 70, 'bell', 0.9), (5.5, 1.0, 75, 'bell', 0.95), (7.0, 1.0, 73, 'bell', 0.85),
            (8.0, 0.5, 70, 'bell', 0.95), (8.75, 0.5, 70, 'bell', 0.9), (9.5, 1.0, 72, 'bell', 0.95), (11.0, 1.0, 70, 'bell', 0.85),
            (12.0, 0.5, 69, 'bell', 0.9), (12.75, 0.5, 70, 'bell', 0.85), (13.5, 1.0, 69, 'bell', 0.9), (15.0, 1.0, 70, 'bell', 0.95)
        ],
        bass_events=[(0.0, 3.9, 34, 0.9), (4.0, 3.9, 30, 0.9), (8.0, 3.9, 27, 0.9), (12.0, 3.9, 29, 0.9)],
        title="The Box Orchestral Pluck (140 BPM Bbm)",
        filename="Trap_140BPM_Bbm_The_Box_Orchestral.wav",
        out_dir=OUT_DIR, root_note=58
    )

    # 8. Gunna - Pushin P (Gm, 134)
    render_melody(
        bpm=134.0, num_bars=4,
        chord_events=[
            (0.0, 3.8, [55, 58, 62], 'rhodes', 0.85), (4.0, 3.8, [51, 55, 58], 'rhodes', 0.85),
            (8.0, 3.8, [48, 51, 55], 'rhodes', 0.85), (12.0, 3.8, [50, 53, 57], 'rhodes', 0.85)
        ],
        lead_events=[
            (0.5, 1.0, 74, 'flute', 0.85), (2.0, 1.0, 77, 'flute', 0.9), (3.5, 0.5, 74, 'flute', 0.8),
            (4.5, 1.0, 75, 'flute', 0.85), (6.0, 1.0, 79, 'flute', 0.9), (7.5, 0.5, 75, 'flute', 0.8),
            (8.5, 1.0, 72, 'flute', 0.85), (10.0, 1.0, 75, 'flute', 0.9), (11.5, 0.5, 74, 'flute', 0.8),
            (12.5, 1.0, 74, 'flute', 0.85), (14.0, 1.0, 72, 'flute', 0.85), (15.0, 1.0, 70, 'flute', 0.8)
        ],
        bass_events=[(0.0, 3.9, 31, 0.85), (4.0, 3.9, 27, 0.85), (8.0, 3.9, 24, 0.85), (12.0, 3.9, 26, 0.85)],
        title="Pushin P Ambient Rhodes (134 BPM Gm)",
        filename="Trap_134BPM_Gm_Pushin_P_Ambient.wav",
        out_dir=OUT_DIR, root_note=55
    )

    # ------------------ REGGAETON & LATIN (9 - 16) ------------------
    # 9. Bad Bunny - Titi Me Pregunto (Fm, 96)
    render_melody(
        bpm=96.0, num_bars=4,
        chord_events=[
            (0.5, 0.5, [65, 68, 72], 'rhodes', 0.9), (1.5, 0.5, [65, 68, 72], 'rhodes', 0.85),
            (2.5, 0.5, [65, 68, 72], 'rhodes', 0.9), (3.5, 0.5, [65, 68, 72], 'rhodes', 0.85),
            (4.5, 0.5, [61, 65, 68], 'rhodes', 0.9), (5.5, 0.5, [61, 65, 68], 'rhodes', 0.85),
            (6.5, 0.5, [61, 65, 68], 'rhodes', 0.9), (7.5, 0.5, [61, 65, 68], 'rhodes', 0.85),
            (8.5, 0.5, [63, 67, 70], 'rhodes', 0.9), (9.5, 0.5, [63, 67, 70], 'rhodes', 0.85),
            (10.5, 0.5, [63, 67, 70], 'rhodes', 0.9), (11.5, 0.5, [63, 67, 70], 'rhodes', 0.85),
            (12.5, 0.5, [60, 63, 67], 'rhodes', 0.9), (13.5, 0.5, [60, 63, 67], 'rhodes', 0.85),
            (14.5, 0.5, [60, 63, 67], 'rhodes', 0.9), (15.5, 0.5, [60, 63, 67], 'rhodes', 0.85)
        ],
        lead_events=[
            (0.0, 0.75, 77, 'bell', 0.9), (1.0, 0.75, 75, 'bell', 0.85), (2.0, 1.5, 72, 'bell', 0.95),
            (4.0, 0.75, 73, 'bell', 0.9), (5.0, 0.75, 72, 'bell', 0.85), (6.0, 1.5, 68, 'bell', 0.95),
            (8.0, 0.75, 75, 'bell', 0.9), (9.0, 0.75, 73, 'bell', 0.85), (10.0, 1.5, 70, 'bell', 0.95),
            (12.0, 0.75, 72, 'bell', 0.9), (13.0, 0.75, 70, 'bell', 0.85), (14.0, 1.5, 67, 'bell', 0.95)
        ],
        bass_events=[(0.0, 3.9, 29, 0.9), (4.0, 3.9, 25, 0.9), (8.0, 3.9, 27, 0.9), (12.0, 3.9, 36, 0.9)],
        title="Titi Me Pregunto Stabs (96 BPM Fm)",
        filename="Reggaeton_96BPM_Fm_Titi_Me_Pregunto.wav",
        out_dir=OUT_DIR, root_note=53
    )

    # 10. Feid - Feliz Cumpleanos Ferxxo (Am, 92)
    render_melody(
        bpm=92.0, num_bars=4,
        chord_events=[
            (0.5, 0.75, [69, 72, 76], 'rhodes', 0.85), (2.5, 0.75, [69, 72, 76], 'rhodes', 0.85),
            (4.5, 0.75, [65, 69, 72], 'rhodes', 0.85), (6.5, 0.75, [65, 69, 72], 'rhodes', 0.85),
            (8.5, 0.75, [60, 64, 67], 'rhodes', 0.85), (10.5, 0.75, [60, 64, 67], 'rhodes', 0.85),
            (12.5, 0.75, [67, 71, 74], 'rhodes', 0.85), (14.5, 0.75, [67, 71, 74], 'rhodes', 0.85)
        ],
        lead_events=[
            (0.0, 1.0, 81, 'flute', 0.9), (1.5, 1.0, 79, 'flute', 0.85), (3.0, 1.0, 76, 'flute', 0.9),
            (4.0, 1.0, 77, 'flute', 0.9), (5.5, 1.0, 76, 'flute', 0.85), (7.0, 1.0, 72, 'flute', 0.9),
            (8.0, 1.0, 76, 'flute', 0.9), (9.5, 1.0, 74, 'flute', 0.85), (11.0, 1.0, 72, 'flute', 0.9),
            (12.0, 1.0, 74, 'flute', 0.9), (13.5, 1.0, 72, 'flute', 0.85), (15.0, 1.0, 69, 'flute', 0.95)
        ],
        bass_events=[(0.0, 3.9, 33, 0.85), (4.0, 3.9, 29, 0.85), (8.0, 3.9, 24, 0.85), (12.0, 3.9, 31, 0.85)],
        title="Feliz Cumpleanos Ferxxo (92 BPM Am)",
        filename="Reggaeton_92BPM_Am_Feliz_Cumpleanos_Ferxxo.wav",
        out_dir=OUT_DIR, root_note=57
    )

    # 11. Rauw Alejandro - Todo De Ti (Cm, 98)
    render_melody(
        bpm=98.0, num_bars=4,
        chord_events=[
            (0.0, 3.8, [60, 63, 67], 'pad', 0.85), (4.0, 3.8, [56, 60, 63], 'pad', 0.85),
            (8.0, 3.8, [58, 62, 65], 'pad', 0.85), (12.0, 3.8, [55, 58, 62], 'pad', 0.85)
        ],
        lead_events=[
            (0.0, 0.5, 72, 'bell', 0.9), (0.75, 0.5, 75, 'bell', 0.85), (1.5, 1.0, 74, 'bell', 0.9), (3.0, 1.0, 72, 'bell', 0.85),
            (4.0, 0.5, 68, 'bell', 0.9), (4.75, 0.5, 72, 'bell', 0.85), (5.5, 1.0, 70, 'bell', 0.9), (7.0, 1.0, 68, 'bell', 0.85),
            (8.0, 0.5, 70, 'bell', 0.9), (8.75, 0.5, 74, 'bell', 0.85), (9.5, 1.0, 72, 'bell', 0.9), (11.0, 1.0, 70, 'bell', 0.85),
            (12.0, 0.5, 67, 'bell', 0.9), (12.75, 0.5, 70, 'bell', 0.85), (13.5, 1.0, 68, 'bell', 0.9), (15.0, 1.0, 67, 'bell', 0.85)
        ],
        bass_events=[(0.0, 3.9, 36, 0.9), (4.0, 3.9, 32, 0.9), (8.0, 3.9, 34, 0.9), (12.0, 3.9, 31, 0.9)],
        title="Todo De Ti Synth Groove (98 BPM Cm)",
        filename="Reggaeton_98BPM_Cm_Todo_De_Ti_Synth.wav",
        out_dir=OUT_DIR, root_note=60
    )

    # 12. Bizarrap & Quevedo - Quedate Bzrp 52 (Dm, 128)
    render_melody(
        bpm=128.0, num_bars=4,
        chord_events=[
            (0.0, 3.8, [62, 65, 69], 'pad', 0.85), (4.0, 3.8, [58, 62, 65], 'pad', 0.85),
            (8.0, 3.8, [53, 57, 60], 'pad', 0.85), (12.0, 3.8, [60, 64, 67], 'pad', 0.85)
        ],
        lead_events=[
            (0.0, 0.75, 74, 'bell', 0.95), (1.0, 0.75, 77, 'bell', 0.9), (2.0, 1.5, 76, 'bell', 0.95),
            (4.0, 0.75, 74, 'bell', 0.95), (5.0, 0.75, 77, 'bell', 0.9), (6.0, 1.5, 79, 'bell', 0.95),
            (8.0, 0.75, 77, 'bell', 0.95), (9.0, 0.75, 76, 'bell', 0.9), (10.0, 1.5, 74, 'bell', 0.95),
            (12.0, 0.75, 76, 'bell', 0.95), (13.0, 0.75, 74, 'bell', 0.9), (14.0, 1.5, 72, 'bell', 0.95)
        ],
        bass_events=[(0.0, 3.9, 26, 0.9), (4.0, 3.9, 34, 0.9), (8.0, 3.9, 29, 0.9), (12.0, 3.9, 36, 0.9)],
        title="Bzrp Quevedo Quedate (128 BPM Dm)",
        filename="Reggaeton_128BPM_Dm_Bzrp_Quevedo_Quedate.wav",
        out_dir=OUT_DIR, root_note=62
    )

    # 13. Bad Bunny - Monaco (Abm, 95)
    render_melody(
        bpm=95.0, num_bars=4,
        chord_events=[
            (0.0, 3.8, [56, 59, 63], 'pad', 0.85), (4.0, 3.8, [52, 56, 59], 'pad', 0.85),
            (8.0, 3.8, [54, 58, 61], 'pad', 0.85), (12.0, 3.8, [51, 54, 58], 'pad', 0.85)
        ],
        lead_events=[
            (0.0, 1.5, 68, 'bell', 0.9), (2.0, 1.5, 71, 'bell', 0.95),
            (4.0, 1.5, 64, 'bell', 0.9), (6.0, 1.5, 68, 'bell', 0.95),
            (8.0, 1.5, 66, 'bell', 0.9), (10.0, 1.5, 70, 'bell', 0.95),
            (12.0, 1.5, 63, 'bell', 0.9), (14.0, 1.5, 66, 'bell', 0.95)
        ],
        bass_events=[(0.0, 3.9, 32, 0.9), (4.0, 3.9, 28, 0.9), (8.0, 3.9, 30, 0.9), (12.0, 3.9, 27, 0.9)],
        title="Monaco Cinematic Latin Trap (95 BPM Abm)",
        filename="Reggaeton_95BPM_Abm_Monaco_Cinematic.wav",
        out_dir=OUT_DIR, root_note=56
    )

    # 14. Feid & Young Miko - Classy 101 (Em, 94)
    render_melody(
        bpm=94.0, num_bars=4,
        chord_events=[
            (0.5, 0.5, [64, 67, 71], 'rhodes', 0.85), (2.5, 0.5, [64, 67, 71], 'rhodes', 0.85),
            (4.5, 0.5, [60, 64, 67], 'rhodes', 0.85), (6.5, 0.5, [60, 64, 67], 'rhodes', 0.85),
            (8.5, 0.5, [62, 65, 69], 'rhodes', 0.85), (10.5, 0.5, [62, 65, 69], 'rhodes', 0.85),
            (12.5, 0.5, [59, 62, 65], 'rhodes', 0.85), (14.5, 0.5, [59, 62, 65], 'rhodes', 0.85)
        ],
        lead_events=[
            (0.0, 1.0, 76, 'flute', 0.9), (1.5, 1.0, 74, 'flute', 0.85), (3.0, 1.0, 71, 'flute', 0.9),
            (4.0, 1.0, 72, 'flute', 0.9), (5.5, 1.0, 71, 'flute', 0.85), (7.0, 1.0, 67, 'flute', 0.9),
            (8.0, 1.0, 74, 'flute', 0.9), (9.5, 1.0, 72, 'flute', 0.85), (11.0, 1.0, 69, 'flute', 0.9),
            (12.0, 1.0, 71, 'flute', 0.9), (13.5, 1.0, 69, 'flute', 0.85), (15.0, 1.0, 67, 'flute', 0.95)
        ],
        bass_events=[(0.0, 3.9, 28, 0.85), (4.0, 3.9, 24, 0.85), (8.0, 3.9, 26, 0.85), (12.0, 3.9, 23, 0.85)],
        title="Classy 101 Smooth Rhodes (94 BPM Em)",
        filename="Reggaeton_94BPM_Em_Classy_101_Keys.wav",
        out_dir=OUT_DIR, root_note=52
    )

    # 15. Bad Bunny & Jhayco - Dakiti (Bbm, 90)
    render_melody(
        bpm=90.0, num_bars=4,
        chord_events=[
            (0.0, 3.8, [58, 61, 65], 'pad', 0.8), (4.0, 3.8, [54, 58, 61], 'pad', 0.8),
            (8.0, 3.8, [49, 53, 56], 'pad', 0.8), (12.0, 3.8, [56, 60, 63], 'pad', 0.8)
        ],
        lead_events=[
            (0.0, 0.5, 70, 'bell', 0.9), (0.75, 0.5, 73, 'bell', 0.85), (1.5, 1.0, 72, 'bell', 0.9), (3.0, 1.0, 70, 'bell', 0.85),
            (4.0, 0.5, 66, 'bell', 0.9), (4.75, 0.5, 70, 'bell', 0.85), (5.5, 1.0, 68, 'bell', 0.9), (7.0, 1.0, 66, 'bell', 0.85),
            (8.0, 0.5, 65, 'bell', 0.9), (8.75, 0.5, 68, 'bell', 0.85), (9.5, 1.0, 66, 'bell', 0.9), (11.0, 1.0, 65, 'bell', 0.85),
            (12.0, 0.5, 68, 'bell', 0.9), (12.75, 0.5, 72, 'bell', 0.85), (13.5, 1.0, 70, 'bell', 0.9), (15.0, 1.0, 68, 'bell', 0.85)
        ],
        bass_events=[(0.0, 3.9, 34, 0.85), (4.0, 3.9, 30, 0.85), (8.0, 3.9, 25, 0.85), (12.0, 3.9, 32, 0.85)],
        title="Dakiti Ambient Pluck (90 BPM Bbm)",
        filename="Reggaeton_90BPM_Bbm_Dakiti_Ambient_Pluck.wav",
        out_dir=OUT_DIR, root_note=58
    )

    # 16. Rauw Alejandro - Desesperados (Gm, 96)
    render_melody(
        bpm=96.0, num_bars=4,
        chord_events=[
            (0.5, 0.5, [67, 70, 74], 'rhodes', 0.85), (2.5, 0.5, [67, 70, 74], 'rhodes', 0.85),
            (4.5, 0.5, [63, 67, 70], 'rhodes', 0.85), (6.5, 0.5, [63, 67, 70], 'rhodes', 0.85),
            (8.5, 0.5, [58, 62, 65], 'rhodes', 0.85), (10.5, 0.5, [58, 62, 65], 'rhodes', 0.85),
            (12.5, 0.5, [65, 69, 72], 'rhodes', 0.85), (14.5, 0.5, [65, 69, 72], 'rhodes', 0.85)
        ],
        lead_events=[
            (0.0, 0.75, 79, 'bell', 0.9), (1.0, 0.75, 77, 'bell', 0.85), (2.0, 1.5, 74, 'bell', 0.95),
            (4.0, 0.75, 75, 'bell', 0.9), (5.0, 0.75, 74, 'bell', 0.85), (6.0, 1.5, 70, 'bell', 0.95),
            (8.0, 0.75, 74, 'bell', 0.9), (9.0, 0.75, 72, 'bell', 0.85), (10.0, 1.5, 67, 'bell', 0.95),
            (12.0, 0.75, 72, 'bell', 0.9), (13.0, 0.75, 70, 'bell', 0.85), (14.0, 1.5, 65, 'bell', 0.95)
        ],
        bass_events=[(0.0, 3.9, 31, 0.9), (4.0, 3.9, 27, 0.9), (8.0, 3.9, 34, 0.9), (12.0, 3.9, 29, 0.9)],
        title="Desesperados Melodic Synth (96 BPM Gm)",
        filename="Reggaeton_96BPM_Gm_Desesperados_Lead.wav",
        out_dir=OUT_DIR, root_note=55
    )

    # ------------------ DRILL (17 - 22) ------------------
    # 17. Pop Smoke - Dior (Em, 142)
    render_melody(
        bpm=142.0, num_bars=4,
        chord_events=[
            (0.0, 3.8, [52, 55, 59], 'pad', 0.85), (4.0, 3.8, [48, 52, 55], 'pad', 0.85),
            (8.0, 3.8, [45, 48, 52], 'pad', 0.85), (12.0, 3.8, [47, 51, 54], 'pad', 0.85)
        ],
        lead_events=[
            (0.0, 0.5, 76, 'bell', 0.95), (0.75, 0.5, 79, 'bell', 0.9), (1.5, 0.5, 78, 'bell', 0.85), (2.25, 1.0, 76, 'bell', 0.9),
            (4.0, 0.5, 72, 'bell', 0.95), (4.75, 0.5, 76, 'bell', 0.9), (5.5, 0.5, 74, 'bell', 0.85), (6.25, 1.0, 72, 'bell', 0.9),
            (8.0, 0.5, 69, 'bell', 0.95), (8.75, 0.5, 72, 'bell', 0.9), (9.5, 0.5, 71, 'bell', 0.85), (10.25, 1.0, 69, 'bell', 0.9),
            (12.0, 0.5, 71, 'bell', 0.95), (12.75, 0.5, 74, 'bell', 0.9), (13.5, 0.5, 72, 'bell', 0.85), (14.25, 1.0, 71, 'bell', 0.9)
        ],
        bass_events=[(0.0, 3.9, 28, 0.9), (4.0, 3.9, 24, 0.9), (8.0, 3.9, 33, 0.9), (12.0, 3.9, 35, 0.9)],
        title="Dior Eerie Bells (142 BPM Em)",
        filename="Drill_142BPM_Em_Dior_Eerie_Bells.wav",
        out_dir=OUT_DIR, root_note=52
    )

    # 18. Central Cee - Doja (Cm, 140)
    render_melody(
        bpm=140.0, num_bars=4,
        chord_events=[
            (0.0, 3.8, [60, 63, 67], 'rhodes', 0.85), (4.0, 3.8, [56, 60, 63], 'rhodes', 0.85),
            (8.0, 3.8, [53, 56, 60], 'rhodes', 0.85), (12.0, 3.8, [55, 59, 62], 'rhodes', 0.85)
        ],
        lead_events=[
            (0.0, 0.5, 72, 'flute', 0.95), (0.75, 0.5, 75, 'flute', 0.9), (1.5, 0.75, 74, 'flute', 0.85), (2.5, 1.0, 72, 'flute', 0.9),
            (4.0, 0.5, 68, 'flute', 0.95), (4.75, 0.5, 72, 'flute', 0.9), (5.5, 0.75, 70, 'flute', 0.85), (6.5, 1.0, 68, 'flute', 0.9),
            (8.0, 0.5, 65, 'flute', 0.95), (8.75, 0.5, 68, 'flute', 0.9), (9.5, 0.75, 67, 'flute', 0.85), (10.5, 1.0, 65, 'flute', 0.9),
            (12.0, 0.5, 67, 'flute', 0.95), (12.75, 0.5, 71, 'flute', 0.9), (13.5, 0.75, 68, 'flute', 0.85), (14.5, 1.0, 67, 'flute', 0.9)
        ],
        bass_events=[(0.0, 3.9, 36, 0.9), (4.0, 3.9, 32, 0.9), (8.0, 3.9, 29, 0.9), (12.0, 3.9, 31, 0.9)],
        title="Doja Whistle Drill Hook (140 BPM Cm)",
        filename="Drill_140BPM_Cm_Doja_Whistle_Drill.wav",
        out_dir=OUT_DIR, root_note=60
    )

    # 19. Fivio Foreign - Big Drip (Am, 144)
    render_melody(
        bpm=144.0, num_bars=4,
        chord_events=[
            (0.0, 3.8, [57, 60, 64], 'pad', 0.85), (4.0, 3.8, [54, 57, 60], 'pad', 0.85),
            (8.0, 3.8, [53, 57, 60], 'pad', 0.85), (12.0, 3.8, [52, 56, 59], 'pad', 0.85)
        ],
        lead_events=[
            (0.0, 0.5, 69, 'bell', 0.95), (0.5, 0.5, 72, 'bell', 0.9), (1.0, 0.5, 75, 'bell', 0.9), (1.5, 1.5, 76, 'bell', 0.95),
            (4.0, 0.5, 66, 'bell', 0.95), (4.5, 0.5, 69, 'bell', 0.9), (5.0, 0.5, 72, 'bell', 0.9), (5.5, 1.5, 75, 'bell', 0.95),
            (8.0, 0.5, 65, 'bell', 0.95), (8.5, 0.5, 69, 'bell', 0.9), (9.0, 0.5, 72, 'bell', 0.9), (9.5, 1.5, 74, 'bell', 0.95),
            (12.0, 0.5, 64, 'bell', 0.95), (12.5, 0.5, 68, 'bell', 0.9), (13.0, 0.5, 71, 'bell', 0.9), (13.5, 1.5, 69, 'bell', 0.95)
        ],
        bass_events=[(0.0, 3.9, 33, 0.9), (4.0, 3.9, 30, 0.9), (8.0, 3.9, 29, 0.9), (12.0, 3.9, 28, 0.9)],
        title="Big Drip Tritone Piano (144 BPM Am)",
        filename="Drill_144BPM_Am_Big_Drip_Tritone_Piano.wav",
        out_dir=OUT_DIR, root_note=57
    )

    # 20. Russ Millions & Tion Wayne - Body (Fm, 141)
    render_melody(
        bpm=141.0, num_bars=4,
        chord_events=[
            (0.0, 3.8, [53, 56, 60], 'rhodes', 0.8), (4.0, 3.8, [49, 53, 56], 'rhodes', 0.8),
            (8.0, 3.8, [51, 55, 58], 'rhodes', 0.8), (12.0, 3.8, [48, 52, 55], 'rhodes', 0.8)
        ],
        lead_events=[
            (0.0, 0.5, 65, 'bell', 0.95), (0.5, 0.5, 68, 'bell', 0.9), (1.0, 0.5, 72, 'bell', 0.9), (1.5, 1.5, 70, 'bell', 0.9),
            (4.0, 0.5, 61, 'bell', 0.95), (4.5, 0.5, 65, 'bell', 0.9), (5.0, 0.5, 68, 'bell', 0.9), (5.5, 1.5, 67, 'bell', 0.9),
            (8.0, 0.5, 63, 'bell', 0.95), (8.5, 0.5, 67, 'bell', 0.9), (9.0, 0.5, 70, 'bell', 0.9), (9.5, 1.5, 68, 'bell', 0.9),
            (12.0, 0.5, 60, 'bell', 0.95), (12.5, 0.5, 64, 'bell', 0.9), (13.0, 0.5, 67, 'bell', 0.9), (13.5, 1.5, 65, 'bell', 0.95)
        ],
        bass_events=[(0.0, 3.9, 29, 0.9), (4.0, 3.9, 25, 0.9), (8.0, 3.9, 27, 0.9), (12.0, 3.9, 36, 0.9)],
        title="Body Sliding Pluck Arp (141 BPM Fm)",
        filename="Drill_141BPM_Fm_Body_Sliding_Pluck.wav",
        out_dir=OUT_DIR, root_note=53
    )

    # 21. Morad - Pelele Spanish Drill (Dm, 143)
    render_melody(
        bpm=143.0, num_bars=4,
        chord_events=[
            (0.0, 3.8, [50, 53, 57], 'guitar', 0.85), (4.0, 3.8, [46, 50, 53], 'guitar', 0.85),
            (8.0, 3.8, [43, 46, 50], 'guitar', 0.85), (12.0, 3.8, [45, 49, 52], 'guitar', 0.85)
        ],
        lead_events=[
            (0.0, 0.5, 62, 'guitar', 0.95), (0.5, 0.5, 65, 'guitar', 0.9), (1.0, 0.5, 69, 'guitar', 0.9), (1.5, 1.5, 74, 'guitar', 0.95),
            (4.0, 0.5, 58, 'guitar', 0.95), (4.5, 0.5, 62, 'guitar', 0.9), (5.0, 0.5, 65, 'guitar', 0.9), (5.5, 1.5, 70, 'guitar', 0.95),
            (8.0, 0.5, 55, 'guitar', 0.95), (8.5, 0.5, 58, 'guitar', 0.9), (9.0, 0.5, 62, 'guitar', 0.9), (9.5, 1.5, 67, 'guitar', 0.95),
            (12.0, 0.5, 57, 'guitar', 0.95), (12.5, 0.5, 61, 'guitar', 0.9), (13.0, 0.5, 64, 'guitar', 0.9), (13.5, 1.5, 62, 'guitar', 0.95)
        ],
        bass_events=[(0.0, 3.9, 26, 0.9), (4.0, 3.9, 34, 0.9), (8.0, 3.9, 31, 0.9), (12.0, 3.9, 33, 0.9)],
        title="Pelele Spanish Drill Guitar (143 BPM Dm)",
        filename="Drill_143BPM_Dm_Pelele_Spanish_Drill.wav",
        out_dir=OUT_DIR, root_note=50
    )

    # 22. Pop Smoke - Welcome to the Party (Gm, 140)
    render_melody(
        bpm=140.0, num_bars=4,
        chord_events=[
            (0.0, 3.8, [55, 58, 62], 'pad', 0.85), (4.0, 3.8, [51, 55, 58], 'pad', 0.85),
            (8.0, 3.8, [53, 57, 60], 'pad', 0.85), (12.0, 3.8, [50, 54, 57], 'pad', 0.85)
        ],
        lead_events=[
            (0.0, 1.0, 67, 'bell', 0.95), (1.5, 1.0, 70, 'bell', 0.9), (3.0, 1.0, 74, 'bell', 0.95),
            (4.0, 1.0, 63, 'bell', 0.95), (5.5, 1.0, 67, 'bell', 0.9), (7.0, 1.0, 70, 'bell', 0.95),
            (8.0, 1.0, 65, 'bell', 0.95), (9.5, 1.0, 69, 'bell', 0.9), (11.0, 1.0, 72, 'bell', 0.95),
            (12.0, 1.0, 62, 'bell', 0.95), (13.5, 1.0, 66, 'bell', 0.9), (15.0, 1.0, 67, 'bell', 0.95)
        ],
        bass_events=[(0.0, 3.9, 31, 0.9), (4.0, 3.9, 27, 0.9), (8.0, 3.9, 29, 0.9), (12.0, 3.9, 26, 0.9)],
        title="Welcome to Party Choir (140 BPM Gm)",
        filename="Drill_140BPM_Gm_Welcome_Party_Choir.wav",
        out_dir=OUT_DIR, root_note=55
    )

    # ------------------ HIP-HOP / BOOMBAP / WEST COAST (23 - 28) ------------------
    # 23. Kendrick Lamar - Not Like Us (Am, 100)
    render_melody(
        bpm=100.0, num_bars=4,
        chord_events=[
            (0.0, 1.5, [57, 60, 64], 'rhodes', 0.9), (2.0, 1.5, [57, 60, 64], 'rhodes', 0.85),
            (4.0, 1.5, [50, 53, 57], 'rhodes', 0.9), (6.0, 1.5, [52, 56, 59], 'rhodes', 0.85),
            (8.0, 1.5, [57, 60, 64], 'rhodes', 0.9), (10.0, 1.5, [57, 60, 64], 'rhodes', 0.85),
            (12.0, 1.5, [50, 53, 57], 'rhodes', 0.9), (14.0, 1.5, [52, 56, 59], 'rhodes', 0.85)
        ],
        lead_events=[
            (0.5, 0.5, 69, 'bell', 0.9), (1.5, 0.5, 72, 'bell', 0.85), (2.5, 1.0, 71, 'bell', 0.9),
            (4.5, 0.5, 65, 'bell', 0.9), (5.5, 0.5, 69, 'bell', 0.85), (6.5, 1.0, 68, 'bell', 0.9),
            (8.5, 0.5, 69, 'bell', 0.9), (9.5, 0.5, 72, 'bell', 0.85), (10.5, 1.0, 74, 'bell', 0.9),
            (12.5, 0.5, 65, 'bell', 0.9), (13.5, 0.5, 68, 'bell', 0.85), (14.5, 1.0, 69, 'bell', 0.95)
        ],
        bass_events=[(0.0, 3.9, 33, 0.9), (4.0, 3.9, 26, 0.9), (8.0, 3.9, 33, 0.9), (12.0, 3.9, 28, 0.9)],
        title="Not Like Us West Coast Groove (100 BPM Am)",
        filename="HipHop_100BPM_Am_Not_Like_Us_West_Coast.wav",
        out_dir=OUT_DIR, root_note=57
    )

    # 24. Dr. Dre & Snoop - Still D.R.E. (Cm, 94)
    render_melody(
        bpm=94.0, num_bars=4,
        chord_events=[
            (0.0, 0.5, [63, 67, 72], 'rhodes', 0.95), (0.5, 0.5, [63, 67, 72], 'rhodes', 0.9),
            (1.0, 0.5, [63, 67, 72], 'rhodes', 0.9), (1.5, 0.5, [63, 67, 72], 'rhodes', 0.9),
            (2.0, 0.5, [63, 67, 72], 'rhodes', 0.95), (2.5, 0.5, [63, 67, 72], 'rhodes', 0.9),
            (3.0, 0.5, [63, 67, 72], 'rhodes', 0.9), (3.5, 0.5, [61, 65, 70], 'rhodes', 0.95),
            (4.0, 0.5, [61, 65, 70], 'rhodes', 0.9), (4.5, 0.5, [61, 65, 70], 'rhodes', 0.9),
            (5.0, 0.5, [61, 65, 70], 'rhodes', 0.9), (5.5, 0.5, [61, 65, 70], 'rhodes', 0.9),
            (6.0, 0.5, [61, 65, 70], 'rhodes', 0.9), (6.5, 0.5, [61, 65, 70], 'rhodes', 0.9),
            (7.0, 0.5, [61, 65, 70], 'rhodes', 0.9), (7.5, 0.5, [63, 67, 72], 'rhodes', 0.95),
            (8.0, 0.5, [63, 67, 72], 'rhodes', 0.9), (8.5, 0.5, [63, 67, 72], 'rhodes', 0.9),
            (9.0, 0.5, [63, 67, 72], 'rhodes', 0.9), (9.5, 0.5, [63, 67, 72], 'rhodes', 0.9),
            (10.0, 0.5, [63, 67, 72], 'rhodes', 0.9), (10.5, 0.5, [63, 67, 72], 'rhodes', 0.9),
            (11.0, 0.5, [63, 67, 72], 'rhodes', 0.9), (11.5, 0.5, [61, 65, 70], 'rhodes', 0.95),
            (12.0, 0.5, [61, 65, 70], 'rhodes', 0.9), (12.5, 0.5, [61, 65, 70], 'rhodes', 0.9),
            (13.0, 0.5, [61, 65, 70], 'rhodes', 0.9), (13.5, 0.5, [61, 65, 70], 'rhodes', 0.9),
            (14.0, 0.5, [61, 65, 70], 'rhodes', 0.9), (14.5, 0.5, [61, 65, 70], 'rhodes', 0.9),
            (15.0, 0.5, [61, 65, 70], 'rhodes', 0.9), (15.5, 0.5, [63, 67, 72], 'rhodes', 0.95)
        ],
        lead_events=[
            (0.0, 3.5, 84, 'bell', 0.5), (4.0, 3.5, 82, 'bell', 0.5),
            (8.0, 3.5, 84, 'bell', 0.5), (12.0, 3.5, 82, 'bell', 0.5)
        ],
        bass_events=[(0.0, 3.9, 36, 0.9), (4.0, 3.9, 34, 0.9), (8.0, 3.9, 36, 0.9), (12.0, 3.9, 34, 0.9)],
        title="Still Dre Piano Staccato (94 BPM Cm)",
        filename="HipHop_94BPM_Cm_Still_Dre_Piano_Staccato.wav",
        out_dir=OUT_DIR, root_note=60
    )

    # 25. J. Cole - No Role Modelz (Fm, 88)
    render_melody(
        bpm=88.0, num_bars=4,
        chord_events=[
            (0.0, 3.8, [53, 56, 60, 65], 'rhodes', 0.85), (4.0, 3.8, [46, 50, 53, 58], 'rhodes', 0.85),
            (8.0, 3.8, [51, 55, 58, 63], 'rhodes', 0.85), (12.0, 3.8, [48, 51, 55, 60], 'rhodes', 0.85)
        ],
        lead_events=[
            (0.0, 1.5, 68, 'flute', 0.9), (2.0, 1.5, 72, 'flute', 0.85),
            (4.0, 1.5, 65, 'flute', 0.9), (6.0, 1.5, 70, 'flute', 0.85),
            (8.0, 1.5, 67, 'flute', 0.9), (10.0, 1.5, 70, 'flute', 0.85),
            (12.0, 1.5, 63, 'flute', 0.9), (14.0, 1.5, 68, 'flute', 0.95)
        ],
        bass_events=[(0.0, 3.9, 29, 0.9), (4.0, 3.9, 34, 0.9), (8.0, 3.9, 27, 0.9), (12.0, 3.9, 36, 0.9)],
        title="No Role Modelz Jazzy Rhodes (88 BPM Fm)",
        filename="HipHop_88BPM_Fm_No_Role_Modelz_Jazzy.wav",
        out_dir=OUT_DIR, root_note=53
    )

    # 26. A$AP Rocky - Praise The Lord (Dm, 92)
    render_melody(
        bpm=92.0, num_bars=4,
        chord_events=[
            (0.0, 3.8, [50, 53, 57], 'pad', 0.8), (4.0, 3.8, [53, 57, 60], 'pad', 0.8),
            (8.0, 3.8, [48, 52, 55], 'pad', 0.8), (12.0, 3.8, [46, 50, 53], 'pad', 0.8)
        ],
        lead_events=[
            (0.0, 0.5, 74, 'flute', 0.95), (0.5, 0.5, 77, 'flute', 0.9), (1.0, 0.5, 76, 'flute', 0.85), (1.5, 1.5, 74, 'flute', 0.95),
            (4.0, 0.5, 77, 'flute', 0.95), (4.5, 0.5, 81, 'flute', 0.9), (5.0, 0.5, 79, 'flute', 0.85), (5.5, 1.5, 77, 'flute', 0.95),
            (8.0, 0.5, 72, 'flute', 0.95), (8.5, 0.5, 76, 'flute', 0.9), (9.0, 0.5, 74, 'flute', 0.85), (9.5, 1.5, 72, 'flute', 0.95),
            (12.0, 0.5, 70, 'flute', 0.95), (12.5, 0.5, 74, 'flute', 0.9), (13.0, 0.5, 72, 'flute', 0.85), (13.5, 1.5, 70, 'flute', 0.95)
        ],
        bass_events=[(0.0, 3.9, 26, 0.9), (4.0, 3.9, 29, 0.9), (8.0, 3.9, 24, 0.9), (12.0, 3.9, 34, 0.9)],
        title="Praise The Lord Pan Flute (92 BPM Dm)",
        filename="HipHop_92BPM_Dm_Praise_The_Lord_Pan_Flute.wav",
        out_dir=OUT_DIR, root_note=50
    )

    # 27. Mac Miller - Self Care (Em, 85)
    render_melody(
        bpm=85.0, num_bars=4,
        chord_events=[
            (0.0, 3.8, [52, 55, 59, 62], 'rhodes', 0.85), (4.0, 3.8, [48, 52, 55, 59], 'rhodes', 0.85),
            (8.0, 3.8, [45, 48, 52, 55], 'rhodes', 0.85), (12.0, 3.8, [47, 50, 54, 57], 'rhodes', 0.85)
        ],
        lead_events=[
            (0.5, 1.0, 71, 'bell', 0.85), (2.0, 1.0, 74, 'bell', 0.9), (3.0, 1.0, 71, 'bell', 0.8),
            (4.5, 1.0, 67, 'bell', 0.85), (6.0, 1.0, 71, 'bell', 0.9), (7.0, 1.0, 67, 'bell', 0.8),
            (8.5, 1.0, 69, 'bell', 0.85), (10.0, 1.0, 72, 'bell', 0.9), (11.0, 1.0, 69, 'bell', 0.8),
            (12.5, 1.0, 71, 'bell', 0.85), (14.0, 1.0, 74, 'bell', 0.9), (15.0, 1.0, 71, 'bell', 0.85)
        ],
        bass_events=[(0.0, 3.9, 28, 0.9), (4.0, 3.9, 24, 0.9), (8.0, 3.9, 33, 0.9), (12.0, 3.9, 35, 0.9)],
        title="Self Care Neo Soul Rhodes (85 BPM Em)",
        filename="HipHop_85BPM_Em_Self_Care_Neo_Soul.wav",
        out_dir=OUT_DIR, root_note=52
    )

    # 28. Kendrick Lamar - Alright (Abm, 89)
    render_melody(
        bpm=89.0, num_bars=4,
        chord_events=[
            (0.0, 3.8, [56, 59, 63, 66], 'pad', 0.85), (4.0, 3.8, [49, 52, 56, 59], 'pad', 0.85),
            (8.0, 3.8, [54, 58, 61, 64], 'pad', 0.85), (12.0, 3.8, [47, 51, 54, 58], 'pad', 0.85)
        ],
        lead_events=[
            (0.0, 1.0, 71, 'flute', 0.9), (1.5, 1.0, 75, 'flute', 0.85), (3.0, 1.0, 78, 'flute', 0.95),
            (4.0, 1.0, 68, 'flute', 0.9), (5.5, 1.0, 71, 'flute', 0.85), (7.0, 1.0, 75, 'flute', 0.95),
            (8.0, 1.0, 70, 'flute', 0.9), (9.5, 1.0, 73, 'flute', 0.85), (11.0, 1.0, 76, 'flute', 0.95),
            (12.0, 1.0, 66, 'flute', 0.9), (13.5, 1.0, 70, 'flute', 0.85), (15.0, 1.0, 71, 'flute', 0.9)
        ],
        bass_events=[(0.0, 3.9, 32, 0.9), (4.0, 3.9, 25, 0.9), (8.0, 3.9, 30, 0.9), (12.0, 3.9, 35, 0.9)],
        title="Alright Soul Brass Chords (89 BPM Abm)",
        filename="HipHop_89BPM_Abm_Alright_Chords.wav",
        out_dir=OUT_DIR, root_note=56
    )

    # ------------------ AFRO & R&B (29 - 32) ------------------
    # 29. Rema - Calm Down (Am, 100)
    render_melody(
        bpm=100.0, num_bars=4,
        chord_events=[
            (0.0, 3.8, [57, 60, 64], 'guitar', 0.85), (4.0, 3.8, [53, 57, 60], 'guitar', 0.85),
            (8.0, 3.8, [48, 52, 55], 'guitar', 0.85), (12.0, 3.8, [55, 59, 62], 'guitar', 0.85)
        ],
        lead_events=[
            (0.0, 0.75, 69, 'guitar', 0.95), (1.0, 0.75, 72, 'guitar', 0.9), (2.0, 1.5, 76, 'guitar', 0.95),
            (4.0, 0.75, 65, 'guitar', 0.95), (5.0, 0.75, 69, 'guitar', 0.9), (6.0, 1.5, 72, 'guitar', 0.95),
            (8.0, 0.75, 67, 'guitar', 0.95), (9.0, 0.75, 71, 'guitar', 0.9), (10.0, 1.5, 74, 'guitar', 0.95),
            (12.0, 0.75, 71, 'guitar', 0.95), (13.0, 0.75, 74, 'guitar', 0.9), (14.0, 1.5, 76, 'guitar', 0.95)
        ],
        bass_events=[(0.0, 3.9, 33, 0.9), (4.0, 3.9, 29, 0.9), (8.0, 3.9, 24, 0.9), (12.0, 3.9, 31, 0.9)],
        title="Calm Down Afro Guitar (100 BPM Am)",
        filename="Afro_Urban_100BPM_Am_Calm_Down_Afro_Guitar.wav",
        out_dir=OUT_DIR, root_note=57
    )

    # 30. Burna Boy - Last Last (Fm, 104)
    render_melody(
        bpm=104.0, num_bars=4,
        chord_events=[
            (0.0, 3.8, [53, 56, 60], 'pad', 0.85), (4.0, 3.8, [46, 50, 53], 'pad', 0.85),
            (8.0, 3.8, [49, 53, 56], 'pad', 0.85), (12.0, 3.8, [48, 52, 55], 'pad', 0.85)
        ],
        lead_events=[
            (0.0, 1.0, 65, 'flute', 0.95), (1.5, 1.0, 68, 'flute', 0.9), (3.0, 1.0, 72, 'flute', 0.95),
            (4.0, 1.0, 61, 'flute', 0.95), (5.5, 1.0, 65, 'flute', 0.9), (7.0, 1.0, 68, 'flute', 0.95),
            (8.0, 1.0, 65, 'flute', 0.95), (9.5, 1.0, 68, 'flute', 0.9), (11.0, 1.0, 72, 'flute', 0.95),
            (12.0, 1.0, 64, 'flute', 0.95), (13.5, 1.0, 67, 'flute', 0.9), (15.0, 1.0, 65, 'flute', 0.95)
        ],
        bass_events=[(0.0, 3.9, 29, 0.9), (4.0, 3.9, 34, 0.9), (8.0, 3.9, 25, 0.9), (12.0, 3.9, 36, 0.9)],
        title="Last Last Soul Brass Hook (104 BPM Fm)",
        filename="Afro_Urban_104BPM_Fm_Last_Last_Soul_Brass.wav",
        out_dir=OUT_DIR, root_note=53
    )

    # 31. The Weeknd - Blinding Lights (Dm, 120)
    render_melody(
        bpm=120.0, num_bars=4,
        chord_events=[
            (0.0, 3.8, [50, 53, 57], 'pad', 0.85), (4.0, 3.8, [45, 48, 52], 'pad', 0.85),
            (8.0, 3.8, [48, 52, 55], 'pad', 0.85), (12.0, 3.8, [43, 47, 50], 'pad', 0.85)
        ],
        lead_events=[
            (0.0, 0.5, 74, 'bell', 0.95), (0.75, 0.5, 74, 'bell', 0.9), (1.5, 1.0, 72, 'bell', 0.9), (3.0, 1.0, 69, 'bell', 0.85),
            (4.0, 0.5, 69, 'bell', 0.95), (4.75, 0.5, 69, 'bell', 0.9), (5.5, 1.0, 72, 'bell', 0.9), (7.0, 1.0, 69, 'bell', 0.85),
            (8.0, 0.5, 72, 'bell', 0.95), (8.75, 0.5, 72, 'bell', 0.9), (9.5, 1.0, 74, 'bell', 0.9), (11.0, 1.0, 72, 'bell', 0.85),
            (12.0, 0.5, 67, 'bell', 0.95), (12.75, 0.5, 69, 'bell', 0.9), (13.5, 1.0, 71, 'bell', 0.9), (15.0, 1.0, 69, 'bell', 0.85)
        ],
        bass_events=[(0.0, 3.9, 26, 0.9), (4.0, 3.9, 33, 0.9), (8.0, 3.9, 24, 0.9), (12.0, 3.9, 31, 0.9)],
        title="Blinding Lights 80s Synth Lead (120 BPM Dm)",
        filename="RnB_120BPM_Dm_Blinding_Lights_Vintage_Synth.wav",
        out_dir=OUT_DIR, root_note=50
    )

    # 32. SZA - Kill Bill (Cm, 86)
    render_melody(
        bpm=86.0, num_bars=4,
        chord_events=[
            (0.0, 3.8, [48, 51, 55], 'guitar', 0.85), (4.0, 3.8, [41, 44, 48], 'guitar', 0.85),
            (8.0, 3.8, [46, 50, 53], 'guitar', 0.85), (12.0, 3.8, [39, 43, 46], 'guitar', 0.85)
        ],
        lead_events=[
            (0.5, 1.0, 67, 'rhodes', 0.85), (2.0, 1.0, 70, 'rhodes', 0.9), (3.0, 1.0, 67, 'rhodes', 0.8),
            (4.5, 1.0, 65, 'rhodes', 0.85), (6.0, 1.0, 68, 'rhodes', 0.9), (7.0, 1.0, 65, 'rhodes', 0.8),
            (8.5, 1.0, 65, 'rhodes', 0.85), (10.0, 1.0, 68, 'rhodes', 0.9), (11.0, 1.0, 70, 'rhodes', 0.85),
            (12.5, 1.0, 63, 'rhodes', 0.85), (14.0, 1.0, 67, 'rhodes', 0.9), (15.0, 1.0, 63, 'rhodes', 0.8)
        ],
        bass_events=[(0.0, 3.9, 36, 0.9), (4.0, 3.9, 29, 0.9), (8.0, 3.9, 34, 0.9), (12.0, 3.9, 27, 0.9)],
        title="Kill Bill Lofi Guitar (86 BPM Cm)",
        filename="RnB_86BPM_Cm_Kill_Bill_Lofi_Guitar.wav",
        out_dir=OUT_DIR, root_note=60
    )

    print("[SUCCESS] All 32 Hit-Inspired Melodies Rendered!")

if __name__ == "__main__":
    generate_all()
