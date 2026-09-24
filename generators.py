import os
import math
import struct
import numpy as np
from scipy import signal
from wav_writer import write_wav
from midi_writer import MidiWriter
from dsp_utils import butter_bandpass, butter_lowpass, butter_highpass, apply_reverb_stereo

SR = 44100
KIT_DIR = r"c:\Users\alfaswz\.gemini\antigravity-ide\scratch\agentes-personalizados\SUPREME-PRODUCER-DRUM-KIT"

# -------------------------------------------------------------
# 1. 808s & Sub Basses (All tuned to C so producers can play any scale)
# -------------------------------------------------------------
def make_808(duration=2.2, fund=32.703, attack_pitch=160.0, pitch_decay=0.035, drive=2.5, decay=1.8, is_slide=False):
    t = np.linspace(0, duration, int(SR * duration), endpoint=False)
    
    # Pitch envelope
    if is_slide:
        # Slide from C up to G then back to C (Drill style)
        freq_env = fund + (attack_pitch - fund) * np.exp(-t / pitch_decay)
        # Add slide mid-way (around 0.5s to 1.2s slide up to G 49Hz)
        slide_mask = (t >= 0.5) & (t <= 1.5)
        freq_env[slide_mask] += (49.0 - fund) * np.sin(np.pi * (t[slide_mask] - 0.5) / 1.0)
    else:
        freq_env = fund + (attack_pitch - fund) * np.exp(-t / pitch_decay)
        
    phase = 2.0 * np.pi * np.cumsum(freq_env) / SR
    
    # Rich harmonics
    raw = np.sin(phase) + 0.35 * np.sin(2.0 * phase) + 0.15 * np.sin(3.0 * phase)
    
    # Drive / Soft-clipping (analog saturation)
    sat = np.tanh(drive * raw)
    
    # Amplitude envelope
    amp_env = np.exp(-t / decay)
    # Fast attack
    attack_samples = int(SR * 0.004)
    amp_env[:attack_samples] *= np.linspace(0, 1, attack_samples)
    # Smooth release at end
    rel_samples = int(SR * 0.05)
    amp_env[-rel_samples:] *= np.linspace(1, 0, rel_samples)
    
    out = sat * amp_env
    # Lowpass filter around 800Hz
    b, a = butter_lowpass(800.0, SR)
    out = signal.lfilter(b, a, out)
    return out / (np.max(np.abs(out)) + 1e-6) * 0.95

# -------------------------------------------------------------
# 2. Kicks (Punchy transient + tight sub body)
# -------------------------------------------------------------
def make_kick(duration=0.45, start_f=200.0, end_f=52.0, p_decay=0.035, click_amt=0.35, punch_decay=0.12):
    t = np.linspace(0, duration, int(SR * duration), endpoint=False)
    
    freq_env = end_f + (start_f - end_f) * np.exp(-t / p_decay)
    phase = 2.0 * np.pi * np.cumsum(freq_env) / SR
    sub_body = np.sin(phase)
    
    # Click transient layer (first 5ms)
    click_t = t[:int(SR * 0.006)]
    click = np.sin(2.0 * np.pi * 1800.0 * click_t) * np.exp(-click_t / 0.0015)
    
    amp_env = np.exp(-t / punch_decay)
    # Smooth tail
    rel_samples = int(SR * 0.03)
    amp_env[-rel_samples:] *= np.linspace(1, 0, rel_samples)
    
    out = sub_body * amp_env
    out[:len(click)] += click * click_amt
    
    # Saturation
    out = np.tanh(1.6 * out)
    return out / (np.max(np.abs(out)) + 1e-6) * 0.96

# -------------------------------------------------------------
# 3. Snares & Rims
# -------------------------------------------------------------
def make_snare(duration=0.35, tone_f=185.0, noise_decay=0.22, snappy_lo=800.0, snappy_hi=6000.0, tone_amt=0.35):
    t = np.linspace(0, duration, int(SR * duration), endpoint=False)
    
    # Drum tone (sine with quick pitch drop)
    f_env = tone_f + 160.0 * np.exp(-t / 0.015)
    tone = np.sin(2.0 * np.pi * np.cumsum(f_env) / SR) * np.exp(-t / 0.07)
    
    # Snappy noise
    np.random.seed(42)
    noise = np.random.uniform(-1, 1, len(t))
    b, a = butter_bandpass(snappy_lo, snappy_hi, SR)
    filtered_noise = signal.lfilter(b, a, noise) * np.exp(-t / noise_decay)
    
    # Initial crack pop (1ms)
    pop_len = int(SR * 0.002)
    filtered_noise[:pop_len] += np.linspace(1, 0, pop_len) * 0.8
    
    out = tone * tone_amt + filtered_noise * (1.0 - tone_amt)
    out = np.tanh(1.5 * out)
    return out / (np.max(np.abs(out)) + 1e-6) * 0.95

def make_rimshot(duration=0.12, freq=980.0):
    t = np.linspace(0, duration, int(SR * duration), endpoint=False)
    # High resonant ping + wood knock
    ping = np.sin(2.0 * np.pi * freq * t) * np.exp(-t / 0.025)
    wood = np.sin(2.0 * np.pi * 420.0 * t) * np.exp(-t / 0.015)
    # Click
    click = np.random.uniform(-1, 1, len(t))
    b, a = butter_bandpass(2000, 8000, SR)
    click = signal.lfilter(b, a, click) * np.exp(-t / 0.008)
    
    out = ping * 0.5 + wood * 0.3 + click * 0.4
    return out / (np.max(np.abs(out)) + 1e-6) * 0.95

# -------------------------------------------------------------
# 4. Claps
# -------------------------------------------------------------
def make_clap(duration=0.4, pre_bursts=3, burst_space_ms=13.0, is_stereo=False):
    total_samples = int(SR * duration)
    out = np.zeros(total_samples)
    
    np.random.seed(1337)
    b, a = butter_bandpass(900.0, 3200.0, SR)
    
    # Pre bursts
    burst_samples = int(SR * 0.004)
    spacing = int(SR * (burst_space_ms / 1000.0))
    
    for i in range(pre_bursts):
        idx = i * spacing
        b_noise = np.random.uniform(-1, 1, burst_samples) * np.exp(-np.linspace(0, 1, burst_samples) * 5)
        out[idx:idx + burst_samples] += b_noise * (0.6 + i * 0.15)
        
    # Main sustained burst
    main_idx = pre_bursts * spacing
    main_len = total_samples - main_idx
    t_main = np.linspace(0, duration, main_len, endpoint=False)
    main_noise = np.random.uniform(-1, 1, main_len) * np.exp(-t_main / 0.14)
    out[main_idx:] += main_noise
    
    # Filter through bandpass
    filtered = signal.lfilter(b, a, out)
    filtered = np.tanh(1.7 * filtered)
    filtered = filtered / (np.max(np.abs(filtered)) + 1e-6) * 0.95
    
    if is_stereo:
        # Slight stereo spread
        left = filtered
        delay_r = int(SR * 0.0018)
        right = np.zeros_like(left)
        right[delay_r:] = left[:-delay_r] * 0.95
        return np.column_stack([left, right])
    return filtered

# -------------------------------------------------------------
# 5. Hi-Hats & Cymbals (Classic 6-oscillator metallic cluster + noise)
# -------------------------------------------------------------
def make_hihat(duration=0.065, is_open=False, decay=0.035):
    if is_open:
        duration = 0.55
        decay = 0.32
    t = np.linspace(0, duration, int(SR * duration), endpoint=False)
    
    # 6 metallic square wave ratios
    metal_freqs = [245.0, 306.0, 384.0, 523.0, 659.0, 831.0]
    metal = np.zeros(len(t))
    for f in metal_freqs:
        metal += signal.square(2.0 * np.pi * f * t)
    metal /= len(metal_freqs)
    
    # Noise layer
    np.random.seed(77)
    noise = np.random.uniform(-1, 1, len(t))
    combined = 0.6 * metal + 0.4 * noise
    
    # High-pass filter at 7500 Hz for sizzling top
    b, a = butter_highpass(7500.0, SR)
    hp = signal.lfilter(b, a, combined)
    
    # Decay envelope
    amp_env = np.exp(-t / decay)
    out = hp * amp_env
    return out / (np.max(np.abs(out)) + 1e-6) * 0.95

def make_crash(duration=2.2):
    t = np.linspace(0, duration, int(SR * duration), endpoint=False)
    np.random.seed(999)
    noise = np.random.uniform(-1, 1, len(t))
    b, a = butter_highpass(5000.0, SR)
    hp = signal.lfilter(b, a, noise)
    
    metal_freqs = [310, 480, 620, 890, 1140, 1680]
    metal = np.zeros(len(t))
    for f in metal_freqs:
        metal += np.sin(2.0 * np.pi * f * t)
    metal /= len(metal_freqs)
    
    sig = hp * 0.7 + metal * 0.3
    amp_env = np.exp(-t / 0.85)
    crash_mono = sig * amp_env
    return apply_reverb_stereo(crash_mono, delay_ms=60, feedback=0.5, wet=0.35)

# -------------------------------------------------------------
# 6. Percussion
# -------------------------------------------------------------
def make_cowbell(duration=0.32, f1=587.0, f2=845.0):
    t = np.linspace(0, duration, int(SR * duration), endpoint=False)
    # TR-808 cowbell formula: two square waves bandpass filtered
    s1 = signal.square(2.0 * np.pi * f1 * t)
    s2 = signal.square(2.0 * np.pi * f2 * t)
    raw = (s1 + s2) * 0.5
    b, a = butter_bandpass(600.0, 1400.0, SR)
    filtered = signal.lfilter(b, a, raw)
    amp_env = np.exp(-t / 0.12)
    out = filtered * amp_env
    return out / (np.max(np.abs(out)) + 1e-6) * 0.95

def make_woodblock(duration=0.06, freq=1200.0):
    t = np.linspace(0, duration, int(SR * duration), endpoint=False)
    sine = np.sin(2.0 * np.pi * freq * t)
    amp_env = np.exp(-t / 0.012)
    out = sine * amp_env
    return out / (np.max(np.abs(out)) + 1e-6) * 0.95

def make_metal_pipe(duration=0.45, freq=1480.0):
    t = np.linspace(0, duration, int(SR * duration), endpoint=False)
    # Inharmonic metallic chime
    sig = np.sin(2 * np.pi * freq * t) * 0.5
    sig += np.sin(2 * np.pi * (freq * 1.414) * t) * 0.3
    sig += np.sin(2 * np.pi * (freq * 2.732) * t) * 0.2
    amp = np.exp(-t / 0.15)
    return sig * amp / (np.max(np.abs(sig * amp)) + 1e-6) * 0.95

def make_bongo(duration=0.25, freq=280.0):
    t = np.linspace(0, duration, int(SR * duration), endpoint=False)
    f_env = freq + 120.0 * np.exp(-t / 0.02)
    tone = np.sin(2.0 * np.pi * np.cumsum(f_env) / SR)
    amp = np.exp(-t / 0.08)
    return tone * amp / (np.max(np.abs(tone * amp)) + 1e-6) * 0.95

def make_shaker(duration=0.14):
    t = np.linspace(0, duration, int(SR * duration), endpoint=False)
    np.random.seed(55)
    noise = np.random.uniform(-1, 1, len(t))
    b, a = butter_bandpass(4500, 11000, SR)
    hp = signal.lfilter(b, a, noise)
    # Shaker stroke envelope: swell then cut
    swell_len = int(len(t) * 0.3)
    env = np.ones(len(t))
    env[:swell_len] = np.linspace(0, 1, swell_len)
    env[swell_len:] = np.exp(-np.linspace(0, 1, len(t) - swell_len) * 7)
    out = hp * env
    return out / (np.max(np.abs(out)) + 1e-6) * 0.95

# -------------------------------------------------------------
# 7. FX & Chants
# -------------------------------------------------------------
def make_chant_hey(duration=0.32):
    t = np.linspace(0, duration, int(SR * duration), endpoint=False)
    # Formants for 'ey' vowel: F1 ~ 500Hz, F2 ~ 1800Hz, F3 ~ 2500Hz
    # Glottal pulse
    pitch = 180.0 - 50.0 * (t / duration)
    phase = 2.0 * np.pi * np.cumsum(pitch) / SR
    pulse = signal.sawtooth(phase)
    
    b1, a1 = butter_bandpass(450, 650, SR)
    b2, a2 = butter_bandpass(1600, 2100, SR)
    b3, a3 = butter_bandpass(2300, 2800, SR)
    
    vocal = signal.lfilter(b1, a1, pulse) * 0.6 + signal.lfilter(b2, a2, pulse) * 0.4 + signal.lfilter(b3, a3, pulse) * 0.3
    amp = np.exp(-t / 0.15)
    out = vocal * amp
    return out / (np.max(np.abs(out)) + 1e-6) * 0.95

def make_riser(duration=6.857, bpm=140): # 4 bars @ 140 BPM
    t = np.linspace(0, duration, int(SR * duration), endpoint=False)
    # Pitch sweep 100Hz -> 10000Hz
    sweep_freq = 100.0 * (100.0 ** (t / duration))
    phase = 2.0 * np.pi * np.cumsum(sweep_freq) / SR
    sine = np.sin(phase)
    
    # White noise sweep
    np.random.seed(888)
    noise = np.random.uniform(-1, 1, len(t))
    
    # Rhythmic wobble (increasing rate)
    lfo = 0.5 + 0.5 * np.sin(2.0 * np.pi * (1.0 + 8.0 * (t / duration) ** 2) * t)
    
    raw = (sine * 0.4 + noise * 0.6) * lfo
    amp = (t / duration) ** 1.5
    out_mono = raw * amp
    return apply_reverb_stereo(out_mono, delay_ms=80, feedback=0.6, wet=0.4)

def make_sub_drop(duration=2.5):
    t = np.linspace(0, duration, int(SR * duration), endpoint=False)
    # 130Hz -> 25Hz exponential drop
    f_env = 25.0 + 105.0 * np.exp(-t / 0.6)
    phase = 2.0 * np.pi * np.cumsum(f_env) / SR
    sine = np.sin(phase)
    amp = np.exp(-t / 1.4)
    # Rumble noise
    noise = np.random.uniform(-1, 1, len(t)) * 0.15 * np.exp(-t / 0.4)
    out = (sine + noise) * amp
    return out / (np.max(np.abs(out)) + 1e-6) * 0.95

def make_laser(duration=0.18):
    t = np.linspace(0, duration, int(SR * duration), endpoint=False)
    f_env = 60.0 + 3200.0 * np.exp(-t / 0.035)
    phase = 2.0 * np.pi * np.cumsum(f_env) / SR
    out = np.sin(phase) * np.exp(-t / 0.08)
    return out / (np.max(np.abs(out)) + 1e-6) * 0.95

print("All generator functions defined.")
