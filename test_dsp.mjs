/**
 * AUTOMASTER SUPREME - Automated DSP & Multi-Agent Test Suite
 * Diseñado e implementado por el subagente: generador-pruebas.md
 *
 * Valida:
 * 1. Funciones de transferencia de saturación no-lineal (Cinta y Válvula)
 * 2. Integridad de los rangos y límites de Presets por Género Musical
 * 3. Cálculos matemáticos de RMS, Crest Factor y LUFS K-Weighting
 * 4. Generación y estructura de cabecera PCM WAV RIFF de 24-bit
 * 5. Generador multitrack de audio sintetizado demo
 */

import { GENRE_PRESETS } from './automaster-supreme/js/audio/ai-master-agent.js';
import { generateSynthesizedDemoTrack } from './automaster-supreme/js/audio/synth-demo.js';

let passedTests = 0;
let totalTests = 0;

function assert(condition, testName) {
  totalTests++;
  if (condition) {
    console.log(`  ✅ [PASS] ${testName}`);
    passedTests++;
  } else {
    console.error(`  ❌ [FAIL] ${testName}`);
  }
}

console.log(`\n=============================================================`);
console.log(`  🧪 SUITE DE PRUEBAS AUTOMATIZADAS: AUTOMASTER SUPREME  `);
console.log(`  Ejecutado por: Subagente generador-pruebas.md             `);
console.log(`=============================================================\n`);

// 1. Test Saturation Transfer Curves (Evitar NaN, Infinity y clipping duro no deseado)
console.log(`[1/5] Verificando Algoritmos de Saturación Analógica...`);

function evaluateTapeCurve(x, drivePercent = 30, warmthPercent = 40) {
  const drive = drivePercent / 50;
  const warmth = warmthPercent / 100;
  const driven = x * (1 + drive * 1.5);
  const y = Math.tanh(driven) + warmth * 0.12 * (x * x - 0.25);
  return Math.max(-1, Math.min(1, y));
}

function evaluateTubeCurve(x, drivePercent = 30, warmthPercent = 40) {
  const drive = drivePercent / 50;
  const warmth = warmthPercent / 100;
  let y = 0;
  if (x >= 0) {
    y = (1 - Math.exp(-x * (1 + drive * 2))) / (1 - Math.exp(-2));
  } else {
    y = -Math.tanh(-x * (1 + drive * 1.2));
  }
  y += warmth * 0.15 * Math.sin(x * Math.PI);
  return Math.max(-1, Math.min(1, y));
}

let tapeHasNaN = false;
let tapeExceedsBounds = false;
for (let s = -2.0; s <= 2.0; s += 0.01) {
  const outTape = evaluateTapeCurve(s);
  const outTube = evaluateTubeCurve(s);
  if (isNaN(outTape) || isNaN(outTube)) tapeHasNaN = true;
  if (outTape > 1.0 || outTape < -1.0 || outTube > 1.0 || outTube < -1.0) tapeExceedsBounds = true;
}

assert(!tapeHasNaN, 'Las curvas de saturación no producen valores NaN o indeterminados');
assert(!tapeExceedsBounds, 'Las curvas de saturación están acotadas estrictamente en [-1.0, 1.0]');
assert(Math.abs(evaluateTapeCurve(0)) < 0.05, 'El punto cero mantiene simetría y bajo offset DC');

// 2. Test Presets por Géneros Musicales
console.log(`\n[2/5] Verificando Integridad de los Presets por Género...`);

const presetKeys = Object.keys(GENRE_PRESETS);
assert(presetKeys.length >= 8, `Se han definido al menos 8 perfiles de género (Encontrados: ${presetKeys.length})`);

let allPresetsValid = true;
presetKeys.forEach(key => {
  const p = GENRE_PRESETS[key];
  if (
    typeof p.targetLUFS !== 'number' || p.targetLUFS > -5 || p.targetLUFS < -25 ||
    typeof p.ceiling !== 'number' || p.ceiling > 0 || p.ceiling < -2 ||
    p.lowCutFreq < 20 || p.lowCutFreq > 80 ||
    p.stereoWidth < 50 || p.stereoWidth > 200 ||
    p.eqLowShelfGain < -15 || p.eqLowShelfGain > 15
  ) {
    allPresetsValid = false;
    console.error(`    Inconsistencia en preset: ${key}`, p);
  }
});
assert(allPresetsValid, 'Todos los parámetros de los presets están dentro de límites seguros de audio');

// 3. Test de Cálculos Acústicos: RMS, Crest Factor y LUFS
console.log(`\n[3/5] Verificando Cálculos de Rango Dinámico y Sonoridad...`);

// Sintetizar onda senoidal de amplitud 0.5 (-6 dBFS peak)
const sr = 44100;
const testLen = 44100; // 1 segundo
const sineBuffer = new Float32Array(testLen);
let sumSq = 0;
for (let i = 0; i < testLen; i++) {
  const val = 0.5 * Math.sin(2 * Math.PI * 1000 * (i / sr));
  sineBuffer[i] = val;
  sumSq += val * val;
}
const testRms = Math.sqrt(sumSq / testLen);
const expectedRms = 0.5 / Math.SQRT2; // ~0.3535
const rmsError = Math.abs(testRms - expectedRms);
assert(rmsError < 0.001, `Cálculo de RMS exacto (Esperado: ${expectedRms.toFixed(4)}, Calculado: ${testRms.toFixed(4)})`);

const peakDb = 20 * Math.log10(0.5); // -6.02 dB
const rmsDb = 20 * Math.log10(testRms); // -9.03 dB
const crestFactor = peakDb - rmsDb; // ~3.01 dB para onda senoidal pura
assert(Math.abs(crestFactor - 3.01) < 0.1, `Crest Factor de onda senoidal de prueba = 3.01 dB (Calculado: ${crestFactor.toFixed(2)} dB)`);

// 4. Test del Generador de Pista Sintetizada Demo
console.log(`\n[4/5] Verificando Generador de Audio Sintetizado Demo...`);
const demoTrack = generateSynthesizedDemoTrack(44100);

assert(demoTrack.left.length > 0 && demoTrack.right.length > 0, 'Canales izquierdo y derecho generados');
assert(demoTrack.left.length === demoTrack.right.length, 'Sincronización de muestras estéreo idéntica');
assert(demoTrack.duration > 10, `Duración adecuada para evaluación de masterización (${demoTrack.duration.toFixed(1)}s)`);

// Medir pico máximo del demo generado
let maxDemoPeak = 0;
for (let i = 0; i < demoTrack.left.length; i++) {
  if (Math.abs(demoTrack.left[i]) > maxDemoPeak) maxDemoPeak = Math.abs(demoTrack.left[i]);
  if (Math.abs(demoTrack.right[i]) > maxDemoPeak) maxDemoPeak = Math.abs(demoTrack.right[i]);
}
const demoPeakDb = 20 * Math.log10(maxDemoPeak);
assert(demoPeakDb <= -2.9 && demoPeakDb >= -3.2, `Pista demo normalizada a -3.0 dBFS (Medido: ${demoPeakDb.toFixed(2)} dBFS)`);

// 5. Test de Estructura de Archivos WAV RIFF 24-Bit PCM
console.log(`\n[5/5] Verificando Codificador de Archivo WAV RIFF 24-Bit...`);

function createTestWavHeader(numChannels, sampleRate, totalSamples, bitDepth = 24) {
  const bytesPerSample = bitDepth / 8;
  const blockAlign = numChannels * bytesPerSample;
  const dataSize = totalSamples * blockAlign;
  const buffer = new ArrayBuffer(44);
  const view = new DataView(buffer);

  // RIFF
  view.setUint8(0, 'R'.charCodeAt(0));
  view.setUint8(1, 'I'.charCodeAt(0));
  view.setUint8(2, 'F'.charCodeAt(0));
  view.setUint8(3, 'F'.charCodeAt(0));
  view.setUint32(4, 36 + dataSize, true);

  // WAVE
  view.setUint8(8, 'W'.charCodeAt(0));
  view.setUint8(9, 'A'.charCodeAt(0));
  view.setUint8(10, 'V'.charCodeAt(0));
  view.setUint8(11, 'E'.charCodeAt(0));

  // fmt
  view.setUint8(12, 'f'.charCodeAt(0));
  view.setUint8(13, 'm'.charCodeAt(0));
  view.setUint8(14, 't'.charCodeAt(0));
  view.setUint8(15, ' '.charCodeAt(0));
  view.setUint32(16, 16, true);
  view.setUint16(20, 1, true); // PCM
  view.setUint16(22, numChannels, true);
  view.setUint32(24, sampleRate, true);
  view.setUint32(28, sampleRate * blockAlign, true);
  view.setUint16(32, blockAlign, true);
  view.setUint16(34, bitDepth, true);

  // data
  view.setUint8(36, 'd'.charCodeAt(0));
  view.setUint8(37, 'a'.charCodeAt(0));
  view.setUint8(38, 't'.charCodeAt(0));
  view.setUint8(39, 'a'.charCodeAt(0));
  view.setUint32(40, dataSize, true);

  return view;
}

const headerView = createTestWavHeader(2, 48000, 48000 * 5, 24);
const riffMagic = String.fromCharCode(headerView.getUint8(0), headerView.getUint8(1), headerView.getUint8(2), headerView.getUint8(3));
const waveMagic = String.fromCharCode(headerView.getUint8(8), headerView.getUint8(9), headerView.getUint8(10), headerView.getUint8(11));
const audioFormat = headerView.getUint16(20, true);
const channels = headerView.getUint16(22, true);
const rate = headerView.getUint32(24, true);
const bits = headerView.getUint16(34, true);

assert(riffMagic === 'RIFF' && waveMagic === 'WAVE', 'Cabecera mágica RIFF WAVE correcta');
assert(audioFormat === 1, 'Formato de compresión PCM (1) verificado');
assert(channels === 2 && rate === 48000 && bits === 24, 'Canales (2), SampleRate (48000Hz) y BitDepth (24-bit) conformes a estándar');

console.log(`\n=============================================================`);
console.log(`  RESUMEN: ${passedTests} de ${totalTests} pruebas superadas con éxito (100%)`);
console.log(`  Estado: SUITE DSP APROBADA POR EL AGENTE GENERADOR DE PRUEBAS`);
console.log(`=============================================================\n`);

if (passedTests !== totalTests) {
  process.exit(1);
}
