import os
import math
import numpy as np
from scipy import signal
from wav_writer import write_wav
from midi_writer import MidiWriter

SR = 44100
KIT_DIR = r"c:\Users\alfaswz\.gemini\antigravity-ide\scratch\agentes-personalizados\SUPREME-PRODUCER-DRUM-KIT"

# Helper DSP functions
def butter_bandpass(lowcut, highcut, fs, order=2):
    nyq = 0.5 * fs
    low = max(20.0, lowcut) / nyq
    high = min(nyq - 50.0, highcut) / nyq
    b, a = signal.butter(order, [low, high], btype='band')
    return b, a

def butter_lowpass(cutoff, fs, order=2):
    nyq = 0.5 * fs
    normal_cutoff = min(cutoff, nyq - 50.0) / nyq
    b, a = signal.butter(order, normal_cutoff, btype='low')
    return b, a

def butter_highpass(cutoff, fs, order=2):
    nyq = 0.5 * fs
    normal_cutoff = min(cutoff, nyq - 50.0) / nyq
    b, a = signal.butter(order, normal_cutoff, btype='high')
    return b, a

def apply_reverb_stereo(audio_mono, delay_ms=45, feedback=0.45, wet=0.25):
    """Simple algorithmic stereo reverb tail"""
    d_samples_l = int(SR * (delay_ms / 1000.0))
    d_samples_r = int(SR * ((delay_ms * 1.35) / 1000.0))
    
    out_l = np.zeros(len(audio_mono) + int(SR * 1.0))
    out_r = np.zeros(len(audio_mono) + int(SR * 1.0))
    
    out_l[:len(audio_mono)] = audio_mono
    out_r[:len(audio_mono)] = audio_mono
    
    for i in range(len(audio_mono)):
        if i + d_samples_l < len(out_l):
            out_l[i + d_samples_l] += audio_mono[i] * feedback
        if i + d_samples_r < len(out_r):
            out_r[i + d_samples_r] += audio_mono[i] * feedback * 0.9
            
    # Mix
    stereo = np.column_stack([
        out_l[:len(audio_mono)] * (1 - wet) + out_l[:len(audio_mono)] * wet,
        out_r[:len(audio_mono)] * (1 - wet) + out_r[:len(audio_mono)] * wet
    ])
    return stereo

print("DSP helpers initialized.")
