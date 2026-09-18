/**
 * AUTOMASTER SUPREME - Rotary Knob & Control Manager
 * Provides:
 * - Professional tactile vertical drag with acceleration
 * - Shift + Drag for ultra-fine mastering calibration
 * - Double-click to reset to default value
 * - Real-time angular SVG/CSS rotation (-135deg to +135deg)
 * - Numeric tooltip & parameter synchronization
 */

export class KnobController {
  constructor(dspEngine, onParamChange = null) {
    this.dsp = dspEngine;
    this.onParamChange = onParamChange;
    this.knobs = new Map();
    this.initKnobs();
  }

  initKnobs() {
    const knobElements = document.querySelectorAll('.supreme-knob');
    knobElements.forEach(el => {
      const param = el.dataset.param;
      const min = parseFloat(el.dataset.min ?? 0);
      const max = parseFloat(el.dataset.max ?? 100);
      const step = parseFloat(el.dataset.step ?? 1);
      const defaultValue = parseFloat(el.dataset.default ?? min);
      const unit = el.dataset.unit || '';
      const decimals = parseInt(el.dataset.decimals ?? (step < 1 ? 1 : 0), 10);

      const knobData = {
        element: el,
        param,
        min,
        max,
        step,
        defaultValue,
        value: defaultValue,
        unit,
        decimals,
        indicator: el.querySelector('.knob-pointer'),
        ring: el.querySelector('.knob-ring-fill'),
        valueDisplay: el.closest('.knob-control')?.querySelector('.knob-val')
      };

      this.knobs.set(param, knobData);
      this.bindKnobEvents(knobData);
      this.updateKnobVisuals(knobData);
    });
  }

  bindKnobEvents(knob) {
    const el = knob.element;

    let startY = 0;
    let startVal = 0;
    let isDragging = false;

    const onMouseDown = (e) => {
      e.preventDefault();
      startY = e.clientY;
      startVal = knob.value;
      isDragging = true;
      document.body.classList.add('knob-dragging');

      window.addEventListener('mousemove', onMouseMove);
      window.addEventListener('mouseup', onMouseUp);
    };

    const onMouseMove = (e) => {
      if (!isDragging) return;
      const deltaY = startY - e.clientY;
      const range = knob.max - knob.min;
      const sensitivity = e.shiftKey ? 0.001 : 0.005; // Shift for fine-tuning

      let newVal = startVal + deltaY * range * sensitivity;
      newVal = Math.max(knob.min, Math.min(knob.max, newVal));

      if (knob.step > 0) {
        newVal = Math.round(newVal / knob.step) * knob.step;
      }

      this.setValue(knob.param, newVal, true);
    };

    const onMouseUp = () => {
      isDragging = false;
      document.body.classList.remove('knob-dragging');
      window.removeEventListener('mousemove', onMouseMove);
      window.removeEventListener('mouseup', onMouseUp);
    };

    el.addEventListener('mousedown', onMouseDown);

    // Double click to reset to default
    el.addEventListener('dblclick', () => {
      this.setValue(knob.param, knob.defaultValue, true);
    });

    // Mouse wheel scrolling
    el.addEventListener('wheel', (e) => {
      e.preventDefault();
      const direction = e.deltaY < 0 ? 1 : -1;
      const fineStep = e.shiftKey ? knob.step * 0.2 : knob.step;
      const newVal = Math.max(knob.min, Math.min(knob.max, knob.value + direction * fineStep));
      this.setValue(knob.param, newVal, true);
    }, { passive: false });
  }

  setValue(param, value, notify = true) {
    const knob = this.knobs.get(param);
    if (!knob) return;

    knob.value = Math.max(knob.min, Math.min(knob.max, value));
    this.updateKnobVisuals(knob);

    if (this.dsp) {
      this.dsp.setParam(param, knob.value);
    }
    if (notify && this.onParamChange) {
      this.onParamChange(param, knob.value);
    }
  }

  updateKnobVisuals(knob) {
    const normalized = (knob.value - knob.min) / (knob.max - knob.min);
    // Angle range: -135deg to +135deg (270 degree sweep)
    const angle = -135 + normalized * 270;

    if (knob.indicator) {
      knob.indicator.style.transform = `rotate(${angle}deg)`;
    }

    if (knob.ring) {
      // SVG stroke-dashoffset or conic gradient
      const circumference = 2 * Math.PI * 22; // 44px radius ring
      const offset = circumference * (1 - normalized * 0.75);
      knob.ring.style.strokeDashoffset = offset;
    }

    if (knob.valueDisplay) {
      const sign = knob.value > 0 && (knob.unit.includes('dB') || knob.param.includes('Gain')) ? '+' : '';
      knob.valueDisplay.textContent = `${sign}${knob.value.toFixed(knob.decimals)}${knob.unit}`;
    }
  }

  syncAllFromEngine() {
    if (!this.dsp || !this.dsp.params) return;
    for (const [key, value] of Object.entries(this.dsp.params)) {
      if (this.knobs.has(key)) {
        this.setValue(key, value, false);
      }
    }
  }
}
