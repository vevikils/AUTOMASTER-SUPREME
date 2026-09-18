/**
 * AUTOMASTER SUPREME - Built-in Multi-Track Music Synthesizer Demo
 * Generates an instant high-energy 16-bar mixed audio track (EDM/Synthwave/Urban Groove)
 * with kick, snare, hi-hats, sub-bass, and lush chord progression.
 * Allows instant testing of the mastering suite without requiring external files!
 */

export function generateSynthesizedDemoTrack(sampleRate = 44100) {
  const bpm = 124;
  const beatSec = 60 / bpm;
  const barSec = beatSec * 4;
  const totalBars = 8; // 8 bars loop (~15.5 seconds)
  const duration = barSec * totalBars;
  const totalSamples = Math.floor(duration * sampleRate);

  // Stereo Buffer
  const left = new Float32Array(totalSamples);
  const right = new Float32Array(totalSamples);

  // Synthesize Drums & Instruments
  // Chord progression: Am - F - C - G
  const chordFreqs = [
    [220, 261.63, 329.63, 392], // Am7
    [174.61, 220, 261.63, 329.63], // Fmaj7
    [130.81, 164.81, 196, 246.94], // Cmaj7
    [196, 246.94, 293.66, 349.23]  // G7
  ];

  const totalBeats = totalBars * 4;

  for (let beat = 0; beat < totalBeats; beat++) {
    const beatTime = beat * beatSec;
    const startSample = Math.floor(beatTime * sampleRate);
    const barIndex = Math.floor(beat / 4);
    const chordIndex = barIndex % 4;

    // 1. Kick Drum on every beat (4-on-the-floor)
    renderKick(left, right, startSample, sampleRate);

    // 2. Snare / Clap on beats 1 and 3 (0-indexed: beats 1 and 3)
    if (beat % 2 === 1) {
      renderSnare(left, right, startSample, sampleRate);
    }

    // 3. Hi-Hats on every eighth note
    renderHiHat(left, right, startSample + Math.floor(beatSec * 0.5 * sampleRate), sampleRate, 0.25);
    renderHiHat(left, right, startSample, sampleRate, 0.15);

    // 4. Bass synth (Pumping eighth notes)
    const bassNote = chordFreqs[chordIndex][0] / 2; // Sub octave
    renderBass(left, right, startSample, Math.floor(beatSec * 0.9 * sampleRate), bassNote, sampleRate);

    // 5. Lush stereo pad chords
    if (beat % 4 === 0) {
      renderPad(left, right, startSample, Math.floor(barSec * sampleRate), chordFreqs[chordIndex], sampleRate);
    }
  }

  // Normalize mix to -3 dBFS peak to give realistic unmastered headroom
  let maxPeak = 0;
  for (let i = 0; i < totalSamples; i++) {
    const absL = Math.abs(left[i]);
    const absR = Math.abs(right[i]);
    if (absL > maxPeak) maxPeak = absL;
    if (absR > maxPeak) maxPeak = absR;
  }

  const targetPeak = Math.pow(10, -3 / 20); // ~0.707
  if (maxPeak > 0) {
    const scale = targetPeak / maxPeak;
    for (let i = 0; i < totalSamples; i++) {
      left[i] *= scale;
      right[i] *= scale;
    }
  }

  return { left, right, sampleRate, duration };
}

function renderKick(left, right, startIdx, sr) {
  const kickDuration = Math.floor(0.35 * sr);
  for (let i = 0; i < kickDuration; i++) {
    const idx = startIdx + i;
    if (idx >= left.length) break;
    const t = i / sr;
    // Pitch envelope: 160Hz dropping to 48Hz
    const pitch = 48 + 112 * Math.exp(-t * 35);
    // Amp envelope: punchy exponential decay
    const amp = Math.exp(-t * 12);
    // Click click transient
    const click = i < 150 ? (Math.random() * 2 - 1) * 0.4 : 0;
    const sample = (Math.sin(2 * Math.PI * pitch * t) + click) * amp * 0.75;

    // Centered in mono
    left[idx] += sample;
    right[idx] += sample;
  }
}

function renderSnare(left, right, startIdx, sr) {
  const snareDur = Math.floor(0.28 * sr);
  for (let i = 0; i < snareDur; i++) {
    const idx = startIdx + i;
    if (idx >= left.length) break;
    const t = i / sr;
    // Tone
    const tone = Math.sin(2 * Math.PI * 185 * t) * Math.exp(-t * 22);
    // Noise snap with stereo spread
    const noiseL = (Math.random() * 2 - 1) * Math.exp(-t * 16);
    const noiseR = (Math.random() * 2 - 1) * Math.exp(-t * 16);

    left[idx] += (tone * 0.4 + noiseL * 0.45) * 0.55;
    right[idx] += (tone * 0.4 + noiseR * 0.45) * 0.55;
  }
}

function renderHiHat(left, right, startIdx, sr, gain = 0.2) {
  const hatDur = Math.floor(0.08 * sr);
  for (let i = 0; i < hatDur; i++) {
    const idx = startIdx + i;
    if (idx >= left.length) break;
    const t = i / sr;
    const env = Math.exp(-t * 60);
    const metallicL = (Math.sin(t * 14000) + Math.random() * 2 - 1) * env * gain;
    const metallicR = (Math.sin(t * 14200) + Math.random() * 2 - 1) * env * gain;
    left[idx] += metallicL * 0.8;
    right[idx] += metallicR * 1.1; // slight right pan
  }
}

function renderBass(left, right, startIdx, durationSamples, freq, sr) {
  for (let i = 0; i < durationSamples; i++) {
    const idx = startIdx + i;
    if (idx >= left.length) break;
    const t = i / sr;
    const env = Math.sin(Math.min(Math.PI, (i / durationSamples) * Math.PI));
    // Warm saw/sine mix
    const fundamental = Math.sin(2 * Math.PI * freq * t);
    const harmonic2 = Math.sin(2 * Math.PI * freq * 2 * t) * 0.35;
    const bass = (fundamental + harmonic2) * env * 0.45;

    // Centered mono bass
    left[idx] += bass;
    right[idx] += bass;
  }
}

function renderPad(left, right, startIdx, durationSamples, freqs, sr) {
  for (let i = 0; i < durationSamples; i++) {
    const idx = startIdx + i;
    if (idx >= left.length) break;
    const t = i / sr;
    // Smooth envelope attack and release
    const norm = i / durationSamples;
    const env = Math.sin(norm * Math.PI);

    let padL = 0;
    let padR = 0;

    freqs.forEach((f, fi) => {
      // Detuned chorus spread
      const lDetune = f * 0.997;
      const rDetune = f * 1.003;
      padL += Math.sin(2 * Math.PI * lDetune * t) * 0.08;
      padR += Math.sin(2 * Math.PI * rDetune * t) * 0.08;
    });

    left[idx] += padL * env * 0.55;
    right[idx] += padR * env * 0.55;
  }
}
