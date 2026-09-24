import os
from midi_writer import MidiWriter

def generate_all_midis(midi_dir):
    os.makedirs(midi_dir, exist_ok=True)
    tpb = 480 # ticks per beat
    
    # 1. MIDI_HiHat_Roll_140BPM_Trap.mid
    m1 = MidiWriter(ticks_per_beat=tpb)
    tr = m1.create_track()
    m1.add_tempo(tr, 0, 140)
    # 4 bars: 8th notes + triplet rolls
    for bar in range(4):
        base = bar * 4 * tpb
        for step in range(8):
            tick = base + step * (tpb // 2)
            m1.add_note(tr, tick, tpb // 4, 42, velocity=95 if step % 2 == 0 else 75)
        # rolls on 2nd and 4th bar
        if bar % 2 == 1:
            for r in range(6):
                tick = base + int(3.0 * tpb + r * (tpb / 6))
                m1.add_note(tr, tick, tpb // 8, 42, velocity=70 + r * 6)
    m1.save(os.path.join(midi_dir, "MIDI_HiHat_Roll_140BPM_Trap.mid"))

    # 2. MIDI_HiHat_Bounce_142BPM_Drill.mid
    m2 = MidiWriter(ticks_per_beat=tpb)
    tr = m2.create_track()
    m2.add_tempo(tr, 0, 142)
    for bar in range(4):
        base = bar * 4 * tpb
        drill_steps = [0.0, 0.75, 1.5, 2.0, 2.75, 3.5]
        for s in drill_steps:
            tick = base + int(s * tpb)
            m2.add_note(tr, tick, tpb // 4, 42, velocity=90)
    m2.save(os.path.join(midi_dir, "MIDI_HiHat_Bounce_142BPM_Drill.mid"))

    # 3. MIDI_808_Trap_Pattern_Cm.mid
    m3 = MidiWriter(ticks_per_beat=tpb)
    tr = m3.create_track()
    m3.add_tempo(tr, 0, 140)
    # Bassline in C Minor: C1, Eb1, F1, G1
    notes_808 = [
        (0.0, 1.5, 36), (1.5, 1.25, 36), (2.75, 1.25, 39),
        (4.0, 1.5, 36), (5.5, 1.25, 41), (6.75, 0.5, 43), (7.25, 0.75, 39),
        (8.0, 1.5, 36), (9.5, 1.25, 36), (10.75, 1.25, 34),
        (12.0, 1.5, 36), (13.5, 1.25, 41), (14.75, 0.5, 43), (15.25, 0.75, 44)
    ]
    for b_pos, dur_b, note in notes_808:
        tick = int(b_pos * tpb)
        dur = int(dur_b * tpb)
        m3.add_note(tr, tick, dur, note, velocity=105)
    m3.save(os.path.join(midi_dir, "MIDI_808_Trap_Pattern_Cm.mid"))

    # 4. MIDI_Snare_Clap_Trap_140BPM.mid
    m4 = MidiWriter(ticks_per_beat=tpb)
    tr = m4.create_track()
    m4.add_tempo(tr, 0, 140)
    for bar in range(4):
        base = bar * 4 * tpb
        # Clap on beat 3 (beat index 2.0)
        m4.add_note(tr, base + 2 * tpb, tpb // 2, 39, velocity=100) # Clap
        # Ghost snare on beat 3.75
        m4.add_note(tr, base + int(3.75 * tpb), tpb // 4, 38, velocity=65) # Ghost snare
    m4.save(os.path.join(midi_dir, "MIDI_Snare_Clap_Trap_140BPM.mid"))

    # 5. MIDI_Reggaeton_Dembow_Groove.mid
    m5 = MidiWriter(ticks_per_beat=tpb)
    tr = m5.create_track()
    m5.add_tempo(tr, 0, 96)
    for bar in range(4):
        base = bar * 4 * tpb
        for b in range(4):
            # Kick (note 36) on beat
            m5.add_note(tr, base + b * tpb, tpb // 2, 36, velocity=105)
            # Snare (note 38) syncopated
            m5.add_note(tr, base + int((b + 0.75) * tpb), tpb // 3, 38, velocity=100)
            if b % 2 == 1:
                m5.add_note(tr, base + int((b - 0.5) * tpb), tpb // 3, 38, velocity=90)
    m5.save(os.path.join(midi_dir, "MIDI_Reggaeton_Dembow_Groove.mid"))

    # 6. MIDI_Melody_Astroworld_Cm.mid
    m6 = MidiWriter(ticks_per_beat=tpb)
    tr = m6.create_track()
    m6.add_tempo(tr, 0, 140)
    chords = [
        (0.0, 4.0, [60, 63, 67, 72]),
        (4.0, 4.0, [56, 60, 63, 68]),
        (8.0, 4.0, [63, 67, 70, 75]),
        (12.0, 4.0, [58, 62, 65, 70])
    ]
    for b_pos, dur_b, notes in chords:
        tick = int(b_pos * tpb)
        dur = int(dur_b * tpb)
        for n in notes:
            m6.add_note(tr, tick, dur, n, velocity=90)
    m6.save(os.path.join(midi_dir, "MIDI_Melody_Astroworld_Cm.mid"))

    # 7. MIDI_Melody_Tokyo_Pluck_Am.mid
    m7 = MidiWriter(ticks_per_beat=tpb)
    tr = m7.create_track()
    m7.add_tempo(tr, 0, 130)
    events_tokyo = [
        (0.0, 69), (0.75, 72), (1.5, 74), (2.25, 76), (3.0, 74), (3.5, 72),
        (4.0, 69), (4.75, 72), (5.5, 76), (6.0, 79), (7.0, 76), (7.5, 74),
        (8.0, 69), (8.75, 72), (9.5, 74), (10.25, 76), (11.0, 74), (11.5, 72),
        (12.0, 76), (13.0, 74), (14.0, 72), (15.0, 69)
    ]
    for b_pos, n in events_tokyo:
        tick = int(b_pos * tpb)
        m7.add_note(tr, tick, int(tpb * 0.75), n, velocity=95)
    m7.save(os.path.join(midi_dir, "MIDI_Melody_Tokyo_Pluck_Am.mid"))

    # 8. MIDI_Melody_Drill_Piano_Em.mid
    m8 = MidiWriter(ticks_per_beat=tpb)
    tr = m8.create_track()
    m8.add_tempo(tr, 0, 142)
    events_drill = [
        (0.0, 52, 2.0), (0.0, 64, 0.5), (0.5, 67, 0.5), (1.0, 70, 0.5), (1.5, 71, 0.5),
        (2.0, 52, 2.0), (2.0, 70, 0.5), (2.5, 67, 0.5), (3.0, 66, 0.5), (3.5, 65, 0.5),
        (4.0, 53, 2.0), (4.0, 65, 0.5), (4.5, 68, 0.5), (5.0, 71, 0.5), (5.5, 72, 0.5),
        (6.0, 53, 2.0), (6.0, 71, 0.5), (6.5, 68, 0.5), (7.0, 67, 0.5), (7.5, 66, 0.5),
        (8.0, 52, 2.0), (8.0, 64, 0.5), (8.5, 67, 0.5), (9.0, 70, 0.5), (9.5, 71, 0.5),
        (10.0, 52, 2.0), (10.0, 70, 0.5), (10.5, 67, 0.5), (11.0, 66, 0.5), (11.5, 65, 0.5),
        (12.0, 55, 2.0), (12.0, 67, 0.5), (12.5, 70, 0.5), (13.0, 71, 0.5), (13.5, 74, 0.5),
        (14.0, 51, 2.0), (14.0, 71, 0.5), (14.5, 70, 0.5), (15.0, 67, 0.5), (15.5, 64, 0.5)
    ]
    for b_pos, n, dur_b in events_drill:
        tick = int(b_pos * tpb)
        dur = int(dur_b * tpb)
        m8.add_note(tr, tick, dur, n, velocity=90)
    m8.save(os.path.join(midi_dir, "MIDI_Melody_Drill_Piano_Em.mid"))

    print(f"Generated 8 MIDI files in {midi_dir}")

if __name__ == "__main__":
    generate_all_midis(r"c:\Users\alfaswz\.gemini\antigravity-ide\scratch\agentes-personalizados\SUPREME-PRODUCER-DRUM-KIT\10_MIDI_Patterns")
