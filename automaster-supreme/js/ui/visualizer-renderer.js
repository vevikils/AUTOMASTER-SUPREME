/**
 * AUTOMASTER SUPREME - 60 FPS Canvas Visualizer Renderer
 * Features:
 * - Real-Time Dual FFT Spectrum (Pre-Master vs Post-Master with neon glows)
 * - Draggable Parametric EQ curve overlay
 * - Stereo Vector Goniometer (Lissajous Oscilloscope)
 * - Phase Correlation Meter (-1 to +1)
 * - True Peak (L/R) Segmented LED Bars & LUFS EBU R128 display
 */

export class VisualizerRenderer {
  constructor(dspEngine, analyzer) {
    this.dsp = dspEngine;
    this.analyzer = analyzer;
    this.isRunning = false;

    // DOM Elements
    this.spectrumCanvas = document.getElementById('spectrum-canvas');
    this.spectrumCtx = this.spectrumCanvas?.getContext('2d');

    this.goniometerCanvas = document.getElementById('goniometer-canvas');
    this.goniometerCtx = this.goniometerCanvas?.getContext('2d');

    // Meters DOM
    this.meterPeakL = document.getElementById('meter-peak-l');
    this.meterPeakR = document.getElementById('meter-peak-r');
    this.meterHoldL = document.getElementById('meter-hold-l');
    this.meterHoldR = document.getElementById('meter-hold-r');

    this.valPeakL = document.getElementById('val-peak-l');
    this.valPeakR = document.getElementById('val-peak-r');
    this.valLufsMomentary = document.getElementById('val-lufs-momentary');
    this.valLufsShort = document.getElementById('val-lufs-short');
    this.valCrest = document.getElementById('val-crest');
    this.phaseBar = document.getElementById('phase-correlation-bar');

    this.setupResizeListeners();
  }

  setupResizeListeners() {
    const resize = () => {
      if (this.spectrumCanvas) {
        const rect = this.spectrumCanvas.getBoundingClientRect();
        this.spectrumCanvas.width = rect.width * window.devicePixelRatio;
        this.spectrumCanvas.height = rect.height * window.devicePixelRatio;
      }
      if (this.goniometerCanvas) {
        const rect = this.goniometerCanvas.getBoundingClientRect();
        this.goniometerCanvas.width = rect.width * window.devicePixelRatio;
        this.goniometerCanvas.height = rect.height * window.devicePixelRatio;
      }
    };
    window.addEventListener('resize', resize);
    setTimeout(resize, 100);
  }

  start() {
    if (this.isRunning) return;
    this.isRunning = true;
    const loop = () => {
      if (!this.isRunning) return;
      this.analyzer.update();
      this.renderSpectrum();
      this.renderGoniometer();
      this.renderMeters();
      requestAnimationFrame(loop);
    };
    requestAnimationFrame(loop);
  }

  stop() {
    this.isRunning = false;
  }

  renderSpectrum() {
    const ctx = this.spectrumCtx;
    const canvas = this.spectrumCanvas;
    if (!ctx || !canvas) return;

    const width = canvas.width;
    const height = canvas.height;

    // Clear background
    ctx.fillStyle = '#0a0d12';
    ctx.fillRect(0, 0, width, height);

    // Draw logarithmic frequency & dB grid
    this.drawSpectrumGrid(ctx, width, height);

    const preData = this.analyzer.preFreqData;
    const postData = this.analyzer.postFreqData;
    const binCount = preData.length;

    // Helper: Map frequency (20Hz to 20kHz) logarithmically to X coordinate
    const freqToX = (freq) => {
      const minLog = Math.log10(20);
      const maxLog = Math.log10(20000);
      const fLog = Math.log10(Math.max(20, Math.min(20000, freq)));
      return ((fLog - minLog) / (maxLog - minLog)) * width;
    };

    // Helper: Map dB (-80dB to +6dB) to Y coordinate
    const dbToY = (db) => {
      const minDb = -80;
      const maxDb = 6;
      const clamped = Math.max(minDb, Math.min(maxDb, db));
      return height - ((clamped - minDb) / (maxDb - minDb)) * height;
    };

    // 1. Draw Pre-Master Spectrum (Gold / Amber subtle curve)
    if (this.dsp.isPlaying) {
      ctx.beginPath();
      ctx.strokeStyle = 'rgba(255, 185, 45, 0.4)';
      ctx.lineWidth = 1.5 * window.devicePixelRatio;
      let started = false;

      for (let i = 1; i < binCount; i += 2) {
        const freq = (i / binCount) * (48000 / 2);
        if (freq < 20 || freq > 20000) continue;
        const x = freqToX(freq);
        const y = dbToY(preData[i]);

        if (!started) {
          ctx.moveTo(x, y);
          started = true;
        } else {
          ctx.lineTo(x, y);
        }
      }
      ctx.stroke();
    }

    // 2. Draw Post-Master Spectrum (Neon Cyan Glow with gradient fill)
    if (this.dsp.isPlaying) {
      ctx.save();
      ctx.beginPath();
      let started = false;
      const firstX = freqToX(20);
      ctx.moveTo(firstX, height);

      for (let i = 1; i < binCount; i += 2) {
        const freq = (i / binCount) * (48000 / 2);
        if (freq < 20 || freq > 20000) continue;
        const x = freqToX(freq);
        const y = dbToY(postData[i]);

        if (!started) {
          ctx.lineTo(x, y);
          started = true;
        } else {
          ctx.lineTo(x, y);
        }
      }

      ctx.lineTo(width, height);
      ctx.closePath();

      // Neon Gradient Fill
      const grad = ctx.createLinearGradient(0, 0, 0, height);
      grad.addColorStop(0, 'rgba(0, 240, 255, 0.35)');
      grad.addColorStop(0.5, 'rgba(0, 200, 255, 0.12)');
      grad.addColorStop(1, 'rgba(0, 100, 255, 0.0)');
      ctx.fillStyle = grad;
      ctx.fill();

      // Neon Stroke
      ctx.strokeStyle = '#00f0ff';
      ctx.lineWidth = 2.2 * window.devicePixelRatio;
      ctx.shadowColor = '#00f0ff';
      ctx.shadowBlur = 8 * window.devicePixelRatio;
      ctx.stroke();
      ctx.restore();
    }

    // 3. Draw EQ Curve Overlay
    this.drawEQCurve(ctx, width, height, freqToX, dbToY);
  }

  drawSpectrumGrid(ctx, width, height) {
    ctx.strokeStyle = 'rgba(255, 255, 255, 0.05)';
    ctx.lineWidth = 1;
    ctx.fillStyle = 'rgba(255, 255, 255, 0.25)';
    ctx.font = `${9 * window.devicePixelRatio}px monospace`;

    // dB lines: 0, -12, -24, -36, -48, -60
    const dbs = [0, -12, -24, -36, -48, -60];
    dbs.forEach(db => {
      const y = height - ((db - (-80)) / (86)) * height;
      ctx.beginPath();
      ctx.moveTo(0, y);
      ctx.lineTo(width, y);
      ctx.stroke();
      ctx.fillText(`${db}dB`, 6, y - 4);
    });

    // Freq markers: 50, 100, 250, 500, 1k, 2.5k, 5k, 10k, 20k
    const freqs = [
      { f: 50, label: '50' },
      { f: 100, label: '100' },
      { f: 250, label: '250' },
      { f: 500, label: '500' },
      { f: 1000, label: '1k' },
      { f: 2500, label: '2.5k' },
      { f: 5000, label: '5k' },
      { f: 10000, label: '10k' },
      { f: 20000, label: '20k' }
    ];

    const minLog = Math.log10(20);
    const maxLog = Math.log10(20000);

    freqs.forEach(item => {
      const x = ((Math.log10(item.f) - minLog) / (maxLog - minLog)) * width;
      ctx.beginPath();
      ctx.moveTo(x, 0);
      ctx.lineTo(x, height);
      ctx.stroke();
      ctx.fillText(item.label, x + 3, height - 6);
    });
  }

  drawEQCurve(ctx, width, height, freqToX, dbToY) {
    if (!this.dsp.params) return;
    const p = this.dsp.params;
    if (p.eqBypass) return;

    ctx.save();
    ctx.strokeStyle = 'rgba(255, 215, 0, 0.85)';
    ctx.lineWidth = 2 * window.devicePixelRatio;
    ctx.setLineDash([4, 4]);

    ctx.beginPath();
    let started = false;

    // Sample 120 points logarithmically across 20Hz - 20kHz
    for (let i = 0; i <= 120; i++) {
      const f = 20 * Math.pow(1000, i / 120);
      const x = freqToX(f);

      // Approximate combined EQ gain in dB at frequency f
      let gainDb = 0;

      // Low shelf
      if (f < p.eqLowShelfFreq * 2) {
        const factor = 1 / (1 + Math.pow(f / p.eqLowShelfFreq, 2));
        gainDb += p.eqLowShelfGain * factor;
      }

      // Bell 2 (Low Mid)
      const q2 = p.eqLowMidQ || 1;
      const dist2 = Math.abs(Math.log2(f / p.eqLowMidFreq));
      gainDb += p.eqLowMidGain * Math.exp(-dist2 * q2 * 1.5);

      // Bell 3 (Mid)
      const q3 = p.eqMidQ || 1;
      const dist3 = Math.abs(Math.log2(f / p.eqMidFreq));
      gainDb += p.eqMidGain * Math.exp(-dist3 * q3 * 1.5);

      // Bell 4 (High Mid)
      const q4 = p.eqHighMidQ || 1;
      const dist4 = Math.abs(Math.log2(f / p.eqHighMidFreq));
      gainDb += p.eqHighMidGain * Math.exp(-dist4 * q4 * 1.5);

      // High Shelf
      if (f > p.eqHighShelfFreq * 0.5) {
        const factor = 1 / (1 + Math.pow(p.eqHighShelfFreq / f, 2));
        gainDb += p.eqHighShelfGain * factor;
      }

      // Center around -18dB line for clean visual overlay
      const y = dbToY(gainDb - 18);

      if (!started) {
        ctx.moveTo(x, y);
        started = true;
      } else {
        ctx.lineTo(x, y);
      }
    }

    ctx.stroke();
    ctx.restore();
  }

  renderGoniometer() {
    const ctx = this.goniometerCtx;
    const canvas = this.goniometerCanvas;
    if (!ctx || !canvas) return;

    const width = canvas.width;
    const height = canvas.height;
    const cx = width / 2;
    const cy = height / 2;
    const radius = Math.min(cx, cy) * 0.88;

    // Fade previous frame for phosphorescent persistence effect
    ctx.fillStyle = 'rgba(8, 12, 18, 0.35)';
    ctx.fillRect(0, 0, width, height);

    // Draw reticle and circular guide
    ctx.strokeStyle = 'rgba(0, 240, 255, 0.15)';
    ctx.lineWidth = 1;
    ctx.beginPath();
    ctx.arc(cx, cy, radius, 0, Math.PI * 2);
    ctx.moveTo(cx, cy - radius);
    ctx.lineTo(cx, cy + radius);
    ctx.moveTo(cx - radius, cy);
    ctx.lineTo(cx + radius, cy);
    ctx.stroke();

    // Labels +S, -S, M
    ctx.fillStyle = 'rgba(0, 240, 255, 0.35)';
    ctx.font = `${9 * window.devicePixelRatio}px monospace`;
    ctx.fillText('L', cx - radius + 4, cy - 4);
    ctx.fillText('R', cx + radius - 12, cy - 4);
    ctx.fillText('MID', cx - 10, cy - radius + 12);

    if (!this.dsp.isPlaying) return;

    // Draw Lissajous Stereo Field
    const timeData = this.analyzer.timeDataL;
    const len = timeData.length;
    const step = 4;

    ctx.save();
    ctx.strokeStyle = 'rgba(0, 255, 170, 0.65)';
    ctx.shadowColor = '#00ffaa';
    ctx.shadowBlur = 4 * window.devicePixelRatio;
    ctx.lineWidth = 1.2 * window.devicePixelRatio;
    ctx.beginPath();

    let started = false;
    for (let i = 0; i < len - 1; i += step) {
      const l = timeData[i];
      const r = timeData[i + 1] || l;

      // Coordinate transformation:
      // X = (L - R) / sqrt(2)  (Side)
      // Y = -(L + R) / sqrt(2) (Mid)
      const x = cx + (l - r) * 0.707 * radius * 1.6;
      const y = cy - (l + r) * 0.707 * radius * 1.6;

      if (!started) {
        ctx.moveTo(x, y);
        started = true;
      } else {
        ctx.lineTo(x, y);
      }
    }
    ctx.stroke();
    ctx.restore();
  }

  renderMeters() {
    const a = this.analyzer;

    // Peak L/R percent (0dB = 100%, -48dB = 0%)
    const toPercent = (db) => Math.max(0, Math.min(100, ((db + 48) / 48) * 100));

    if (this.meterPeakL) this.meterPeakL.style.height = `${toPercent(a.peakL)}%`;
    if (this.meterPeakR) this.meterPeakR.style.height = `${toPercent(a.peakR)}%`;
    if (this.meterHoldL) this.meterHoldL.style.top = `${100 - toPercent(a.peakHoldL)}%`;
    if (this.meterHoldR) this.meterHoldR.style.top = `${100 - toPercent(a.peakHoldR)}%`;

    if (this.valPeakL) this.valPeakL.textContent = `${a.peakL > -90 ? a.peakL.toFixed(1) : '-∞'} dB`;
    if (this.valPeakR) this.valPeakR.textContent = `${a.peakR > -90 ? a.peakR.toFixed(1) : '-∞'} dB`;

    if (this.valLufsMomentary) this.valLufsMomentary.textContent = `${a.momentaryLUFS.toFixed(1)}`;
    if (this.valLufsShort) this.valLufsShort.textContent = `${a.shortTermLUFS.toFixed(1)}`;
    if (this.valCrest) this.valCrest.textContent = `${a.crestFactor.toFixed(1)} dB`;

    if (this.phaseBar) {
      // Correlation range: -1 to +1 -> 0% to 100%
      const norm = Math.max(0, Math.min(1, (a.phaseCorrelation + 1) / 2));
      this.phaseBar.style.left = `${norm * 100}%`;
    }
  }
}
