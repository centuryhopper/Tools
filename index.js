


let ctx, noiseSource, filter, rumbleFilter, gainNode, lfo, lfoGain, subOsc, subGain, sub2Osc;
let eqBassLow, eqBassMid, eqBassHigh, eqTrebleLow, eqTrebleMid, eqTrebleHigh;
let playing = false;
let timerHandle = null;
let currentPreset = 'prop';

const playBtn = document.getElementById('playBtn');
const volumeSlider = document.getElementById('volume');
const rumbleSlider = document.getElementById('rumble');
const volVal = document.getElementById('volVal');
const rumbleVal = document.getElementById('rumbleVal');
const timerSelect = document.getElementById('timer');
const statusEl = document.getElementById('status');
const presetBtns = document.querySelectorAll('.preset-btn');

const eqToggle = document.getElementById('eqToggle');
const eqPanel = document.getElementById('eqPanel');
const eqResetBtn = document.getElementById('eqReset');

document.getElementById('preset-prop').checked = true

// band name -> [slider el, value el, filter frequency in Hz]
const eqBands = {
  eqBassLow:    { freq: 60,    node: null },
  eqBassMid:    { freq: 150,   node: null },
  eqBassHigh:   { freq: 300,   node: null },
  eqMidLow:     { freq: 500,   node: null },
  eqMid:        { freq: 1000,  node: null },
  eqMidHigh:    { freq: 2000,  node: null },
  eqTrebleLow:  { freq: 3500,  node: null },
  eqTrebleMid:  { freq: 6000,  node: null },
  eqTrebleHigh: { freq: 10000, node: null },
};

Object.keys(eqBands).forEach(id => {
  eqBands[id].slider = document.getElementById(id);
  eqBands[id].valEl = document.getElementById(id + 'Val');
});

const presets = {
  takeoff:  { cutoff: 900,  q: 0.7, rumbleFreq: 55,  wobble: 0.6, subFreq: 38 },
  cruise:   { cutoff: 500,  q: 0.6, rumbleFreq: 42,  wobble: 0.2, subFreq: 30 },
  prop:     { cutoff: 300,  q: 1.5, rumbleFreq: 24,  wobble: 1.6, subFreq: 20 },
};

function makeNoiseBuffer(audioCtx) {
  const bufferSize = audioCtx.sampleRate * 4;
  const buffer = audioCtx.createBuffer(1, bufferSize, audioCtx.sampleRate);
  const data = buffer.getChannelData(0);
  // Brownian-ish noise for a softer, deeper rumble than pure white noise
  let last = 0;
  for (let i = 0; i < bufferSize; i++) {
    const white = Math.random() * 2 - 1;
    last = (last + 0.02 * white) / 1.02;
    data[i] = last * 3.5;
  }
  return buffer;
}

function start() {
  if (!ctx) {
    ctx = new (window.AudioContext || window.webkitAudioContext)();
  }
  if (ctx.state === 'suspended') ctx.resume();

  noiseSource = ctx.createBufferSource();
  noiseSource.buffer = makeNoiseBuffer(ctx);
  noiseSource.loop = true;

  filter = ctx.createBiquadFilter();
  filter.type = 'lowpass';

  rumbleFilter = ctx.createBiquadFilter();
  rumbleFilter.type = 'peaking';
  rumbleFilter.gain.value = 8;
  rumbleFilter.Q.value = 1.2;

  gainNode = ctx.createGain();
  gainNode.gain.value = volumeSlider.value / 100 * 0.5;

  // subtle engine "wobble" via LFO modulating the rumble filter frequency
  lfo = ctx.createOscillator();
  lfo.type = 'sine';
  lfoGain = ctx.createGain();
  lfo.connect(lfoGain);
  lfoGain.connect(rumbleFilter.frequency);

  // real sub-bass tone — filtered noise alone can't produce felt depth on
  // most speakers, so a low sine + a quiet sub-octave layer fill in true low end
  subOsc = ctx.createOscillator();
  subOsc.type = 'sine';
  subGain = ctx.createGain();
  subGain.gain.value = 0;
  subOsc.connect(subGain);
  subGain.connect(gainNode);

  sub2Osc = ctx.createOscillator();
  sub2Osc.type = 'sine';
  sub2Osc.connect(subGain);

  // 6-band EQ: peaking filters, chained bass -> treble, gains driven by sliders
  Object.keys(eqBands).forEach(id => {
    const band = eqBands[id];
    const node = ctx.createBiquadFilter();
    node.type = 'peaking';
    node.frequency.value = band.freq;
    node.Q.value = 1.0;
    node.gain.value = parseFloat(band.slider.value);
    band.node = node;
  });

  applyPreset(currentPreset, true);

  const eqChain = ['eqBassLow', 'eqBassMid', 'eqBassHigh', 'eqMidLow', 'eqMid', 'eqMidHigh', 'eqTrebleLow', 'eqTrebleMid', 'eqTrebleHigh'];

  noiseSource.connect(filter);
  filter.connect(rumbleFilter);
  rumbleFilter.connect(eqBands[eqChain[0]].node);
  for (let i = 0; i < eqChain.length - 1; i++) {
    eqBands[eqChain[i]].node.connect(eqBands[eqChain[i + 1]].node);
  }
  eqBands[eqChain[eqChain.length - 1]].node.connect(gainNode);
  gainNode.connect(ctx.destination);

  noiseSource.start();
  lfo.start();
  subOsc.start();
  sub2Osc.start();

  playing = true;
  playBtn.textContent = '⏸';
  playBtn.classList.add('playing');
  playBtn.setAttribute('aria-label', 'Pause');
  updateStatus();
  armTimer();
}

function stop() {
  if (noiseSource) {
    try { noiseSource.stop(); } catch(e) {}
    noiseSource.disconnect();
  }
  if (lfo) { try { lfo.stop(); } catch(e) {} lfo.disconnect(); }
  if (subOsc) { try { subOsc.stop(); } catch(e) {} subOsc.disconnect(); }
  if (sub2Osc) { try { sub2Osc.stop(); } catch(e) {} sub2Osc.disconnect(); }
  if (subGain) subGain.disconnect();
  if (filter) filter.disconnect();
  if (rumbleFilter) rumbleFilter.disconnect();
  Object.keys(eqBands).forEach(id => {
    if (eqBands[id].node) { eqBands[id].node.disconnect(); eqBands[id].node = null; }
  });
  if (gainNode) gainNode.disconnect();
  playing = false;
  playBtn.textContent = '▶';
  playBtn.classList.remove('playing');
  playBtn.setAttribute('aria-label', 'Play');
  clearTimeout(timerHandle);
  statusEl.textContent = '';
}

function applyPreset(name, immediate) {
  currentPreset = name;
  const p = presets[name];
  const rumbleAmount = rumbleSlider.value / 100;
  if (!filter || !rumbleFilter || !lfoGain || !subOsc) return;
  const t = ctx.currentTime;
  const rampTime = immediate ? 0.01 : 1.2;
  filter.frequency.setTargetAtTime(p.cutoff, t, rampTime);
  filter.Q.setTargetAtTime(p.q, t, rampTime);
  rumbleFilter.frequency.setTargetAtTime(p.rumbleFreq * (0.6 + rumbleAmount), t, rampTime);
  rumbleFilter.gain.setTargetAtTime(6 + rumbleAmount * 10, t, rampTime);
  lfo.frequency.setTargetAtTime(0.15 + p.wobble * 0.1, t, rampTime);
  lfoGain.gain.setTargetAtTime(p.rumbleFreq * 0.15 * p.wobble, t, rampTime);

  // sub-bass: silent below 60% rumble, then ramps in — with an extra push
  // in the last stretch so "all the way right" actually feels deeper
  const subEngage = Math.max(0, (rumbleAmount - 0.6) / 0.4); // 0 at 60%, 1 at 100%
  const subBoost = subEngage * subEngage; // curved so the top end hits harder
  subOsc.frequency.setTargetAtTime(p.subFreq, t, rampTime);
  sub2Osc.frequency.setTargetAtTime(p.subFreq / 2, t, rampTime);
  subGain.gain.setTargetAtTime(subBoost * 0.35, t, rampTime);
}

function updateStatus() {
  const secs = parseInt(timerSelect.value, 10);
  if (secs > 0 && playing) {
    const mins = Math.round(secs / 60);
    statusEl.textContent = 'Playing — stopping in ' + (mins >= 60 ? (mins/60) + 'h' : mins + ' min');
  } else if (playing) {
    statusEl.textContent = 'Playing';
  }
}

function armTimer() {
  clearTimeout(timerHandle);
  const secs = parseInt(timerSelect.value, 10);
  if (secs > 0) {
    timerHandle = setTimeout(() => { stop(); }, secs * 1000);
  }
}

playBtn.addEventListener('click', () => {
  if (playing) stop(); else start();
});

volumeSlider.addEventListener('input', () => {
  volVal.textContent = volumeSlider.value + '%';
  if (gainNode) {
    gainNode.gain.setTargetAtTime(volumeSlider.value / 100 * 0.5, ctx.currentTime, 0.05);
  }
});

const initialRumbleValue = rumbleSlider.value
rumbleVal.textContent = initialRumbleValue < 33 ? 'Light' : initialRumbleValue < 60 ? 'Medium' : initialRumbleValue < 90 ? 'Deep' : 'Ultra deep';

rumbleSlider.addEventListener('input', () => {
  const v = rumbleSlider.value;
  rumbleVal.textContent = v < 33 ? 'Light' : v < 60 ? 'Medium' : v < 90 ? 'Deep' : 'Ultra deep';
  if (playing) applyPreset(currentPreset, false);
});

presetBtns.forEach(btn => {
  btn.addEventListener('click', () => {
    presetBtns.forEach(b => b.classList.remove('active'));
    btn.classList.add('active');
    if (playing) applyPreset(btn.dataset.preset, false);
    else currentPreset = btn.dataset.preset;
  });
});

timerSelect.addEventListener('change', () => {
  if (playing) { armTimer(); updateStatus(); }
});

eqToggle.addEventListener('click', () => {
  const open = eqPanel.classList.toggle('open');
  eqToggle.classList.toggle('open', open);
});

Object.keys(eqBands).forEach(id => {
  const band = eqBands[id];
  band.slider.addEventListener('input', () => {
    const v = parseFloat(band.slider.value);
    band.valEl.textContent = (v > 0 ? '+' : '') + v + 'dB';
    if (band.node && ctx) {
      band.node.gain.setTargetAtTime(v, ctx.currentTime, 0.05);
    }
  });
});

eqResetBtn.addEventListener('click', () => {
  Object.keys(eqBands).forEach(id => {
    const band = eqBands[id];
    band.slider.value = 0;
    band.valEl.textContent = '0dB';
    if (band.node && ctx) {
      band.node.gain.setTargetAtTime(0, ctx.currentTime, 0.05);
    }
  });
});

