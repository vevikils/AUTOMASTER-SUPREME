import os
import math
import numpy as np
from scipy import signal
from wav_writer import write_wav
import generators as gen
from dsp_utils import butter_lowpass, butter_highpass, butter_bandpass

SR = 44100

class DrumGrooveRenderer:
    def __init__(self):
        # Pre-cache one-shots
        print("[*] Initializing drum groove samples...")
        self.k_trap = gen.make_kick(0.45, 210, 52, click_amt=0.45)
        self.k_dembow = gen.make_kick(0.38, 230, 58, click_amt=0.35, punch_decay=0.10)
        self.k_drill = gen.make_kick(0.35, 260, 55, click_amt=0.55, punch_decay=0.08)
        self.k_boombap = gen.make_kick(0.50, 160, 48, click_amt=0.25, punch_decay=0.18)
        self.k_afro = gen.make_kick(0.40, 200, 54, click_amt=0.35, punch_decay=0.12)
        
        self.s_trap = gen.make_snare(0.35, 185, 0.22)
        self.s_reggaeton = gen.make_snare(0.32, 210, 0.19, snappy_lo=900, snappy_hi=6500, tone_amt=0.45)
        self.s_drill = gen.make_snare(0.28, 240, 0.16, snappy_lo=1200, snappy_hi=7500, tone_amt=0.5)
        self.s_boombap = gen.make_snare(0.40, 175, 0.28, tone_amt=0.4)
        self.rim = gen.make_rimshot(0.12, 980.0)
        
        self.clap_trap = gen.make_clap(0.40, 3, 13.0)
        self.clap_wide = gen.make_clap(0.42, 4, 11.0, is_stereo=False)
        
        self.hat_c = gen.make_hihat(0.065, False, 0.035)
        self.hat_drill = gen.make_hihat(0.045, False, 0.022)
        self.hat_o = gen.make_hihat(0.55, True, 0.35)
        self.shaker = gen.make_shaker(0.14)
        self.cowbell = gen.make_cowbell(0.32)
        self.bongo = gen.make_bongo(0.25)
        
        self.sub808_spinz = gen.make_808(2.2, 32.7, drive=2.8, decay=1.8)
        self.sub808_drill = gen.make_808(2.2, 32.7, drive=3.5, decay=1.7, is_slide=True)
        self.sub_heavy = gen.make_808(2.5, 32.7, attack_pitch=110, drive=1.5, decay=2.1)

    def render_loop(self, bpm, num_bars, events, filename, out_dir, title):
        total_beats = num_bars * 4
        beat_dur = 60.0 / bpm
        total_dur = beat_dur * total_beats
        total_samples = int(SR * total_dur)
        
        track_l = np.zeros(total_samples)
        track_r = np.zeros(total_samples)
        
        for sample_type, beat_pos, gain, pan in events:
            # Map sample
            if sample_type == 'k_trap': s = self.k_trap
            elif sample_type == 'k_dem': s = self.k_dembow
            elif sample_type == 'k_drill': s = self.k_drill
            elif sample_type == 'k_boom': s = self.k_boombap
            elif sample_type == 'k_afro': s = self.k_afro
            elif sample_type == 's_trap': s = self.s_trap
            elif sample_type == 's_reg': s = self.s_reggaeton
            elif sample_type == 's_drill': s = self.s_drill
            elif sample_type == 's_boom': s = self.s_boombap
            elif sample_type == 'rim': s = self.rim
            elif sample_type == 'c_trap': s = self.clap_trap
            elif sample_type == 'c_wide': s = self.clap_wide
            elif sample_type == 'h_c': s = self.hat_c
            elif sample_type == 'h_drill': s = self.hat_drill
            elif sample_type == 'h_o': s = self.hat_o
            elif sample_type == 'shaker': s = self.shaker
            elif sample_type == 'cowbell': s = self.cowbell
            elif sample_type == 'bongo': s = self.bongo
            elif sample_type == '808_spinz': s = self.sub808_spinz
            elif sample_type == '808_drill': s = self.sub808_drill
            elif sample_type == '808_sub': s = self.sub_heavy
            else: s = self.hat_c
            
            s_idx = int(beat_pos * beat_dur * SR)
            if s_idx < total_samples:
                end_idx = min(total_samples, s_idx + len(s))
                # Pan: -1 (left) to +1 (right)
                gain_l = gain * math.cos((pan + 1.0) * math.pi / 4.0)
                gain_r = gain * math.sin((pan + 1.0) * math.pi / 4.0)
                
                track_l[s_idx:end_idx] += s[:end_idx - s_idx] * gain_l
                track_r[s_idx:end_idx] += s[:end_idx - s_idx] * gain_r
                
        # Warm mastering: gentle lowpass at 18kHz, soft clip
        b, a = signal.butter(2, 17500.0 / (SR * 0.5), btype='low')
        track_l = signal.lfilter(b, a, track_l)
        track_r = signal.lfilter(b, a, track_r)
        
        # Soft tape glue
        track_l = np.tanh(1.25 * track_l)
        track_r = np.tanh(1.25 * track_r)
        
        stereo = np.column_stack([track_l, track_r])
        mx = np.max(np.abs(stereo)) + 1e-6
        stereo = stereo / mx * 0.94
        
        filepath = os.path.join(out_dir, filename)
        write_wav(filepath, stereo, sample_rate=SR, tempo=bpm, num_beats=total_beats, root_note=60, title=title)
        print(f"  [+] Drum Loop: {filename} ({bpm} BPM)")

print("DrumGrooveRenderer ready.")
