import os
import numpy as np
from wav_writer import write_wav
import advanced_melody_engine as ame

SR = 44100

def render_melody(bpm, num_bars, chord_events, lead_events, bass_events, title, filename, out_dir, root_note=60):
    total_beats = num_bars * 4
    beat_dur = 60.0 / bpm
    total_dur = beat_dur * total_beats
    total_samples = int(SR * total_dur)
    
    mix = np.zeros(total_samples)
    
    # 1. Chords / Pad layer
    for start_b, dur_b, notes, inst_type, vel in chord_events:
        s_idx = int(start_b * beat_dur * SR)
        dur_s = dur_b * beat_dur
        for n in notes:
            if inst_type == 'rhodes':
                audio = ame.sound_rhodes(n, dur_s, vel)
            elif inst_type == 'pad':
                audio = ame.sound_analog_pad(n, dur_s, vel)
            elif inst_type == 'guitar':
                audio = ame.sound_guitar(n, dur_s, vel)
            else:
                audio = ame.sound_bell_pluck(n, dur_s, vel)
            
            end_idx = min(total_samples, s_idx + len(audio))
            mix[s_idx:end_idx] += audio[:end_idx - s_idx] * 0.35
            
    # 2. Lead hook layer
    for start_b, dur_b, n, inst_type, vel in lead_events:
        s_idx = int(start_b * beat_dur * SR)
        dur_s = dur_b * beat_dur
        if inst_type == 'flute':
            audio = ame.sound_flute(n, dur_s, vel)
        elif inst_type == 'guitar':
            audio = ame.sound_guitar(n, dur_s, vel)
        elif inst_type == 'rhodes':
            audio = ame.sound_rhodes(n, dur_s, vel)
        else:
            audio = ame.sound_bell_pluck(n, dur_s, vel)
            
        end_idx = min(total_samples, s_idx + len(audio))
        mix[s_idx:end_idx] += audio[:end_idx - s_idx] * 0.45
        
    # 3. Bass root layer
    for start_b, dur_b, n, vel in bass_events:
        s_idx = int(start_b * beat_dur * SR)
        dur_s = dur_b * beat_dur
        audio = ame.sound_sub_bass(n, dur_s, vel)
        end_idx = min(total_samples, s_idx + len(audio))
        mix[s_idx:end_idx] += audio[:end_idx - s_idx] * 0.4
        
    # Apply warm studio reverb & post-processing
    processed = ame.post_process_stereo(mix, wet=0.28, decay=0.55, cutoff=3200.0)
    
    filepath = os.path.join(out_dir, filename)
    write_wav(filepath, processed, sample_rate=SR, tempo=bpm, num_beats=total_beats, root_note=root_note, title=title)
    print(f"  [+] Rendered: {filename} ({bpm} BPM, {num_bars} bars)")

print("Melody renderer ready.")
