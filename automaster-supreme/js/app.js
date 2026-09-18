/**
 * AUTOMASTER SUPREME - Main Application Entry Point
 * Orchestrates DSP Engine, AI Mastering, UI Knobs, 60fps Visualizers,
 * and Multi-Agent Collaboration.
 */

import { DSPEngine } from './audio/dsp-engine.js';
import { AudioAnalyzer } from './audio/audio-analyzer.js';
import { AIMasterAgent, GENRE_PRESETS } from './audio/ai-master-agent.js';
import { generateSynthesizedDemoTrack } from './audio/synth-demo.js';
import { KnobController } from './ui/knob-controller.js';
import { VisualizerRenderer } from './ui/visualizer-renderer.js';
import { WaveformPlayer } from './ui/waveform-player.js';
import { AgentsBridge } from './agents-bridge.js';

class AutoMasterSupremeApp {
  constructor() {
    this.dsp = new DSPEngine();
    this.analyzer = new AudioAnalyzer(this.dsp);
    this.aiAgent = new AIMasterAgent(this.dsp);
    this.agentsBridge = new AgentsBridge();

    this.knobController = null;
    this.visualizer = null;
    this.waveformPlayer = null;

    this.currentGenre = 'spotify';
  }

  async init() {
    console.log('⚡ Initializing AUTOMASTER SUPREME VST Suite...');

    // Initialize UI Controllers
    this.knobController = new KnobController(this.dsp, (param, val) => {
      // Optional callback on knob change
    });

    this.visualizer = new VisualizerRenderer(this.dsp, this.analyzer);
    this.waveformPlayer = new WaveformPlayer(this.dsp);

    this.bindUI();

    // Welcome log from the Multi-Agent Suite
    this.agentsBridge.log(
      'agentes-personalizados',
      '🚀 Suite AUTOMASTER SUPREME iniciada. 4 Agentes en línea y listos para masterizar.',
      {
        motor: 'Web Audio DSP 64-bit',
        cadena: 'EQ5 + Sat + Multiband + Imager + Limiter',
        estado: 'STANDBY'
      }
    );

    // Auto-load synthesized demo audio so user can hear and test immediately!
    await this.loadDemoTrack();

    // Start 60fps rendering
    this.visualizer.start();
  }

  bindUI() {
    // Preset dropdown / selector buttons
    const presetSelect = document.getElementById('genre-preset-select');
    if (presetSelect) {
      presetSelect.addEventListener('change', (e) => {
        this.selectGenre(e.target.value);
      });
    }

    // AI Auto Master Button
    const autoMasterBtn = document.getElementById('btn-auto-master');
    if (autoMasterBtn) {
      autoMasterBtn.addEventListener('click', () => this.runAIMastering());
    }

    // Demo Track button
    const demoBtn = document.getElementById('btn-load-demo');
    if (demoBtn) {
      demoBtn.addEventListener('click', () => this.loadDemoTrack());
    }

    // Audio File Input & Drag and Drop
    const fileInput = document.getElementById('audio-file-input');
    const dropZone = document.getElementById('drop-zone');

    if (fileInput) {
      fileInput.addEventListener('change', (e) => {
        const file = e.target.files[0];
        if (file) this.loadAudioFile(file);
      });
    }

    if (dropZone) {
      dropZone.addEventListener('dragover', (e) => {
        e.preventDefault();
        dropZone.classList.add('drag-over');
      });
      dropZone.addEventListener('dragleave', () => {
        dropZone.classList.remove('drag-over');
      });
      dropZone.addEventListener('drop', (e) => {
        e.preventDefault();
        dropZone.classList.remove('drag-over');
        const file = e.dataTransfer.files[0];
        if (file) this.loadAudioFile(file);
      });
      dropZone.addEventListener('click', () => {
        fileInput?.click();
      });
    }

    // A/B Monitoring Switches
    const btnABWet = document.getElementById('btn-ab-wet');
    const btnABDry = document.getElementById('btn-ab-dry');
    const chkLevelMatch = document.getElementById('chk-level-match');

    if (btnABWet && btnABDry) {
      btnABWet.addEventListener('click', () => {
        this.dsp.setABMode('wet');
        btnABWet.classList.add('active');
        btnABDry.classList.remove('active');
        this.agentsBridge.log('agentes-personalizados', '🎧 Monitoreo: MÁSTER (Procesado Wet activo)');
      });
      btnABDry.addEventListener('click', () => {
        this.dsp.setABMode('dry');
        btnABDry.classList.add('active');
        btnABWet.classList.remove('active');
        this.agentsBridge.log('agentes-personalizados', '🎧 Monitoreo: MEZCLA ORIGINAL (Bypass Dry con Level Match)');
      });
    }

    if (chkLevelMatch) {
      chkLevelMatch.addEventListener('change', (e) => {
        this.dsp.levelMatchEnabled = e.target.checked;
      });
    }

    // Module Bypass Switches
    this.bindModuleBypass('btn-bypass-lowcut', 'lowCutBypass');
    this.bindModuleBypass('btn-bypass-eq', 'eqBypass');
    this.bindModuleBypass('btn-bypass-sat', 'saturationBypass');
    this.bindModuleBypass('btn-bypass-mb', 'mbBypass');
    this.bindModuleBypass('btn-bypass-stereo', 'stereoBypass');
    this.bindModuleBypass('btn-bypass-limiter', 'limiterBypass');

    // Saturation Mode selector
    const satModeSelect = document.getElementById('sat-mode-select');
    if (satModeSelect) {
      satModeSelect.addEventListener('change', (e) => {
        this.dsp.setParam('saturationMode', e.target.value);
      });
    }

    // Export Master Button & Modal
    const exportBtn = document.getElementById('btn-export-master');
    const exportModal = document.getElementById('export-modal');
    const btnDoExport = document.getElementById('btn-do-export');
    const btnCancelExport = document.getElementById('btn-cancel-export');

    if (exportBtn && exportModal) {
      exportBtn.addEventListener('click', () => {
        exportModal.classList.add('visible');
      });
    }

    if (btnCancelExport && exportModal) {
      btnCancelExport.addEventListener('click', () => {
        exportModal.classList.remove('visible');
      });
    }

    if (btnDoExport) {
      btnDoExport.addEventListener('click', () => this.exportMasterAudio());
    }

    // Preset Save / Load (JSON)
    const btnSavePreset = document.getElementById('btn-save-preset');
    const btnLoadPreset = document.getElementById('btn-load-preset');
    const presetFileInput = document.getElementById('preset-file-input');

    if (btnSavePreset) {
      btnSavePreset.addEventListener('click', () => this.exportPresetJSON());
    }
    if (btnLoadPreset && presetFileInput) {
      btnLoadPreset.addEventListener('click', () => presetFileInput.click());
      presetFileInput.addEventListener('change', (e) => {
        const file = e.target.files[0];
        if (file) this.importPresetJSON(file);
      });
    }
  }

  bindModuleBypass(btnId, paramName) {
    const btn = document.getElementById(btnId);
    if (!btn) return;
    btn.addEventListener('click', () => {
      const isBypassed = !this.dsp.params[paramName];
      this.dsp.setParam(paramName, isBypassed);
      btn.classList.toggle('bypassed', isBypassed);
      btn.textContent = isBypassed ? 'BYPASS' : 'ON';
    });
  }

  selectGenre(genreKey) {
    this.currentGenre = genreKey;
    const preset = GENRE_PRESETS[genreKey];
    if (!preset) return;

    this.dsp.applyAllParams(preset);
    this.knobController.syncAllFromEngine();

    const satSelect = document.getElementById('sat-mode-select');
    if (satSelect && preset.saturationMode) {
      satSelect.value = preset.saturationMode;
    }

    this.agentsBridge.log(
      'agentes-personalizados',
      `🎛️ Preset seleccionado: ${preset.name}`,
      {
        objetivo: `${preset.targetLUFS} LUFS`,
        techo: `${preset.ceiling} dBTP`,
        ancho: `${preset.stereoWidth}%`
      }
    );
  }

  async runAIMastering() {
    if (!this.dsp.audioBuffer) {
      alert('Por favor carga o genera una pista de audio primero.');
      return;
    }

    const btn = document.getElementById('btn-auto-master');
    if (btn) btn.classList.add('loading');

    try {
      const result = await this.aiAgent.analyzeAndMaster(
        this.dsp.audioBuffer,
        this.currentGenre,
        (progress) => {
          this.agentsBridge.log(progress.agent, progress.message, progress.details);
        }
      );

      this.knobController.syncAllFromEngine();

      // Show summary banner
      const banner = document.getElementById('ai-master-banner');
      if (banner) {
        banner.classList.add('visible');
        const text = banner.querySelector('.banner-text');
        if (text) {
          text.innerHTML = `✅ Masterización completada por IA: <strong>${result.metrics.genre}</strong> | Objetivo: <strong>${result.metrics.targetLUFS} LUFS</strong> | Crest Factor inicial: <strong>${result.metrics.crestFactor.toFixed(1)} dB</strong>`;
        }
        setTimeout(() => banner.classList.remove('visible'), 6000);
      }
    } catch (err) {
      console.error(err);
      this.agentsBridge.log('revisor-codigo', `⚠️ Error en análisis IA: ${err.message}`);
    } finally {
      if (btn) btn.classList.remove('loading');
    }
  }

  async loadDemoTrack() {
    this.agentsBridge.log('agentes-personalizados', '⏳ Sintetizando pista multitrack demo (44.1kHz Estéreo)...');

    const demoData = generateSynthesizedDemoTrack(44100);
    await this.dsp.init();

    // Create an AudioBuffer from synth data
    const buffer = this.dsp.ctx.createBuffer(2, demoData.left.length, demoData.sampleRate);
    buffer.copyToChannel(demoData.left, 0);
    buffer.copyToChannel(demoData.right, 1);

    this.dsp.audioBuffer = buffer;
    this.dsp.pausedAt = 0;
    this.dsp.calculateAudioRMSMetrics();

    this.waveformPlayer.setTrackName('DEMO: Cyber-Synthwave Groove (124 BPM)');
    this.waveformPlayer.updateTimeDisplay();
    this.waveformPlayer.draw();

    this.agentsBridge.log(
      'arquitecto-documentador',
      '🎵 Pista Demo generada con éxito. Lista para masterizar.',
      {
        canales: '2 (Estéreo)',
        duración: `${demoData.duration.toFixed(1)}s`,
        pico: '-3.0 dBFS'
      }
    );
  }

  async loadAudioFile(file) {
    this.agentsBridge.log('revisor-codigo', `📂 Cargando archivo: ${file.name} (${(file.size / 1024 / 1024).toFixed(2)} MB)...`);

    try {
      const arrayBuffer = await file.arrayBuffer();
      await this.dsp.loadAudioBuffer(arrayBuffer);

      this.waveformPlayer.setTrackName(file.name);
      this.waveformPlayer.updateTimeDisplay();
      this.waveformPlayer.draw();

      this.agentsBridge.log(
        'generador-pruebas',
        `✅ Archivo cargado y decodificado correctamente.`,
        {
          duración: `${this.dsp.getDuration().toFixed(1)}s`,
          sampleRate: `${this.dsp.audioBuffer.sampleRate} Hz`,
          canales: this.dsp.audioBuffer.numberOfChannels
        }
      );
    } catch (err) {
      console.error(err);
      this.agentsBridge.log('revisor-codigo', `❌ Error al decodificar audio: ${err.message}`);
      alert('No se pudo decodificar el archivo de audio. Verifica que sea un WAV o MP3 válido.');
    }
  }

  async exportMasterAudio() {
    const modal = document.getElementById('export-modal');
    const progressBar = document.getElementById('export-progress-bar');
    const statusText = document.getElementById('export-status-text');

    if (statusText) statusText.textContent = 'Renderizando máster con precisión de 64-bit...';

    try {
      this.agentsBridge.log('generador-pruebas', '⚡ Iniciando render offline y codificación WAV 24-bit PCM...');

      const renderedBuffer = await this.dsp.renderMasterOffline((progress) => {
        if (progressBar) progressBar.style.width = `${Math.floor(progress * 100)}%`;
        if (statusText) statusText.textContent = `Procesando: ${Math.floor(progress * 100)}%`;
      });

      if (statusText) statusText.textContent = 'Generando archivo WAV descargable con dither TPDF...';

      const wavBlob = this.dsp.exportToWAVBlob(renderedBuffer);
      const url = URL.createObjectURL(wavBlob);

      // Trigger download
      const a = document.createElement('a');
      a.style.display = 'none';
      a.href = url;
      const originalTitle = document.getElementById('track-title')?.textContent || 'track';
      const cleanTitle = originalTitle.replace(/[^a-zA-Z0-9_-]/g, '_');
      a.download = `${cleanTitle}_AUTOMASTER_SUPREME_24bit.wav`;
      document.body.appendChild(a);
      a.click();
      document.body.removeChild(a);

      this.agentsBridge.log(
        'arquitecto-documentador',
        '🎉 Masterización exportada con éxito en 24-bit / 44.1-48kHz WAV.',
        {
          archivo: a.download,
          tamaño: `${(wavBlob.size / 1024 / 1024).toFixed(2)} MB`
        }
      );

      if (modal) modal.classList.remove('visible');
    } catch (err) {
      console.error(err);
      alert(`Error durante la exportación: ${err.message}`);
      if (modal) modal.classList.remove('visible');
    }
  }

  exportPresetJSON() {
    const jsonStr = JSON.stringify(this.dsp.params, null, 2);
    const blob = new Blob([jsonStr], { type: 'application/json' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = `AUTOMASTER_Preset_${Date.now()}.json`;
    a.click();
    this.agentsBridge.log('arquitecto-documentador', `💾 Preset exportado como ${a.download}`);
  }

  async importPresetJSON(file) {
    try {
      const text = await file.text();
      const params = JSON.parse(text);
      this.dsp.applyAllParams(params);
      this.knobController.syncAllFromEngine();
      this.agentsBridge.log('arquitecto-documentador', `📥 Preset importado desde ${file.name}`);
    } catch (e) {
      alert('Archivo de preset JSON inválido.');
    }
  }
}

// Launch application on DOM ready
window.addEventListener('DOMContentLoaded', () => {
  const app = new AutoMasterSupremeApp();
  app.init();
});
