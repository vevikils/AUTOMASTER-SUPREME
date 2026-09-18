/**
 * AUTOMASTER SUPREME - Waveform Visualizer & Transport Controller
 * Renders the audio waveform with real-time scrub head, loop points, and timecode.
 */

export class WaveformPlayer {
  constructor(dspEngine) {
    this.dsp = dspEngine;
    this.canvas = document.getElementById('waveform-canvas');
    this.ctx = this.canvas?.getContext('2d');

    // DOM Controls
    this.playBtn = document.getElementById('btn-play');
    this.stopBtn = document.getElementById('btn-stop');
    this.loopBtn = document.getElementById('btn-loop');
    this.timeCurrent = document.getElementById('time-current');
    this.timeTotal = document.getElementById('time-total');
    this.trackName = document.getElementById('track-title');

    this.isSeeking = false;
    this.bindEvents();
  }

  bindEvents() {
    if (this.playBtn) {
      this.playBtn.addEventListener('click', () => this.togglePlay());
    }
    if (this.stopBtn) {
      this.stopBtn.addEventListener('click', () => {
        this.dsp.stop();
        this.dsp.seek(0);
        this.updatePlayState();
        this.draw();
      });
    }
    if (this.loopBtn) {
      this.loopBtn.addEventListener('click', () => {
        this.dsp.isLooping = !this.dsp.isLooping;
        this.loopBtn.classList.toggle('active', this.dsp.isLooping);
      });
    }

    if (this.canvas) {
      const handleSeek = (e) => {
        const rect = this.canvas.getBoundingClientRect();
        const normX = Math.max(0, Math.min(1, (e.clientX - rect.left) / rect.width));
        const seekTime = normX * this.dsp.getDuration();
        this.dsp.seek(seekTime);
        this.draw();
      };

      this.canvas.addEventListener('mousedown', (e) => {
        this.isSeeking = true;
        handleSeek(e);
      });

      window.addEventListener('mousemove', (e) => {
        if (this.isSeeking) handleSeek(e);
      });

      window.addEventListener('mouseup', () => {
        this.isSeeking = false;
      });
    }

    this.dsp.onPlaybackEnd = () => {
      this.updatePlayState();
      this.draw();
    };

    // Animation ticker for playhead
    const tick = () => {
      if (this.dsp.isPlaying) {
        this.updateTimeDisplay();
        this.draw();
      }
      requestAnimationFrame(tick);
    };
    requestAnimationFrame(tick);
  }

  togglePlay() {
    if (this.dsp.isPlaying) {
      this.dsp.pause();
    } else {
      this.dsp.play();
    }
    this.updatePlayState();
  }

  updatePlayState() {
    if (this.playBtn) {
      this.playBtn.classList.toggle('playing', this.dsp.isPlaying);
      const icon = this.playBtn.querySelector('.icon');
      if (icon) icon.textContent = this.dsp.isPlaying ? '⏸' : '▶';
    }
  }

  setTrackName(name) {
    if (this.trackName) {
      this.trackName.textContent = name;
    }
  }

  updateTimeDisplay() {
    const cur = this.dsp.getCurrentTime();
    const dur = this.dsp.getDuration();
    if (this.timeCurrent) this.timeCurrent.textContent = this.formatTime(cur);
    if (this.timeTotal) this.timeTotal.textContent = this.formatTime(dur);
  }

  formatTime(seconds) {
    if (isNaN(seconds) || seconds < 0) return '00:00.0';
    const mins = Math.floor(seconds / 60);
    const secs = Math.floor(seconds % 60);
    const ms = Math.floor((seconds % 1) * 10);
    return `${String(mins).padStart(2, '0')}:${String(secs).padStart(2, '0')}.${ms}`;
  }

  draw() {
    const canvas = this.canvas;
    const ctx = this.ctx;
    if (!canvas || !ctx) return;

    const width = canvas.width;
    const height = canvas.height;
    ctx.clearRect(0, 0, width, height);

    // Background
    ctx.fillStyle = '#080c10';
    ctx.fillRect(0, 0, width, height);

    if (!this.dsp.audioBuffer) {
      // Empty placeholder line
      ctx.strokeStyle = 'rgba(255, 255, 255, 0.15)';
      ctx.lineWidth = 1;
      ctx.beginPath();
      ctx.moveTo(0, height / 2);
      ctx.lineTo(width, height / 2);
      ctx.stroke();
      return;
    }

    const buffer = this.dsp.audioBuffer;
    const channelData = buffer.getChannelData(0);
    const totalSamples = channelData.length;
    const step = Math.ceil(totalSamples / width);
    const amp = height / 2;

    const curTime = this.dsp.getCurrentTime();
    const dur = buffer.duration;
    const playheadX = dur > 0 ? (curTime / dur) * width : 0;

    // Draw waveform bars
    for (let x = 0; x < width; x++) {
      let min = 1.0;
      let max = -1.0;
      const start = x * step;
      const end = Math.min(totalSamples, start + step);

      for (let j = start; j < end; j += 4) {
        const val = channelData[j];
        if (val < min) min = val;
        if (val > max) max = val;
      }

      const yTop = (1 - max) * amp;
      const yBottom = (1 - min) * amp;
      const barHeight = Math.max(2, yBottom - yTop);

      // Color played portion cyan, unplayed portion subtle titanium
      if (x <= playheadX) {
        ctx.fillStyle = '#00f0ff';
      } else {
        ctx.fillStyle = '#3a4a58';
      }

      ctx.fillRect(x, yTop, 1, barHeight);
    }

    // Draw Playhead line
    ctx.save();
    ctx.strokeStyle = '#ffb92d';
    ctx.shadowColor = '#ffb92d';
    ctx.shadowBlur = 6;
    ctx.lineWidth = 2;
    ctx.beginPath();
    ctx.moveTo(playheadX, 0);
    ctx.lineTo(playheadX, height);
    ctx.stroke();
    ctx.restore();
  }
}
