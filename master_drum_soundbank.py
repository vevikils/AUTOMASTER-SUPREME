import os
import math
import numpy as np
from scipy import signal
from dsp_utils import butter_lowpass, butter_highpass, butter_bandpass

SR = 44100

class MasterSoundbank:
    def __init__(self):
        print("[*] Generating authentic style-specific hit drum samples...")
        self.samples = {}
        self._build_argentine_trap()
        self._build_puerto_rico_trap()
        self._build_classic_reggaeton()
        self._build_modern_tainy_reggaeton()
        print(f"[+] MasterSoundbank ready: {len(self.samples)} distinct hit instruments cached.")

    # 1. TRAP ARGENTINO (Duki, YSY A, Bizarrap style)
    def _build_argentine_trap(self):
        # 808 Spinz Distorted (Aggressive Argentine Trap)
        t_808 = np.linspace(0, 2.2, int(SR * 2.2), endpoint=False)
        f_env = 32.7 + 140.0 * np.exp(-t_808 / 0.028)
        phase = 2.0 * np.pi * np.cumsum(f_env) / SR
        raw = np.sin(phase) + 0.45 * np.sin(2 * phase) + 0.25 * np.sin(3 * phase)
        # Heavy saturation
        dist = np.tanh(3.8 * raw)
        amp = np.exp(-t_808 / 1.7)
        amp[:int(SR*0.003)] *= np.linspace(0, 1, int(SR*0.003))
        amp[-int(SR*0.05):] *= np.linspace(1, 0, int(SR*0.05))
        s_808 = dist * amp
        b, a = butter_lowpass(1200.0, SR)
        s_808 = signal.lfilter(b, a, s_808)
        self.samples['arg_808'] = s_808 / np.max(np.abs(s_808)) * 0.95

        # Argentine Knock Kick (Super punchy, tight 110ms decay, no mud)
        t_k = np.linspace(0, 0.35, int(SR * 0.35), endpoint=False)
        f_k = 62.0 + 260.0 * np.exp(-t_k / 0.022)
        click = np.sin(2 * np.pi * 2800.0 * t_k[:int(SR*0.004)]) * np.exp(-t_k[:int(SR*0.004)] / 0.001)
        sub = np.sin(2.0 * np.pi * np.cumsum(f_k) / SR) * np.exp(-t_k / 0.08)
        kick = sub
        kick[:len(click)] += click * 0.55
        kick = np.tanh(2.2 * kick)
        self.samples['arg_kick'] = kick / np.max(np.abs(kick)) * 0.96

        # Argentine Trap Clap (Short, crisp, slap transient)
        t_c = np.linspace(0, 0.32, int(SR * 0.32), endpoint=False)
        np.random.seed(111)
        n = np.random.uniform(-1, 1, len(t_c))
        b, a = butter_bandpass(1100.0, 4800.0, SR)
        clap = signal.lfilter(b, a, n) * np.exp(-t_c / 0.11)
        # 3 pre-bursts
        for pb in [0, int(SR*0.009), int(SR*0.018)]:
            clap[pb:pb+int(SR*0.003)] += np.random.uniform(-0.6, 0.6, int(SR*0.003))
        clap = np.tanh(1.9 * clap)
        self.samples['arg_clap'] = clap / np.max(np.abs(clap)) * 0.95

        # Argentine High Sizzle Hat (Ultra fast 40ms decay for 32nd note rolls)
        t_h = np.linspace(0, 0.05, int(SR * 0.05), endpoint=False)
        n_h = np.random.uniform(-1, 1, len(t_h))
        b, a = butter_highpass(8500.0, SR)
        hat = signal.lfilter(b, a, n_h) * np.exp(-t_h / 0.018)
        self.samples['arg_hat'] = hat / np.max(np.abs(hat)) * 0.92

        # Open Hat Sizzle
        t_oh = np.linspace(0, 0.45, int(SR * 0.45), endpoint=False)
        n_oh = np.random.uniform(-1, 1, len(t_oh))
        hat_o = signal.lfilter(b, a, n_oh) * np.exp(-t_oh / 0.28)
        self.samples['arg_hat_open'] = hat_o / np.max(np.abs(hat_o)) * 0.90

    # 2. LATIN TRAP PUERTO RICO (Bad Bunny, Anuel AA, Eladio Carrión style)
    def _build_puerto_rico_trap(self):
        # PR Deep Sub 808 (Warm, rounded, massive 30-50Hz pressure)
        t_808 = np.linspace(0, 2.5, int(SR * 2.5), endpoint=False)
        f_env = 32.7 + 95.0 * np.exp(-t_808 / 0.04)
        phase = 2.0 * np.pi * np.cumsum(f_env) / SR
        raw = np.sin(phase) + 0.25 * np.sin(2 * phase)
        dist = np.tanh(1.8 * raw)
        amp = np.exp(-t_808 / 2.0)
        s_808 = dist * amp
        b, a = butter_lowpass(600.0, SR)
        s_808 = signal.lfilter(b, a, s_808)
        self.samples['pr_trap_808'] = s_808 / np.max(np.abs(s_808)) * 0.95

        # PR Heavy Trap Kick (Round, deep, acoustic layer)
        t_k = np.linspace(0, 0.45, int(SR * 0.45), endpoint=False)
        f_k = 48.0 + 190.0 * np.exp(-t_k / 0.035)
        body = np.sin(2.0 * np.pi * np.cumsum(f_k) / SR) * np.exp(-t_k / 0.16)
        click = np.sin(2 * np.pi * 1600.0 * t_k[:int(SR*0.005)]) * np.exp(-t_k[:int(SR*0.005)] / 0.0015)
        kick = body
        kick[:len(click)] += click * 0.35
        kick = np.tanh(1.5 * kick)
        self.samples['pr_trap_kick'] = kick / np.max(np.abs(kick)) * 0.95

        # PR Acoustic Rimshot (Chambea / Soy Peor style)
        t_r = np.linspace(0, 0.16, int(SR * 0.16), endpoint=False)
        wood = np.sin(2 * np.pi * 840.0 * t_r) * np.exp(-t_r / 0.03) + np.sin(2 * np.pi * 1620.0 * t_r) * 0.4 * np.exp(-t_r / 0.02)
        n_r = np.random.uniform(-1, 1, len(t_r))
        b, a = butter_bandpass(2000.0, 7000.0, SR)
        crack = signal.lfilter(b, a, n_r) * np.exp(-t_r / 0.015)
        rim = wood * 0.6 + crack * 0.45
        self.samples['pr_trap_rim'] = rim / np.max(np.abs(rim)) * 0.95

        # PR Down-tuned Trap Snare (Punchy body at 170Hz)
        t_s = np.linspace(0, 0.38, int(SR * 0.38), endpoint=False)
        f_s = 170.0 + 130.0 * np.exp(-t_s / 0.02)
        s_body = np.sin(2.0 * np.pi * np.cumsum(f_s) / SR) * np.exp(-t_s / 0.10)
        n_s = np.random.uniform(-1, 1, len(t_s))
        b, a = butter_bandpass(750.0, 5000.0, SR)
        s_wire = signal.lfilter(b, a, n_s) * np.exp(-t_s / 0.24)
        snare = s_body * 0.4 + s_wire * 0.6
        self.samples['pr_trap_snare'] = snare / np.max(np.abs(snare)) * 0.95

    # 3. REGGAETÓN CLÁSICO PUERTO RICO (Daddy Yankee, Don Omar, Luny Tunes style)
    def _build_classic_reggaeton(self):
        # The iconic Gasolina Dembow Kick (Fat 56Hz body + round punch)
        t_k = np.linspace(0, 0.38, int(SR * 0.38), endpoint=False)
        f_k = 56.0 + 240.0 * np.exp(-t_k / 0.025)
        body = np.sin(2.0 * np.pi * np.cumsum(f_k) / SR) * np.exp(-t_k / 0.12)
        click = np.sin(2 * np.pi * 2100.0 * t_k[:int(SR*0.005)]) * np.exp(-t_k[:int(SR*0.005)] / 0.0012)
        kick = body
        kick[:len(click)] += click * 0.4
        kick = np.tanh(1.7 * kick)
        self.samples['classic_kick'] = kick / np.max(np.abs(kick)) * 0.96

        # The iconic Classic Dembow Snare (Gasolina / Mayor Que Yo hollow snap)
        t_s = np.linspace(0, 0.28, int(SR * 0.28), endpoint=False)
        f_s = 225.0 + 130.0 * np.exp(-t_s / 0.015)
        body = np.sin(2.0 * np.pi * np.cumsum(f_s) / SR) * np.exp(-t_s / 0.08)
        ring = np.sin(2.0 * np.pi * 480.0 * t_s) * 0.35 * np.exp(-t_s / 0.06)
        n = np.random.uniform(-1, 1, len(t_s))
        b, a = butter_bandpass(1400.0, 5500.0, SR)
        wire = signal.lfilter(b, a, n) * np.exp(-t_s / 0.16)
        crack_len = int(SR * 0.002)
        wire[:crack_len] += np.linspace(1, 0, crack_len) * 0.8
        snare = body * 0.5 + ring * 0.25 + wire * 0.65
        snare = np.tanh(1.8 * snare)
        self.samples['classic_snare'] = snare / np.max(np.abs(snare)) * 0.95

        # Metal Güira Scrape (Down & Up)
        t_g = np.linspace(0, 0.09, int(SR * 0.09), endpoint=False)
        np.random.seed(88)
        n_g = np.random.uniform(-1, 1, len(t_g))
        res = np.sin(2 * np.pi * 5800.0 * t_g) * 0.25 + np.sin(2 * np.pi * 8200.0 * t_g) * 0.2
        raw = n_g * 0.7 + res * 0.3
        b, a = butter_bandpass(3500.0, 11000.0, SR)
        filt = signal.lfilter(b, a, raw)
        env = np.sin(np.pi * (t_g / 0.09)) ** 1.5
        guira = filt * env
        self.samples['classic_guira'] = guira / np.max(np.abs(guira)) * 0.88

        # Timbal Perc (High ring for fills)
        t_tim = np.linspace(0, 0.22, int(SR * 0.22), endpoint=False)
        tim = np.sin(2 * np.pi * 780.0 * t_tim) * np.exp(-t_tim / 0.06) + np.sin(2 * np.pi * 1250.0 * t_tim) * 0.35 * np.exp(-t_tim / 0.04)
        self.samples['classic_timbal'] = tim / np.max(np.abs(tim)) * 0.90

    # 4. REGGAETÓN MODERNO & TAINY STYLE (Tainy / Data / Un Verano Sin Ti / Feid)
    def _build_modern_tainy_reggaeton(self):
        # Tainy Sub Kick (48Hz deep fundamental, tight, soft-clipped)
        t_k = np.linspace(0, 0.40, int(SR * 0.40), endpoint=False)
        f_k = 48.0 + 200.0 * np.exp(-t_k / 0.03)
        body = np.sin(2.0 * np.pi * np.cumsum(f_k) / SR) * np.exp(-t_k / 0.14)
        click = np.sin(2 * np.pi * 1900.0 * t_k[:int(SR*0.004)]) * np.exp(-t_k[:int(SR*0.004)] / 0.001)
        kick = body
        kick[:len(click)] += click * 0.3
        kick = np.tanh(1.4 * kick)
        self.samples['tainy_kick'] = kick / np.max(np.abs(kick)) * 0.95

        # Tainy Clap/Snare Hybrid (Acoustic texture + modern snap)
        t_s = np.linspace(0, 0.32, int(SR * 0.32), endpoint=False)
        f_s = 205.0 + 100.0 * np.exp(-t_s / 0.018)
        body = np.sin(2.0 * np.pi * np.cumsum(f_s) / SR) * np.exp(-t_s / 0.07)
        n = np.random.uniform(-1, 1, len(t_s))
        b, a = butter_bandpass(1000.0, 4200.0, SR)
        wire = signal.lfilter(b, a, n) * np.exp(-t_s / 0.18)
        # Pre-clap micro transient
        wire[:int(SR*0.003)] += np.random.uniform(-0.5, 0.5, int(SR*0.003))
        hybrid = body * 0.45 + wire * 0.65
        hybrid = np.tanh(1.5 * hybrid)
        self.samples['tainy_snare'] = hybrid / np.max(np.abs(hybrid)) * 0.95

        # Organic Stereo Shaker
        t_sh = np.linspace(0, 0.12, int(SR * 0.12), endpoint=False)
        np.random.seed(55)
        n_sh = np.random.uniform(-1, 1, len(t_sh))
        b, a = butter_bandpass(4000.0, 10500.0, SR)
        filt_sh = signal.lfilter(b, a, n_sh)
        env_sh = np.sin(np.pi * (t_sh / 0.12)) ** 1.3
        self.samples['tainy_shaker'] = (filt_sh * env_sh) / np.max(np.abs(filt_sh * env_sh)) * 0.85

        # Tainy Wood/Rim Ghost Perc
        t_w = np.linspace(0, 0.10, int(SR * 0.10), endpoint=False)
        wood = np.sin(2 * np.pi * 920.0 * t_w) * np.exp(-t_w / 0.02)
        self.samples['tainy_perc'] = wood / np.max(np.abs(wood)) * 0.88

print("MasterSoundbank module ready.")
