import os
import math
import numpy as np
from scipy import signal
from wav_writer import write_wav
from master_drum_soundbank import MasterSoundbank

SR = 44100

class HitLoopRenderer:
    def __init__(self):
        self.bank = MasterSoundbank()

    def render(self, bpm, num_bars, events, filename, out_dir, title):
        total_beats = num_bars * 4
        beat_dur = 60.0 / bpm
        total_dur = beat_dur * total_beats
        total_samples = int(SR * total_dur)
        
        track_l = np.zeros(total_samples)
        track_r = np.zeros(total_samples)
        
        for sample_name, beat_pos, gain, pan in events:
            if sample_name not in self.bank.samples:
                continue
            s = self.bank.samples[sample_name]
            
            s_idx = int(beat_pos * beat_dur * SR)
            if s_idx < total_samples:
                end_idx = min(total_samples, s_idx + len(s))
                # Pan law: -1.0 (L) to +1.0 (R)
                gain_l = gain * math.cos((pan + 1.0) * math.pi / 4.0)
                gain_r = gain * math.sin((pan + 1.0) * math.pi / 4.0)
                
                track_l[s_idx:end_idx] += s[:end_idx - s_idx] * gain_l
                track_r[s_idx:end_idx] += s[:end_idx - s_idx] * gain_r

        # Warm bus processing: Cut ultrasonic harshness > 18kHz, soft tape clip
        b, a = signal.butter(2, 17500.0 / (SR * 0.5), btype='low')
        track_l = signal.lfilter(b, a, track_l)
        track_r = signal.lfilter(b, a, track_r)
        
        track_l = np.tanh(1.25 * track_l)
        track_r = np.tanh(1.25 * track_r)
        
        stereo = np.column_stack([track_l, track_r])
        mx = np.max(np.abs(stereo)) + 1e-6
        stereo = stereo / mx * 0.94
        
        filepath = os.path.join(out_dir, filename)
        os.makedirs(out_dir, exist_ok=True)
        write_wav(filepath, stereo, sample_rate=SR, tempo=bpm, num_beats=total_beats, root_note=60, title=title)
        print(f"  [+] {filename} ({bpm} BPM)")

print("HitLoopRenderer ready.")
