import os
import math
import numpy as np
from scipy import signal
from wav_writer import write_wav
from midi_writer import MidiWriter
from dsp_utils import butter_lowpass, butter_highpass, apply_reverb_stereo
import generators as gen

SR = 44100

def note_to_freq(midi_note):
    return 440.0 * (2.0 ** ((midi_note - 69) / 12.0))

# -------------------------------------------------------------
# Melody Synthesis Engine
# -------------------------------------------------------------
def synth_pluck_voice(midi_note, duration, decay=0.8):
    freq = note_to_freq(midi_note)
    t = np.linspace(0, duration, int(SR * duration), endpoint=False)
    # Saw wave with resonant lowpass filter sweep
    saw = signal.sawtooth(2 * np.pi * freq * t)
    # Filter envelope
    amp = np.exp(-t / decay)
    return saw * amp

def synth_super_saw(midi_note, duration, detune_cents=[ -12, -4, 0, 4, 12 ]):
    freq = note_to_freq(midi_note)
    t = np.linspace(0, duration, int(SR * duration), endpoint=False)
    sig = np.zeros(len(t))
    for c in detune_cents:
        f = freq * (2.0 ** (c / 1200.0))
        sig += signal.sawtooth(2 * np.pi * f * t)
    sig /= len(detune_cents)
    # Filter envelope
    b, a = butter_lowpass(min(3500.0, freq * 8), SR)
    filtered = signal.lfilter(b, a, sig)
    # Attack and release
    att = int(SR * 0.015)
    env = np.ones(len(t))
    env[:att] = np.linspace(0, 1, att)
    rel = int(SR * 0.05)
    env[-rel:] = np.linspace(1, 0, rel)
    return filtered * env

def synth_piano_note(midi_note, duration):
    freq = note_to_freq(midi_note)
    t = np.linspace(0, duration, int(SR * duration), endpoint=False)
    # Fundamental + harmonics
    sig = (
        np.sin(2 * np.pi * freq * t) * 1.0 +
        np.sin(2 * np.pi * freq * 2 * t) * 0.5 +
        np.sin(2 * np.pi * freq * 3 * t) * 0.25 +
        np.sin(2 * np.pi * freq * 4 * t) * 0.12
    )
    # Percussive attack hammer
    hammer_len = min(int(SR * 0.005), len(t))
    sig[:hammer_len] += np.random.uniform(-1, 1, hammer_len) * 0.3
    decay = np.exp(-t / 1.2)
    return sig * decay

# -------------------------------------------------------------
# 1. Melody 140 BPM Cm - Astroworld Space Synth
# -------------------------------------------------------------
def make_melody_astroworld_140():
    bpm = 140.0
    beats_per_bar = 4
    num_bars = 4
    total_beats = num_bars * beats_per_bar # 16 beats
    bar_dur = (60.0 / bpm) * beats_per_bar
    total_dur = (60.0 / bpm) * total_beats # ~6.857s
    total_samples = int(SR * total_dur)
    
    track = np.zeros(total_samples)
    
    # Chords: Bar 1: Cm (C4, Eb4, G4)
    #         Bar 2: Ab (Ab3, C4, Eb4)
    #         Bar 3: Eb (Eb4, G4, Bb4)
    #         Bar 4: Bb (Bb3, D4, F4)
    chords = [
        (0.0, bar_dur * 0.95, [60, 63, 67, 72]),
        (bar_dur, bar_dur * 0.95, [56, 60, 63, 68]),
        (bar_dur * 2, bar_dur * 0.95, [63, 67, 70, 75]),
        (bar_dur * 3, bar_dur * 0.95, [58, 62, 65, 70])
    ]
    
    for start_t, dur, notes in chords:
        s_idx = int(start_t * SR)
        dur_samples = int(dur * SR)
        for n in notes:
            note_audio = synth_super_saw(n, dur)
            track[s_idx:s_idx + len(note_audio)] += note_audio * 0.25
            
    # Add arp lead line
    beat_dur = 60.0 / bpm
    lead_notes = [
        (0.0, 72), (0.5, 75), (1.0, 74), (1.5, 72),
        (2.0, 75), (2.5, 79), (3.0, 75), (3.5, 74),
        (4.0, 68), (4.5, 72), (5.0, 75), (5.5, 72),
        (6.0, 70), (6.5, 74), (7.0, 77), (7.5, 75)
    ]
    for b_pos, n in lead_notes:
        s_idx = int(b_pos * 2.0 * beat_dur * SR)
        if s_idx < total_samples:
            p = synth_pluck_voice(n, beat_dur * 1.5, decay=0.4)
            end_idx = min(total_samples, s_idx + len(p))
            track[s_idx:end_idx] += p[:end_idx - s_idx] * 0.35

    track = track / (np.max(np.abs(track)) + 1e-6) * 0.90
    return apply_reverb_stereo(track, delay_ms=75, feedback=0.45, wet=0.35), bpm, total_beats, 60 # C4 root

# -------------------------------------------------------------
# 2. Melody 130 BPM Am - Tokyo Night Pluck
# -------------------------------------------------------------
def make_melody_tokyo_pluck_130():
    bpm = 130.0
    num_bars = 4
    total_beats = 16
    total_dur = (60.0 / bpm) * total_beats
    total_samples = int(SR * total_dur)
    beat_dur = 60.0 / bpm
    track = np.zeros(total_samples)
    
    # Oriental Pentatonic minor: A, C, D, E, G (69, 72, 74, 76, 79)
    melody_events = [
        (0.0, 69), (0.75, 72), (1.5, 74), (2.25, 76), (3.0, 74), (3.5, 72),
        (4.0, 69), (4.75, 72), (5.5, 76), (6.0, 79), (7.0, 76), (7.5, 74),
        (8.0, 69), (8.75, 72), (9.5, 74), (10.25, 76), (11.0, 74), (11.5, 72),
        (12.0, 76), (13.0, 74), (14.0, 72), (15.0, 69)
    ]
    
    for b_pos, n in melody_events:
        s_idx = int(b_pos * beat_dur * SR)
        dur = beat_dur * 1.8
        p = synth_pluck_voice(n, dur, decay=0.35)
        # Add high octave bell shimmer
        bell = synth_pluck_voice(n + 12, dur, decay=0.2) * 0.4
        voice = p + bell
        end_idx = min(total_samples, s_idx + len(voice))
        track[s_idx:end_idx] += voice[:end_idx - s_idx] * 0.45
        
    track = track / (np.max(np.abs(track)) + 1e-6) * 0.90
    return apply_reverb_stereo(track, delay_ms=90, feedback=0.4, wet=0.3), bpm, total_beats, 69 # A4 root

# -------------------------------------------------------------
# 3. Melody 96 BPM Fm - Latin Reggaeton Vibes
# -------------------------------------------------------------
def make_melody_reggaeton_96():
    bpm = 96.0
    num_bars = 4
    total_beats = 16
    total_dur = (60.0 / bpm) * total_beats
    total_samples = int(SR * total_dur)
    beat_dur = 60.0 / bpm
    bar_dur = beat_dur * 4
    track = np.zeros(total_samples)
    
    # Progress: Fm -> Bbm -> Eb -> C
    prog = [
        (0.0, [65, 68, 72]),       # Fm
        (bar_dur, [65, 70, 73]),   # Bbm
        (bar_dur * 2, [63, 67, 70]), # Eb
        (bar_dur * 3, [60, 64, 67])  # C maj
    ]
    # Reggaeton offbeat chord stabs
    for start_bar, notes in prog:
        for b_off in [0.5, 1.5, 2.5, 3.5]:
            t_stab = start_bar + b_off * beat_dur
            s_idx = int(t_stab * SR)
            dur = beat_dur * 0.8
            for n in notes:
                stab = synth_pluck_voice(n, dur, decay=0.25)
                end_idx = min(total_samples, s_idx + len(stab))
                track[s_idx:end_idx] += stab[:end_idx - s_idx] * 0.28
                
    track = track / (np.max(np.abs(track)) + 1e-6) * 0.90
    return apply_reverb_stereo(track, delay_ms=65, feedback=0.35, wet=0.25), bpm, total_beats, 65 # F4 root

# -------------------------------------------------------------
# 4. Melody 142 BPM Em - Dark Drill Piano
# -------------------------------------------------------------
def make_melody_drill_piano_142():
    bpm = 142.0
    num_bars = 4
    total_beats = 16
    total_dur = (60.0 / bpm) * total_beats
    total_samples = int(SR * total_dur)
    beat_dur = 60.0 / bpm
    track = np.zeros(total_samples)
    
    # Dark arpeggio with tritones (E minor / diminished: E, G, Bb, B, F)
    events = [
        (0.0, 52, 2.0), (0.0, 64, 0.5), (0.5, 67, 0.5), (1.0, 70, 0.5), (1.5, 71, 0.5),
        (2.0, 52, 2.0), (2.0, 70, 0.5), (2.5, 67, 0.5), (3.0, 66, 0.5), (3.5, 65, 0.5),
        (4.0, 53, 2.0), (4.0, 65, 0.5), (4.5, 68, 0.5), (5.0, 71, 0.5), (5.5, 72, 0.5),
        (6.0, 53, 2.0), (6.0, 71, 0.5), (6.5, 68, 0.5), (7.0, 67, 0.5), (7.5, 66, 0.5),
        (8.0, 52, 2.0), (8.0, 64, 0.5), (8.5, 67, 0.5), (9.0, 70, 0.5), (9.5, 71, 0.5),
        (10.0, 52, 2.0), (10.0, 70, 0.5), (10.5, 67, 0.5), (11.0, 66, 0.5), (11.5, 65, 0.5),
        (12.0, 55, 2.0), (12.0, 67, 0.5), (12.5, 70, 0.5), (13.0, 71, 0.5), (13.5, 74, 0.5),
        (14.0, 51, 2.0), (14.0, 71, 0.5), (14.5, 70, 0.5), (15.0, 67, 0.5), (15.5, 64, 0.5)
    ]
    for b_pos, n, dur_b in events:
        s_idx = int(b_pos * beat_dur * SR)
        dur = dur_b * beat_dur
        p = synth_piano_note(n, dur)
        end_idx = min(total_samples, s_idx + len(p))
        track[s_idx:end_idx] += p[:end_idx - s_idx] * 0.4
        
    track = track / (np.max(np.abs(track)) + 1e-6) * 0.90
    return apply_reverb_stereo(track, delay_ms=100, feedback=0.5, wet=0.35), bpm, total_beats, 64 # E4 root

# -------------------------------------------------------------
# 5. Drum Loops (Tempo-synced loops using our one-shots)
# -------------------------------------------------------------
def make_drum_loops(one_shots):
    kick = one_shots['kick_trap']
    snare = one_shots['snare_trap']
    clap = one_shots['clap_trap']
    hat_c = one_shots['hat_closed']
    hat_o = one_shots['hat_open']
    sub808 = one_shots['808_spinz']
    
    # A) 140 BPM Full Trap Bounce (4 bars)
    bpm = 140.0
    total_beats = 16
    total_dur = (60.0 / bpm) * total_beats
    total_samples = int(SR * total_dur)
    beat_dur = 60.0 / bpm
    
    full_loop = np.zeros(total_samples)
    top_loop = np.zeros(total_samples)
    
    def place(track, audio, beat_pos, gain=1.0):
        s_idx = int(beat_pos * beat_dur * SR)
        if s_idx < total_samples:
            end_idx = min(total_samples, s_idx + len(audio))
            track[s_idx:end_idx] += audio[:end_idx - s_idx] * gain

    # Hi-Hats: 8th notes with triplet rolls on beat 2.5 and 3.5
    for bar in range(4):
        b_base = bar * 4
        # 8th notes
        for step in range(8):
            pos = b_base + step * 0.5
            place(top_loop, hat_c, pos, gain=0.6 if step % 2 == 0 else 0.45)
        # Roll in bar 2 and 4 (1/16th triplets)
        if bar % 2 == 1:
            for r in range(6):
                pos = b_base + 3.0 + r * (1.0 / 6.0)
                place(top_loop, hat_c, pos, gain=0.55 + r * 0.05)
        # Open hat on upbeat 1.5
        place(top_loop, hat_o, b_base + 1.5, gain=0.5)
        # Clap on beat 3
        place(top_loop, clap, b_base + 2.0, gain=0.9)
        # Ghost snare on beat 3.75
        place(top_loop, snare, b_base + 3.75, gain=0.4)

    # Add Kick and 808 to Full Loop
    full_loop += top_loop * 0.75
    # Kicks
    kick_positions = [
        0.0, 1.5, 2.75,
        4.0, 5.5, 6.75, 7.25,
        8.0, 9.5, 10.75,
        12.0, 13.5, 14.75, 15.25
    ]
    for kp in kick_positions:
        place(full_loop, kick, kp, gain=0.95)
        
    # 808s (tuned to kick)
    b808_positions = [0.0, 4.0, 8.0, 12.0]
    for bp in b808_positions:
        place(full_loop, sub808, bp, gain=0.85)

    full_loop = full_loop / (np.max(np.abs(full_loop)) + 1e-6) * 0.95
    top_loop = top_loop / (np.max(np.abs(top_loop)) + 1e-6) * 0.95
    
    # B) 96 BPM Reggaeton Dembow Loop (4 bars)
    bpm_reg = 96.0
    total_beats_reg = 16
    total_dur_reg = (60.0 / bpm_reg) * total_beats_reg
    total_samples_reg = int(SR * total_dur_reg)
    beat_dur_reg = 60.0 / bpm_reg
    
    reg_loop = np.zeros(total_samples_reg)
    kick_reg = one_shots['kick_dembow']
    snare_reg = one_shots['snare_reggaeton']
    shaker_reg = one_shots['perc_shaker']
    
    def place_reg(track, audio, beat_pos, gain=1.0):
        s_idx = int(beat_pos * beat_dur_reg * SR)
        if s_idx < total_samples_reg:
            end_idx = min(total_samples_reg, s_idx + len(audio))
            track[s_idx:end_idx] += audio[:end_idx - s_idx] * gain

    for b in range(16):
        # 4-on-the-floor kick
        place_reg(reg_loop, kick_reg, b, gain=0.95)
        # Dembow syncopated snares on beat + 0.75
        place_reg(reg_loop, snare_reg, b + 0.75, gain=0.85)
        # Extra snare on 2nd and 4th beats + 0.5
        if b % 2 == 1:
            place_reg(reg_loop, snare_reg, b - 0.5, gain=0.8)
        # Shaker 16th notes
        for s in range(4):
            place_reg(reg_loop, shaker_reg, b + s * 0.25, gain=0.45 if s % 2 == 0 else 0.3)

    reg_loop = reg_loop / (np.max(np.abs(reg_loop)) + 1e-6) * 0.95

    # C) 142 BPM UK/NY Drill Loop (4 bars)
    bpm_drill = 142.0
    total_beats_drill = 16
    total_dur_drill = (60.0 / bpm_drill) * total_beats_drill
    total_samples_drill = int(SR * total_dur_drill)
    beat_dur_drill = 60.0 / bpm_drill
    drill_loop = np.zeros(total_samples_drill)
    
    kick_drill = one_shots['kick_drill']
    snare_drill = one_shots['snare_drill']
    b808_slide = one_shots['808_drill']
    
    def place_drill(track, audio, beat_pos, gain=1.0):
        s_idx = int(beat_pos * beat_dur_drill * SR)
        if s_idx < total_samples_drill:
            end_idx = min(total_samples_drill, s_idx + len(audio))
            track[s_idx:end_idx] += audio[:end_idx - s_idx] * gain

    # Drill syncopated pattern
    drill_kicks = [0.0, 1.75, 4.0, 5.75, 7.25, 8.0, 9.75, 12.0, 13.75, 15.25]
    for dk in drill_kicks:
        place_drill(drill_loop, kick_drill, dk, gain=0.95)
    # Snares on beat 3 (beat 2 in 4/4 count, but bar beat 3 in half-time = 2.0 and 6.0 etc)
    for bar in range(4):
        place_drill(drill_loop, snare_drill, bar * 4 + 2.0, gain=0.9)
        # Ghost snare on offbeat
        place_drill(drill_loop, snare_drill, bar * 4 + 3.66, gain=0.45)
    # Hi-hats with drill skip: steps 0, 1.5, 3
    for bar in range(4):
        for step in [0.0, 0.75, 1.5, 2.0, 2.75, 3.5]:
            place_drill(drill_loop, hat_c, bar * 4 + step, gain=0.6)
            
    # Add sliding 808
    place_drill(drill_loop, b808_slide, 0.0, gain=0.85)
    place_drill(drill_loop, b808_slide, 8.0, gain=0.85)

    drill_loop = drill_loop / (np.max(np.abs(drill_loop)) + 1e-6) * 0.95

    return {
        'trap_full_140': (apply_reverb_stereo(full_loop, delay_ms=30, feedback=0.2, wet=0.1), 140.0, 16),
        'trap_top_140': (apply_reverb_stereo(top_loop, delay_ms=30, feedback=0.2, wet=0.1), 140.0, 16),
        'reggaeton_96': (apply_reverb_stereo(reg_loop, delay_ms=30, feedback=0.2, wet=0.1), 96.0, 16),
        'drill_142': (apply_reverb_stereo(drill_loop, delay_ms=30, feedback=0.2, wet=0.1), 142.0, 16)
    }

print("Loop maker ready.")
