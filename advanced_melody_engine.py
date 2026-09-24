import os
import math
import numpy as np
from scipy import signal
from wav_writer import write_wav
import generators as gen
from dsp_utils import butter_bandpass, butter_lowpass, butter_highpass

SR = 44100

def note_freq(midi):
    return 440.0 * (2.0 ** ((midi - 69.0) / 12.0))

# -------------------------------------------------------------
# Warm Studio Reverb & EQ Post-Processor
# -------------------------------------------------------------
def post_process_stereo(audio_mono, wet=0.28, decay=0.55, cutoff=3200.0, lofi_warmth=True):
    # Freeverb-style stereo reverb
    delays_l = [int(SR * d) for d in [0.029, 0.037, 0.041, 0.045]]
    delays_r = [int(SR * d) for d in [0.031, 0.039, 0.043, 0.047]]
    
    n_samples = len(audio_mono)
    out_l = np.zeros(n_samples)
    out_r = np.zeros(n_samples)
    
    # Lowpass filter for warm reverb damping (no screechy highs)
    b_damp, a_damp = signal.butter(1, min(cutoff, 4000.0) / (SR * 0.5), btype='low')
    
    for d_l, d_r in zip(delays_l, delays_r):
        buf_l = np.zeros(n_samples)
        buf_r = np.zeros(n_samples)
        fb = 0.55 * decay
        for i in range(0, n_samples, d_l):
            chunk = min(d_l, n_samples - i)
            if i + chunk + d_l < n_samples:
                buf_l[i + d_l : i + d_l + chunk] += audio_mono[i : i + chunk] * fb
        for i in range(0, n_samples, d_r):
            chunk = min(d_r, n_samples - i)
            if i + chunk + d_r < n_samples:
                buf_r[i + d_r : i + d_r + chunk] += audio_mono[i : i + chunk] * fb
        out_l += buf_l
        out_r += buf_r
        
    out_l = signal.lfilter(b_damp, a_damp, out_l)
    out_r = signal.lfilter(b_damp, a_damp, out_r)
    
    # Warm Post-EQ: Cut piercing frequencies > 6.5kHz, gently warm 250-500Hz
    stereo_l = audio_mono * (1.0 - wet) + out_l * wet
    stereo_r = audio_mono * (1.0 - wet) + out_r * wet
    
    if lofi_warmth:
        b_warm, a_warm = signal.butter(2, 6500.0 / (SR * 0.5), btype='low')
        stereo_l = signal.lfilter(b_warm, a_warm, stereo_l)
        stereo_r = signal.lfilter(b_warm, a_warm, stereo_r)
        
        # Sub-rumble highpass
        b_hp, a_hp = signal.butter(1, 35.0 / (SR * 0.5), btype='high')
        stereo_l = signal.lfilter(b_hp, a_hp, stereo_l)
        stereo_r = signal.lfilter(b_hp, a_hp, stereo_r)
        
    # Soft analog tape glue
    stereo_l = np.tanh(1.2 * stereo_l)
    stereo_r = np.tanh(1.2 * stereo_r)
    
    combined = np.column_stack([stereo_l, stereo_r])
    mx = np.max(np.abs(combined)) + 1e-6
    return combined / mx * 0.92

# -------------------------------------------------------------
# Studio Physical & Analog Instruments (Warm, non-abrasive)
# -------------------------------------------------------------
def sound_rhodes(midi, dur, vel=1.0):
    f = note_freq(midi)
    t = np.linspace(0, dur, int(SR * dur), endpoint=False)
    # Fundamental sine + second harmonic + fast tine strike
    fund = np.sin(2 * np.pi * f * t) * np.exp(-t / 1.6)
    h2 = np.sin(2 * np.pi * f * 2 * t) * 0.35 * np.exp(-t / 1.1)
    tine = np.sin(2 * np.pi * f * 4.15 * t) * 0.2 * np.exp(-t / 0.15)
    sig = (fund + h2 + tine) * vel
    # Soft lowpass
    b, a = signal.butter(2, min(3200.0, f * 6) / (SR * 0.5), btype='low')
    sig = signal.lfilter(b, a, sig)
    # Anti-click envelope
    att = int(SR * 0.005)
    sig[:att] *= np.linspace(0, 1, att)
    rel = int(SR * 0.04)
    sig[-rel:] *= np.linspace(1, 0, rel)
    return sig

def sound_guitar(midi, dur, vel=1.0):
    f = note_freq(midi)
    n_samples = int(SR * dur)
    period = max(2, int(SR / f))
    np.random.seed(int(midi * 100) % 10000)
    noise = np.random.uniform(-1, 1, period)
    # Soft pick lowpass
    b, a = signal.butter(2, 0.35, btype='low')
    noise = signal.lfilter(b, a, noise)
    out = np.zeros(n_samples)
    out[:period] = noise
    decay_factor = 0.988 - (f / 20000.0)
    for i in range(period, n_samples):
        out[i] = (out[i - period] + out[i - period - 1]) * 0.5 * decay_factor
    rel = int(SR * 0.04)
    out[-rel:] *= np.linspace(1, 0, rel)
    return out * vel

def sound_analog_pad(midi, dur, vel=0.7):
    f = note_freq(midi)
    t = np.linspace(0, dur, int(SR * dur), endpoint=False)
    # Dual detuned saws with slow warm filter
    s1 = signal.sawtooth(2 * np.pi * f * (2.0 ** (-5 / 1200)) * t)
    s2 = signal.sawtooth(2 * np.pi * f * (2.0 ** (5 / 1200)) * t)
    s3 = np.sin(2 * np.pi * (f * 0.5) * t) * 0.4 # Sub octave body
    sig = (s1 + s2 + s3) * 0.4
    b, a = signal.butter(2, min(1800.0, f * 4) / (SR * 0.5), btype='low')
    sig = signal.lfilter(b, a, sig)
    # Swell attack and smooth release
    att = int(SR * 0.06)
    sig[:att] *= np.linspace(0, 1, att)
    rel = int(SR * 0.08)
    sig[-rel:] *= np.linspace(1, 0, rel)
    return sig * vel

def sound_flute(midi, dur, vel=0.8):
    f = note_freq(midi)
    t = np.linspace(0, dur, int(SR * dur), endpoint=False)
    # Vibrato LFO
    vib = 1.0 + 0.012 * np.sin(2 * np.pi * 5.2 * t) * (1.0 - np.exp(-t / 0.25))
    phase = 2 * np.pi * np.cumsum(f * vib) / SR
    tone = np.sin(phase) + 0.18 * np.sin(2 * phase)
    # Warm breath
    np.random.seed(int(f) % 777)
    noise = np.random.uniform(-1, 1, len(t))
    b, a = signal.butter(2, [1400 / (SR * 0.5), 2800 / (SR * 0.5)], btype='band')
    breath = signal.lfilter(b, a, noise) * 0.09
    sig = (tone + breath) * vel
    att = int(SR * 0.035)
    sig[:att] *= np.linspace(0, 1, att)
    rel = int(SR * 0.06)
    sig[-rel:] *= np.linspace(1, 0, rel)
    return sig

def sound_bell_pluck(midi, dur, vel=0.75):
    f = note_freq(midi)
    t = np.linspace(0, dur, int(SR * dur), endpoint=False)
    # Sine fundamental + pure 3rd harmonic + sub
    tone = np.sin(2 * np.pi * f * t) * np.exp(-t / 0.8)
    h3 = np.sin(2 * np.pi * f * 3 * t) * 0.25 * np.exp(-t / 0.3)
    sig = (tone + h3) * vel
    b, a = signal.butter(2, min(4500.0, f * 5) / (SR * 0.5), btype='low')
    sig = signal.lfilter(b, a, sig)
    att = int(SR * 0.003)
    sig[:att] *= np.linspace(0, 1, att)
    rel = int(SR * 0.04)
    sig[-rel:] *= np.linspace(1, 0, rel)
    return sig

def sound_sub_bass(midi, dur, vel=0.85):
    f = note_freq(midi)
    t = np.linspace(0, dur, int(SR * dur), endpoint=False)
    sig = np.sin(2 * np.pi * f * t) + 0.2 * np.sin(2 * np.pi * f * 2 * t)
    sig = np.tanh(1.5 * sig) * vel
    b, a = signal.butter(2, 350.0 / (SR * 0.5), btype='low')
    sig = signal.lfilter(b, a, sig)
    att = int(SR * 0.01)
    sig[:att] *= np.linspace(0, 1, att)
    rel = int(SR * 0.05)
    sig[-rel:] *= np.linspace(1, 0, rel)
    return sig

print("Advanced sound engines loaded.")
