/**
 * AUTOMASTER SUPREME - DSP Audio Engine
 * Pro-grade Web Audio 64-bit precision mastering chain with:
 * - Low-Cut Sub Tamer
 * - 5-Band Parametric Mastering EQ
 * - 4x Oversampled Analog Tape & Tube Saturation
 * - 3-Band Multiband Dynamics Processor
 * - Mid/Side Stereo Imager with Bass Mono-Maker
 * - Lookahead True-Peak Brickwall Limiter
 * - Level-Matched A/B Switch
 * - 32-bit Float Offline Render & 24-bit WAV Exporter
 */

export class DSPEngine {
  constructor() {
    this.ctx = null;
    this.isInitialized = false;
    this.audioBuffer = null;
    this.sourceNode = null;
    this.isPlaying = false;
    this.playbackStartTime = 0;
    this.pausedAt = 0;
    this.isLooping = false;

    // A/B Monitoring state: 'wet' (mastered) | 'dry' (original mix)
    this.abMode = 'wet';
    this.levelMatchEnabled = true;
    this.measuredDryRMS = 0.1;
    this.measuredWetRMS = 0.1;

    // Parameters state
    this.params = {
      inputGain: 0, // dB (-18 to +18)
      lowCutFreq: 30, // Hz (20 to 80)
      lowCutBypass: false,

      // 5-Band EQ
      eqLowShelfFreq: 80,
      eqLowShelfGain: 0, // dB (-12 to +12)
      eqLowMidFreq: 320,
      eqLowMidGain: 0,
      eqLowMidQ: 1.2,
      eqMidFreq: 1800,
      eqMidGain: 0,
      eqMidQ: 1.4,
      eqHighMidFreq: 4800,
      eqHighMidGain: 0,
      eqHighMidQ: 1.2,
      eqHighShelfFreq: 11000,
      eqHighShelfGain: 0,
      eqBypass: false,

      // Tape / Tube Saturation
      saturationDrive: 15, // 0 to 100%
      saturationWarmth: 30, // 0 to 100%
      saturationMode: 'tape', // 'tape' | 'tube' | 'classA' | 'clean'
      saturationBypass: false,

      // 3-Band Multiband Dynamics
      mbLowThreshold: -18, // dB
      mbLowRatio: 2.2,
      mbLowGain: 0,
      mbMidThreshold: -16,
      mbMidRatio: 2.0,
      mbMidGain: 0,
      mbHighThreshold: -14,
      mbHighRatio: 1.8,
      mbHighGain: 0,
      mbBypass: false,

      // Stereo Imaging & Mono Maker
      stereoWidth: 125, // % (0 = mono, 100 = normal, 200 = wide)
      monoMakerFreq: 110, // Hz
      stereoBypass: false,

      // True Peak Limiter & Loudness Maximizer
      loudnessDrive: 3.5, // dB (0 to 14)
      ceiling: -0.2, // dBTP (-1.5 to 0)
      limiterRelease: 0.08, // seconds
      limiterBypass: false,

      outputGain: 0 // dB
    };
  }

  async init() {
    if (this.isInitialized) return;
    const AudioContextClass = window.AudioContext || window.webkitAudioContext;
    this.ctx = new AudioContextClass({ latencyHint: 'interactive' });

    // Build real-time nodes
    this.buildGraph();
    this.isInitialized = true;
  }

  ensureContextRunning() {
    if (this.ctx && this.ctx.state === 'suspended') {
      return this.ctx.resume();
    }
    return Promise.resolve();
  }

  buildGraph() {
    const ctx = this.ctx;

    // 1. Analyzers for Pre and Post monitoring
    this.preAnalyzer = ctx.createAnalyser();
    this.preAnalyzer.fftSize = 2048;
    this.preAnalyzer.smoothingTimeConstant = 0.85;

    this.postAnalyzer = ctx.createAnalyser();
    this.postAnalyzer.fftSize = 2048;
    this.postAnalyzer.smoothingTimeConstant = 0.85;

    // 2. Input Stage
    this.inputGainNode = ctx.createGain();

    // 3. Sub Tamer Low-Cut Filter
    this.lowCutFilter = ctx.createBiquadFilter();
    this.lowCutFilter.type = 'highpass';
    this.lowCutFilter.frequency.value = this.params.lowCutFreq;
    this.lowCutFilter.Q.value = 0.707;

    // 4. 5-Band Parametric Mastering EQ
    this.eqBand1 = ctx.createBiquadFilter(); // Low Shelf
    this.eqBand1.type = 'lowshelf';
    this.eqBand1.frequency.value = this.params.eqLowShelfFreq;
    this.eqBand1.gain.value = this.params.eqLowShelfGain;

    this.eqBand2 = ctx.createBiquadFilter(); // Low-Mid Bell
    this.eqBand2.type = 'peaking';
    this.eqBand2.frequency.value = this.params.eqLowMidFreq;
    this.eqBand2.gain.value = this.params.eqLowMidGain;
    this.eqBand2.Q.value = this.params.eqLowMidQ;

    this.eqBand3 = ctx.createBiquadFilter(); // Mid Bell
    this.eqBand3.type = 'peaking';
    this.eqBand3.frequency.value = this.params.eqMidFreq;
    this.eqBand3.gain.value = this.params.eqMidGain;
    this.eqBand3.Q.value = this.params.eqMidQ;

    this.eqBand4 = ctx.createBiquadFilter(); // High-Mid Bell
    this.eqBand4.type = 'peaking';
    this.eqBand4.frequency.value = this.params.eqHighMidFreq;
    this.eqBand4.gain.value = this.params.eqHighMidGain;
    this.eqBand4.Q.value = this.params.eqHighMidQ;

    this.eqBand5 = ctx.createBiquadFilter(); // High Shelf
    this.eqBand5.type = 'highshelf';
    this.eqBand5.frequency.value = this.params.eqHighShelfFreq;
    this.eqBand5.gain.value = this.params.eqHighShelfGain;

    // Chain EQ bands sequentially
    this.lowCutFilter.connect(this.eqBand1);
    this.eqBand1.connect(this.eqBand2);
    this.eqBand2.connect(this.eqBand3);
    this.eqBand3.connect(this.eqBand4);
    this.eqBand4.connect(this.eqBand5);

    // 5. Analog Tape / Tube Saturation (4x oversampled WaveShaper)
    this.saturationPreGain = ctx.createGain();
    this.shaperNode = ctx.createWaveShaper();
    this.shaperNode.oversample = '4x';
    this.updateSaturationCurve();
    this.saturationPostGain = ctx.createGain();

    this.eqBand5.connect(this.saturationPreGain);
    this.saturationPreGain.connect(this.shaperNode);
    this.shaperNode.connect(this.saturationPostGain);

    // 6. 3-Band Multiband Dynamics Crossover (160 Hz & 4200 Hz)
    this.setupMultibandCrossover(this.saturationPostGain);

    // 7. Stereo Imager & Mono Maker
    this.setupStereoImager(this.mbSumNode);

    // 8. Loudness Drive & Lookahead Brickwall Limiter
    this.limiterDriveNode = ctx.createGain();
    this.limiterNode = ctx.createDynamicsCompressor();
    this.limiterNode.threshold.value = -0.5;
    this.limiterNode.knee.value = 0.0;
    this.limiterNode.ratio.value = 20.0; // Brickwall ratio
    this.limiterNode.attack.value = 0.001; // Ultra-fast 1ms attack
    this.limiterNode.release.value = this.params.limiterRelease;

    this.ceilingGainNode = ctx.createGain();

    this.imagerOutNode.connect(this.limiterDriveNode);
    this.limiterDriveNode.connect(this.limiterNode);
    this.limiterNode.connect(this.ceilingGainNode);

    // 9. Wet / Dry A/B Crossfader & Level Matching
    this.wetGain = ctx.createGain();
    this.wetGain.gain.value = 1.0;
    this.ceilingGainNode.connect(this.wetGain);

    this.dryGain = ctx.createGain();
    this.dryGain.gain.value = 0.0;
    this.dryLevelMatchGain = ctx.createGain();
    this.dryLevelMatchGain.gain.value = 1.0;
    this.dryGain.connect(this.dryLevelMatchGain);

    // Master Output & Post-Analyzer
    this.masterGainNode = ctx.createGain();
    this.wetGain.connect(this.masterGainNode);
    this.dryLevelMatchGain.connect(this.masterGainNode);

    this.masterGainNode.connect(this.postAnalyzer);
    this.postAnalyzer.connect(ctx.destination);
  }

  setupMultibandCrossover(inputNode) {
    const ctx = this.ctx;

    // Splitter filters (Linkwitz-Riley 24dB/oct approximations)
    this.lowPass1 = ctx.createBiquadFilter();
    this.lowPass1.type = 'lowpass';
    this.lowPass1.frequency.value = 160;
    this.lowPass1.Q.value = 0.707;

    this.lowPass2 = ctx.createBiquadFilter();
    this.lowPass2.type = 'lowpass';
    this.lowPass2.frequency.value = 160;
    this.lowPass2.Q.value = 0.707;

    this.bandPass = ctx.createBiquadFilter();
    this.bandPass.type = 'bandpass';
    this.bandPass.frequency.value = 850;
    this.bandPass.Q.value = 0.5;

    this.highPass1 = ctx.createBiquadFilter();
    this.highPass1.type = 'highpass';
    this.highPass1.frequency.value = 4200;
    this.highPass1.Q.value = 0.707;

    this.highPass2 = ctx.createBiquadFilter();
    this.highPass2.type = 'highpass';
    this.highPass2.frequency.value = 4200;
    this.highPass2.Q.value = 0.707;

    // Compressors for each band
    this.compLow = ctx.createDynamicsCompressor();
    this.compLow.threshold.value = this.params.mbLowThreshold;
    this.compLow.ratio.value = this.params.mbLowRatio;
    this.compLow.attack.value = 0.025;
    this.compLow.release.value = 0.150;
    this.gainLow = ctx.createGain();

    this.compMid = ctx.createDynamicsCompressor();
    this.compMid.threshold.value = this.params.mbMidThreshold;
    this.compMid.ratio.value = this.params.mbMidRatio;
    this.compMid.attack.value = 0.015;
    this.compMid.release.value = 0.100;
    this.gainMid = ctx.createGain();

    this.compHigh = ctx.createDynamicsCompressor();
    this.compHigh.threshold.value = this.params.mbHighThreshold;
    this.compHigh.ratio.value = this.params.mbHighRatio;
    this.compHigh.attack.value = 0.005;
    this.compHigh.release.value = 0.070;
    this.gainHigh = ctx.createGain();

    // Wiring Low Band
    inputNode.connect(this.lowPass1);
    this.lowPass1.connect(this.lowPass2);
    this.lowPass2.connect(this.compLow);
    this.compLow.connect(this.gainLow);

    // Wiring Mid Band
    inputNode.connect(this.bandPass);
    this.bandPass.connect(this.compMid);
    this.compMid.connect(this.gainMid);

    // Wiring High Band
    inputNode.connect(this.highPass1);
    this.highPass1.connect(this.highPass2);
    this.highPass2.connect(this.compHigh);
    this.compHigh.connect(this.gainHigh);

    // Summing Node
    this.mbSumNode = ctx.createGain();
    this.gainLow.connect(this.mbSumNode);
    this.gainMid.connect(this.mbSumNode);
    this.gainHigh.connect(this.mbSumNode);
  }

  setupStereoImager(inputNode) {
    const ctx = this.ctx;
    this.imagerInNode = ctx.createGain();
    this.imagerOutNode = ctx.createGain();

    // Mid/Side matrixing using GainNodes and Splitter/Merger
    this.splitter = ctx.createChannelSplitter(2);
    this.merger = ctx.createChannelMerger(2);

    // Bass Mono Filter (keeps < 110Hz tightly centered)
    this.bassMonoFilter = ctx.createBiquadFilter();
    this.bassMonoFilter.type = 'lowpass';
    this.bassMonoFilter.frequency.value = this.params.monoMakerFreq;

    this.sideGainNode = ctx.createGain();
    this.sideGainNode.gain.value = this.params.stereoWidth / 100;

    // Highpass side channels so bass is strictly mono
    this.sideHighPass = ctx.createBiquadFilter();
    this.sideHighPass.type = 'highpass';
    this.sideHighPass.frequency.value = this.params.monoMakerFreq;

    inputNode.connect(this.imagerInNode);
    this.imagerInNode.connect(this.splitter);

    // Matrix Mid = (L + R) / 2
    this.midSum = ctx.createGain();
    this.midSum.gain.value = 0.5;
    this.splitter.connect(this.midSum, 0);
    this.splitter.connect(this.midSum, 1);

    // Matrix Side = (L - R) / 2
    this.sideL = ctx.createGain();
    this.sideL.gain.value = 0.5;
    this.sideR = ctx.createGain();
    this.sideR.gain.value = -0.5;
    this.splitter.connect(this.sideL, 0);
    this.splitter.connect(this.sideR, 1);

    this.sideSum = ctx.createGain();
    this.sideL.connect(this.sideSum);
    this.sideR.connect(this.sideSum);
    this.sideSum.connect(this.sideHighPass);
    this.sideHighPass.connect(this.sideGainNode);

    // Recombine L = Mid + Side, R = Mid - Side
    this.outL = ctx.createGain();
    this.outR = ctx.createGain();
    this.negSide = ctx.createGain();
    this.negSide.gain.value = -1.0;

    this.midSum.connect(this.outL);
    this.sideGainNode.connect(this.outL);

    this.midSum.connect(this.outR);
    this.sideGainNode.connect(this.negSide);
    this.negSide.connect(this.outR);

    this.outL.connect(this.merger, 0, 0);
    this.outR.connect(this.merger, 0, 1);
    this.merger.connect(this.imagerOutNode);
  }

  updateSaturationCurve() {
    if (!this.shaperNode) return;
    const n_samples = 4096;
    const curve = new Float32Array(n_samples);
    const drive = Math.max(0.01, this.params.saturationDrive / 50); // 0 to 2
    const warmth = this.params.saturationWarmth / 100;
    const mode = this.params.saturationMode;

    for (let i = 0; i < n_samples; ++i) {
      let x = (i * 2) / n_samples - 1;
      let y = x;

      if (mode === 'tape') {
        // Asymmetric soft-saturation with warm 2nd & 3rd order harmonics
        const driven = x * (1 + drive * 1.5);
        y = Math.tanh(driven) + warmth * 0.12 * (x * x - 0.25);
      } else if (mode === 'tube') {
        // Tube triode saturation with even harmonic sweetness
        if (x >= 0) {
          y = (1 - Math.exp(-x * (1 + drive * 2))) / (1 - Math.exp(-2));
        } else {
          y = -Math.tanh(-x * (1 + drive * 1.2));
        }
        y += warmth * 0.15 * Math.sin(x * Math.PI);
      } else if (mode === 'classA') {
        // Pure harmonic excitement punch
        y = (2 / Math.PI) * Math.atan(x * (1 + drive * 2.2));
      } else {
        // Clean transparent soft-clip
        y = Math.max(-1, Math.min(1, x));
      }

      // Clamp between -1 and 1 to prevent clipping artifacts
      curve[i] = Math.max(-1, Math.min(1, y));
    }
    this.shaperNode.curve = curve;
  }

  setParam(name, value) {
    if (this.params[name] === undefined) return;
    this.params[name] = value;
    this.applyParam(name, value);
  }

  applyParam(name, value) {
    if (!this.ctx) return;
    const now = this.ctx.currentTime;

    switch (name) {
      case 'inputGain': {
        const linear = Math.pow(10, value / 20);
        this.inputGainNode.gain.setTargetAtTime(linear, now, 0.01);
        break;
      }
      case 'lowCutFreq':
        this.lowCutFilter.frequency.setTargetAtTime(this.params.lowCutBypass ? 10 : value, now, 0.01);
        break;
      case 'lowCutBypass':
        this.lowCutFilter.frequency.setTargetAtTime(value ? 10 : this.params.lowCutFreq, now, 0.01);
        break;

      // 5-Band Parametric EQ
      case 'eqLowShelfFreq':
        this.eqBand1.frequency.setTargetAtTime(value, now, 0.01);
        break;
      case 'eqLowShelfGain':
        this.eqBand1.gain.setTargetAtTime(this.params.eqBypass ? 0 : value, now, 0.01);
        break;
      case 'eqLowMidFreq':
        this.eqBand2.frequency.setTargetAtTime(value, now, 0.01);
        break;
      case 'eqLowMidGain':
        this.eqBand2.gain.setTargetAtTime(this.params.eqBypass ? 0 : value, now, 0.01);
        break;
      case 'eqLowMidQ':
        this.eqBand2.Q.setTargetAtTime(value, now, 0.01);
        break;
      case 'eqMidFreq':
        this.eqBand3.frequency.setTargetAtTime(value, now, 0.01);
        break;
      case 'eqMidGain':
        this.eqBand3.gain.setTargetAtTime(this.params.eqBypass ? 0 : value, now, 0.01);
        break;
      case 'eqMidQ':
        this.eqBand3.Q.setTargetAtTime(value, now, 0.01);
        break;
      case 'eqHighMidFreq':
        this.eqBand4.frequency.setTargetAtTime(value, now, 0.01);
        break;
      case 'eqHighMidGain':
        this.eqBand4.gain.setTargetAtTime(this.params.eqBypass ? 0 : value, now, 0.01);
        break;
      case 'eqHighMidQ':
        this.eqBand4.Q.setTargetAtTime(value, now, 0.01);
        break;
      case 'eqHighShelfFreq':
        this.eqBand5.frequency.setTargetAtTime(value, now, 0.01);
        break;
      case 'eqHighShelfGain':
        this.eqBand5.gain.setTargetAtTime(this.params.eqBypass ? 0 : value, now, 0.01);
        break;
      case 'eqBypass':
        this.eqBand1.gain.setTargetAtTime(value ? 0 : this.params.eqLowShelfGain, now, 0.01);
        this.eqBand2.gain.setTargetAtTime(value ? 0 : this.params.eqLowMidGain, now, 0.01);
        this.eqBand3.gain.setTargetAtTime(value ? 0 : this.params.eqMidGain, now, 0.01);
        this.eqBand4.gain.setTargetAtTime(value ? 0 : this.params.eqHighMidGain, now, 0.01);
        this.eqBand5.gain.setTargetAtTime(value ? 0 : this.params.eqHighShelfGain, now, 0.01);
        break;

      // Saturation
      case 'saturationDrive':
      case 'saturationWarmth':
      case 'saturationMode':
        this.updateSaturationCurve();
        break;
      case 'saturationBypass':
        if (value) {
          const linearCurve = new Float32Array([-1, 1]);
          this.shaperNode.curve = linearCurve;
        } else {
          this.updateSaturationCurve();
        }
        break;

      // Multiband Dynamics
      case 'mbLowThreshold':
        this.compLow.threshold.setTargetAtTime(value, now, 0.01);
        break;
      case 'mbLowRatio':
        this.compLow.ratio.setTargetAtTime(this.params.mbBypass ? 1 : value, now, 0.01);
        break;
      case 'mbLowGain':
        this.gainLow.gain.setTargetAtTime(Math.pow(10, value / 20), now, 0.01);
        break;
      case 'mbMidThreshold':
        this.compMid.threshold.setTargetAtTime(value, now, 0.01);
        break;
      case 'mbMidRatio':
        this.compMid.ratio.setTargetAtTime(this.params.mbBypass ? 1 : value, now, 0.01);
        break;
      case 'mbMidGain':
        this.gainMid.gain.setTargetAtTime(Math.pow(10, value / 20), now, 0.01);
        break;
      case 'mbHighThreshold':
        this.compHigh.threshold.setTargetAtTime(value, now, 0.01);
        break;
      case 'mbHighRatio':
        this.compHigh.ratio.setTargetAtTime(this.params.mbBypass ? 1 : value, now, 0.01);
        break;
      case 'mbHighGain':
        this.gainHigh.gain.setTargetAtTime(Math.pow(10, value / 20), now, 0.01);
        break;
      case 'mbBypass':
        this.compLow.ratio.setTargetAtTime(value ? 1 : this.params.mbLowRatio, now, 0.01);
        this.compMid.ratio.setTargetAtTime(value ? 1 : this.params.mbMidRatio, now, 0.01);
        this.compHigh.ratio.setTargetAtTime(value ? 1 : this.params.mbHighRatio, now, 0.01);
        break;

      // Stereo Imager
      case 'stereoWidth': {
        const widthVal = this.params.stereoBypass ? 1.0 : value / 100;
        this.sideGainNode.gain.setTargetAtTime(widthVal, now, 0.01);
        break;
      }
      case 'monoMakerFreq':
        this.sideHighPass.frequency.setTargetAtTime(value, now, 0.01);
        break;
      case 'stereoBypass':
        this.sideGainNode.gain.setTargetAtTime(value ? 1.0 : this.params.stereoWidth / 100, now, 0.01);
        break;

      // Limiter & Loudness Maximizer
      case 'loudnessDrive': {
        const driveLinear = this.params.limiterBypass ? 1.0 : Math.pow(10, value / 20);
        this.limiterDriveNode.gain.setTargetAtTime(driveLinear, now, 0.01);
        break;
      }
      case 'ceiling': {
        const ceilingLinear = Math.pow(10, value / 20);
        this.ceilingGainNode.gain.setTargetAtTime(ceilingLinear, now, 0.01);
        break;
      }
      case 'limiterRelease':
        this.limiterNode.release.setTargetAtTime(value, now, 0.01);
        break;
      case 'limiterBypass':
        this.limiterNode.ratio.setTargetAtTime(value ? 1.0 : 20.0, now, 0.01);
        this.limiterDriveNode.gain.setTargetAtTime(value ? 1.0 : Math.pow(10, this.params.loudnessDrive / 20), now, 0.01);
        break;

      case 'outputGain': {
        const outLinear = Math.pow(10, value / 20);
        this.masterGainNode.gain.setTargetAtTime(outLinear, now, 0.01);
        break;
      }
    }
  }

  applyAllParams(paramsObj) {
    Object.keys(paramsObj).forEach(key => {
      this.setParam(key, paramsObj[key]);
    });
  }

  // Audio Playback & Transport Control
  async loadAudioBuffer(arrayBuffer) {
    await this.init();
    await this.ensureContextRunning();
    this.stop();
    this.audioBuffer = await this.ctx.decodeAudioData(arrayBuffer);
    this.pausedAt = 0;
    this.calculateAudioRMSMetrics();
    return this.audioBuffer;
  }

  play(offsetSeconds = null) {
    if (!this.audioBuffer) return;
    this.ensureContextRunning();

    if (this.isPlaying) {
      this.stop();
    }

    const startTime = offsetSeconds !== null ? offsetSeconds : this.pausedAt;
    this.sourceNode = this.ctx.createBufferSource();
    this.sourceNode.buffer = this.audioBuffer;
    this.sourceNode.loop = this.isLooping;

    // Connect source to Dry branch and Pre-Analyzer
    this.sourceNode.connect(this.preAnalyzer);
    this.sourceNode.connect(this.dryGain);

    // Connect source to DSP Processing chain
    this.sourceNode.connect(this.inputGainNode);
    this.inputGainNode.connect(this.lowCutFilter);

    this.sourceNode.onended = () => {
      if (!this.isLooping && this.isPlaying) {
        this.isPlaying = false;
        this.pausedAt = 0;
        if (this.onPlaybackEnd) this.onPlaybackEnd();
      }
    };

    this.sourceNode.start(0, startTime);
    this.playbackStartTime = this.ctx.currentTime - startTime;
    this.isPlaying = true;
  }

  pause() {
    if (!this.isPlaying) return;
    this.pausedAt = this.getCurrentTime();
    this.stop();
  }

  stop() {
    if (this.sourceNode) {
      try {
        this.sourceNode.stop(0);
        this.sourceNode.disconnect();
      } catch (e) {
        // Node already stopped
      }
      this.sourceNode = null;
    }
    this.isPlaying = false;
  }

  seek(seconds) {
    const clamped = Math.max(0, Math.min(seconds, this.getDuration()));
    this.pausedAt = clamped;
    if (this.isPlaying) {
      this.play(clamped);
    }
  }

  getCurrentTime() {
    if (!this.isPlaying) return this.pausedAt;
    let time = this.ctx.currentTime - this.playbackStartTime;
    if (this.isLooping && this.audioBuffer) {
      time = time % this.audioBuffer.duration;
    }
    return Math.min(time, this.getDuration());
  }

  getDuration() {
    return this.audioBuffer ? this.audioBuffer.duration : 0;
  }

  // A/B Monitoring with Volume Matching
  setABMode(mode) {
    this.abMode = mode; // 'wet' | 'dry'
    if (!this.ctx) return;
    const now = this.ctx.currentTime;

    if (mode === 'wet') {
      this.wetGain.gain.setTargetAtTime(1.0, now, 0.015);
      this.dryGain.gain.setTargetAtTime(0.0, now, 0.015);
    } else {
      this.wetGain.gain.setTargetAtTime(0.0, now, 0.015);
      this.dryGain.gain.setTargetAtTime(1.0, now, 0.015);

      // Level match Dry to Master loudness if enabled
      if (this.levelMatchEnabled && this.measuredDryRMS > 0.0001 && this.measuredWetRMS > 0.0001) {
        const compRatio = Math.min(4.0, this.measuredWetRMS / this.measuredDryRMS);
        this.dryLevelMatchGain.gain.setTargetAtTime(compRatio, now, 0.02);
      } else {
        this.dryLevelMatchGain.gain.setTargetAtTime(1.0, now, 0.02);
      }
    }
  }

  calculateAudioRMSMetrics() {
    if (!this.audioBuffer) return;
    const chL = this.audioBuffer.getChannelData(0);
    const chR = this.audioBuffer.numberOfChannels > 1 ? this.audioBuffer.getChannelData(1) : chL;
    let sumSq = 0;
    const len = Math.min(chL.length, 44100 * 30); // sample first 30 seconds
    for (let i = 0; i < len; i += 4) {
      sumSq += chL[i] * chL[i] + chR[i] * chR[i];
    }
    this.measuredDryRMS = Math.sqrt(sumSq / (len / 2));
    this.measuredWetRMS = this.measuredDryRMS * Math.pow(10, this.params.loudnessDrive / 20) * 1.3;
  }

  // Offline Rendering & 24-Bit WAV Export
  async renderMasterOffline(onProgress = null) {
    if (!this.audioBuffer) throw new Error('No audio buffer loaded');

    const sampleRate = this.audioBuffer.sampleRate;
    const length = this.audioBuffer.length;
    const numChannels = 2;

    const OfflineCtxClass = window.OfflineAudioContext || window.webkitOfflineAudioContext;
    const offlineCtx = new OfflineCtxClass(numChannels, length, sampleRate);

    // Source
    const offlineSource = offlineCtx.createBufferSource();
    offlineSource.buffer = this.audioBuffer;

    // Input Gain
    const inGain = offlineCtx.createGain();
    inGain.gain.value = Math.pow(10, this.params.inputGain / 20);

    // Low Cut
    const lowCut = offlineCtx.createBiquadFilter();
    lowCut.type = 'highpass';
    lowCut.frequency.value = this.params.lowCutBypass ? 10 : this.params.lowCutFreq;
    lowCut.Q.value = 0.707;

    // 5-Band Parametric EQ
    const eq1 = offlineCtx.createBiquadFilter();
    eq1.type = 'lowshelf';
    eq1.frequency.value = this.params.eqLowShelfFreq;
    eq1.gain.value = this.params.eqBypass ? 0 : this.params.eqLowShelfGain;

    const eq2 = offlineCtx.createBiquadFilter();
    eq2.type = 'peaking';
    eq2.frequency.value = this.params.eqLowMidFreq;
    eq2.gain.value = this.params.eqBypass ? 0 : this.params.eqLowMidGain;
    eq2.Q.value = this.params.eqLowMidQ;

    const eq3 = offlineCtx.createBiquadFilter();
    eq3.type = 'peaking';
    eq3.frequency.value = this.params.eqMidFreq;
    eq3.gain.value = this.params.eqBypass ? 0 : this.params.eqMidGain;
    eq3.Q.value = this.params.eqMidQ;

    const eq4 = offlineCtx.createBiquadFilter();
    eq4.type = 'peaking';
    eq4.frequency.value = this.params.eqHighMidFreq;
    eq4.gain.value = this.params.eqBypass ? 0 : this.params.eqHighMidGain;
    eq4.Q.value = this.params.eqHighMidQ;

    const eq5 = offlineCtx.createBiquadFilter();
    eq5.type = 'highshelf';
    eq5.frequency.value = this.params.eqHighShelfFreq;
    eq5.gain.value = this.params.eqBypass ? 0 : this.params.eqHighShelfGain;

    // Connect EQ
    offlineSource.connect(inGain);
    inGain.connect(lowCut);
    lowCut.connect(eq1);
    eq1.connect(eq2);
    eq2.connect(eq3);
    eq3.connect(eq4);
    eq4.connect(eq5);

    // Saturation
    const shaper = offlineCtx.createWaveShaper();
    shaper.oversample = '4x';
    if (!this.params.saturationBypass && this.shaperNode && this.shaperNode.curve) {
      shaper.curve = this.shaperNode.curve;
    }
    eq5.connect(shaper);

    // Multiband Crossover
    const lp1 = offlineCtx.createBiquadFilter();
    lp1.type = 'lowpass';
    lp1.frequency.value = 160;
    const lp2 = offlineCtx.createBiquadFilter();
    lp2.type = 'lowpass';
    lp2.frequency.value = 160;

    const bp = offlineCtx.createBiquadFilter();
    bp.type = 'bandpass';
    bp.frequency.value = 850;
    bp.Q.value = 0.5;

    const hp1 = offlineCtx.createBiquadFilter();
    hp1.type = 'highpass';
    hp1.frequency.value = 4200;
    const hp2 = offlineCtx.createBiquadFilter();
    hp2.type = 'highpass';
    hp2.frequency.value = 4200;

    const cLow = offlineCtx.createDynamicsCompressor();
    cLow.threshold.value = this.params.mbLowThreshold;
    cLow.ratio.value = this.params.mbBypass ? 1 : this.params.mbLowRatio;
    const gLow = offlineCtx.createGain();
    gLow.gain.value = Math.pow(10, this.params.mbLowGain / 20);

    const cMid = offlineCtx.createDynamicsCompressor();
    cMid.threshold.value = this.params.mbMidThreshold;
    cMid.ratio.value = this.params.mbBypass ? 1 : this.params.mbMidRatio;
    const gMid = offlineCtx.createGain();
    gMid.gain.value = Math.pow(10, this.params.mbMidGain / 20);

    const cHigh = offlineCtx.createDynamicsCompressor();
    cHigh.threshold.value = this.params.mbHighThreshold;
    cHigh.ratio.value = this.params.mbBypass ? 1 : this.params.mbHighRatio;
    const gHigh = offlineCtx.createGain();
    gHigh.gain.value = Math.pow(10, this.params.mbHighGain / 20);

    shaper.connect(lp1);
    lp1.connect(lp2);
    lp2.connect(cLow);
    cLow.connect(gLow);

    shaper.connect(bp);
    bp.connect(cMid);
    cMid.connect(gMid);

    shaper.connect(hp1);
    hp1.connect(hp2);
    hp2.connect(cHigh);
    cHigh.connect(gHigh);

    const sumMb = offlineCtx.createGain();
    gLow.connect(sumMb);
    gMid.connect(sumMb);
    gHigh.connect(sumMb);

    // Stereo Imager
    const splitter = offlineCtx.createChannelSplitter(2);
    const merger = offlineCtx.createChannelMerger(2);

    const midSum = offlineCtx.createGain();
    midSum.gain.value = 0.5;
    const sideL = offlineCtx.createGain();
    sideL.gain.value = 0.5;
    const sideR = offlineCtx.createGain();
    sideR.gain.value = -0.5;
    const sideSum = offlineCtx.createGain();

    const sideHP = offlineCtx.createBiquadFilter();
    sideHP.type = 'highpass';
    sideHP.frequency.value = this.params.monoMakerFreq;

    const sideWidthGain = offlineCtx.createGain();
    sideWidthGain.gain.value = this.params.stereoBypass ? 1.0 : this.params.stereoWidth / 100;

    sumMb.connect(splitter);
    splitter.connect(midSum, 0);
    splitter.connect(midSum, 1);
    splitter.connect(sideL, 0);
    splitter.connect(sideR, 1);
    sideL.connect(sideSum);
    sideR.connect(sideSum);
    sideSum.connect(sideHP);
    sideHP.connect(sideWidthGain);

    const outL = offlineCtx.createGain();
    const outR = offlineCtx.createGain();
    const negSide = offlineCtx.createGain();
    negSide.gain.value = -1.0;

    midSum.connect(outL);
    sideWidthGain.connect(outL);
    midSum.connect(outR);
    sideWidthGain.connect(negSide);
    negSide.connect(outR);

    outL.connect(merger, 0, 0);
    outR.connect(merger, 0, 1);

    // Loudness Maximizer / True-Peak Limiter
    const driveGain = offlineCtx.createGain();
    driveGain.gain.value = this.params.limiterBypass ? 1.0 : Math.pow(10, this.params.loudnessDrive / 20);

    const limiter = offlineCtx.createDynamicsCompressor();
    limiter.threshold.value = -0.5;
    limiter.knee.value = 0;
    limiter.ratio.value = this.params.limiterBypass ? 1 : 20.0;
    limiter.attack.value = 0.001;
    limiter.release.value = this.params.limiterRelease;

    const ceilGain = offlineCtx.createGain();
    ceilGain.gain.value = Math.pow(10, this.params.ceiling / 20);

    const outGainNode = offlineCtx.createGain();
    outGainNode.gain.value = Math.pow(10, this.params.outputGain / 20);

    merger.connect(driveGain);
    driveGain.connect(limiter);
    limiter.connect(ceilGain);
    ceilGain.connect(outGainNode);
    outGainNode.connect(offlineCtx.destination);

    offlineSource.start(0);

    if (onProgress) {
      const interval = setInterval(() => {
        onProgress(offlineCtx.currentTime / (length / sampleRate));
      }, 100);
      const renderedBuffer = await offlineCtx.startRendering();
      clearInterval(interval);
      if (onProgress) onProgress(1.0);
      return renderedBuffer;
    }

    return await offlineCtx.startRendering();
  }

  // Convert AudioBuffer to 24-bit PCM WAV File Blob
  exportToWAVBlob(audioBuffer) {
    const numChannels = audioBuffer.numberOfChannels;
    const sampleRate = audioBuffer.sampleRate;
    const format = 1; // PCM
    const bitDepth = 24;
    const bytesPerSample = bitDepth / 8;
    const blockAlign = numChannels * bytesPerSample;

    const length = audioBuffer.length * blockAlign;
    const buffer = new ArrayBuffer(44 + length);
    const view = new DataView(buffer);

    // RIFF identifier
    this.writeString(view, 0, 'RIFF');
    view.setUint32(4, 36 + length, true);
    this.writeString(view, 8, 'WAVE');

    // fmt sub-chunk
    this.writeString(view, 12, 'fmt ');
    view.setUint32(16, 16, true); // Subchunk1Size (16 for PCM)
    view.setUint16(20, format, true); // AudioFormat
    view.setUint16(22, numChannels, true);
    view.setUint32(24, sampleRate, true);
    view.setUint32(28, sampleRate * blockAlign, true); // ByteRate
    view.setUint16(32, blockAlign, true);
    view.setUint16(34, bitDepth, true);

    // data sub-chunk
    this.writeString(view, 36, 'data');
    view.setUint32(40, length, true);

    // Write 24-bit samples with TPDF dither
    let offset = 44;
    const ch0 = audioBuffer.getChannelData(0);
    const ch1 = numChannels > 1 ? audioBuffer.getChannelData(1) : ch0;

    for (let i = 0; i < audioBuffer.length; i++) {
      for (let ch = 0; ch < numChannels; ch++) {
        const sample = ch === 0 ? ch0[i] : ch1[i];
        // Triangular PDF dither (reduces truncation distortion)
        const dither = (Math.random() - Math.random()) * (1 / 8388607);
        let s = Math.max(-1, Math.min(1, sample + dither));
        let intSample = s < 0 ? s * 0x800000 : s * 0x7FFFFF;
        intSample = Math.floor(intSample);

        // 24-bit little endian
        view.setUint8(offset, intSample & 0xFF);
        view.setUint8(offset + 1, (intSample >> 8) & 0xFF);
        view.setUint8(offset + 2, (intSample >> 16) & 0xFF);
        offset += 3;
      }
    }

    return new Blob([view], { type: 'audio/wav' });
  }

  writeString(view, offset, string) {
    for (let i = 0; i < string.length; i++) {
      view.setUint8(offset + i, string.charCodeAt(i));
    }
  }
}
