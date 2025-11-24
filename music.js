(function () {
  // Build a DOM audio element for a short effect clip.
  function createSound(src, volume = 1) {
    const audio = new Audio(src);
    audio.preload = 'auto';
    audio.volume = volume;
    return { src, volume, audio };
  }

  const soundEffects = {
    jump: createSound('audio/jump.wav', 0.3),
    hurt: createSound('audio/hurt.wav', 0.38),
    enemy: createSound('audio/enemy_defeat.wav', 0.34),
    coin: createSound('audio/coin.wav', 0.3)
  };

  const music = createChiptuneMusic();

  // Generate a simple looping chiptune using Web Audio nodes.
  function createChiptuneMusic() {
    const AudioContextClass = window.AudioContext || window.webkitAudioContext;
    if (!AudioContextClass) {
      return { start() {}, stop() {} };
    }
    const ctx = new AudioContextClass();
    const master = ctx.createGain();
    master.gain.value = 0.32;
    master.connect(ctx.destination);

    const tempo = 132;
    const stepDur = 60 / tempo / 2;
    const melodyPattern = [
      'C5', null, 'E5', null, 'G5', null, 'E5', null,
      'C5', null, 'E5', null, 'A5', null, 'G5', null,
      'F5', null, 'A5', null, 'C6', null, 'A5', null,
      'G5', null, 'E5', null, 'D5', null, 'C5', null
    ];
    const harmonyPattern = [
      'G4', null, 'B4', null, 'D5', null, 'B4', null,
      'G4', null, 'C5', null, 'E5', null, 'D5', null,
      'A4', null, 'C5', null, 'E5', null, 'C5', null,
      'B4', null, 'G4', null, 'F4', null, 'E4', null
    ];
    const bassPattern = [
      'C3', null, 'C3', null, 'G2', null, 'C3', null,
      'F2', null, 'F2', null, 'C3', null, 'G2', null,
      'A2', null, 'A2', null, 'E2', null, 'B2', null,
      'G2', null, 'G2', null, 'D2', null, 'G2', null
    ];
    const noisePattern = [1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1];

    const noteIndex = { C: 0, 'C#': 1, Db: 1, D: 2, 'D#': 3, Eb: 3, E: 4, F: 5, 'F#': 6, Gb: 6, G: 7, 'G#': 8, Ab: 8, A: 9, 'A#': 10, Bb: 10, B: 11 };
    const freqCache = new Map();
    // Convert note names (e.g. C5) into frequencies with caching.
    function noteToFreq(note) {
      if (freqCache.has(note)) return freqCache.get(note);
      const match = /^([A-G][b#]?)(\d)$/.exec(note);
      if (!match) return 0;
      const [, name, octaveStr] = match;
      const octave = parseInt(octaveStr, 10);
      const semitone = noteIndex[name] ?? 0;
      const distance = semitone - 9 + (octave - 4) * 12;
      const freq = 440 * Math.pow(2, distance / 12);
      freqCache.set(note, freq);
      return freq;
    }

    // Queue a single pitched oscillator tone.
    function scheduleTone(note, time, duration, type, volume) {
      const freq = noteToFreq(note);
      if (!freq) return;
      const osc = ctx.createOscillator();
      osc.type = type;
      osc.frequency.setValueAtTime(freq, time);
      const gain = ctx.createGain();
      gain.gain.setValueAtTime(0, time);
      gain.gain.linearRampToValueAtTime(volume, time + 0.01);
      gain.gain.exponentialRampToValueAtTime(0.0001, time + duration);
      osc.connect(gain).connect(master);
      osc.start(time);
      osc.stop(time + duration + 0.02);
    }

    // Queue a short burst of filtered noise for percussion.
    function scheduleNoise(time, duration, volume) {
      const length = Math.max(1, Math.floor(duration * ctx.sampleRate));
      const buffer = ctx.createBuffer(1, length, ctx.sampleRate);
      const data = buffer.getChannelData(0);
      for (let i = 0; i < length; i++) {
        data[i] = Math.random() * 2 - 1;
      }
      const src = ctx.createBufferSource();
      src.buffer = buffer;
      src.loop = false;
      const gain = ctx.createGain();
      gain.gain.setValueAtTime(volume, time);
      gain.gain.exponentialRampToValueAtTime(0.0001, time + duration);
      src.connect(gain).connect(master);
      src.start(time);
      src.stop(time + duration + 0.02);
    }

    let stepIndex = 0;
    let nextTime = 0;
    let timer = null;
    let playing = false;
    const lookahead = 0.75;

    // Emit one step of melody/harmony/bass/noise at the given time.
    function scheduleStep(time) {
      const idx = stepIndex % melodyPattern.length;
      const melodyNote = melodyPattern[idx];
      const harmonyNote = harmonyPattern[idx];
      const bassNote = bassPattern[idx % bassPattern.length];

      if (melodyNote) scheduleTone(melodyNote, time, stepDur * 1.9, 'square', 0.12);
      if (harmonyNote) scheduleTone(harmonyNote, time, stepDur * 1.4, 'square', 0.08);
      if (bassNote) scheduleTone(bassNote, time, stepDur * 1.9, 'triangle', 0.16);
      if (noisePattern[idx % noisePattern.length]) {
        scheduleNoise(time, stepDur * 0.7, 0.08);
      }
    }

    // Drive scheduling ahead of audio playback time.
    function scheduler() {
      if (!playing) return;
      while (nextTime < ctx.currentTime + lookahead) {
        scheduleStep(nextTime);
        nextTime += stepDur;
        stepIndex += 1;
      }
      timer = setTimeout(scheduler, 120);
    }

    // Begin playback loop if not already running.
    function start() {
      if (playing) return;
      ctx.resume();
      playing = true;
      stepIndex = 0;
      nextTime = ctx.currentTime + 0.05;
      scheduler();
    }

    // Halt playback and clear pending timers.
    function stop() {
      playing = false;
      if (timer) {
        clearTimeout(timer);
        timer = null;
      }
    }

    return { start, stop };
  }

  let audioUnlocked = false;

  // Prime audio and start music on first user gesture.
  function unlockAudio() {
    if (audioUnlocked) return;
    audioUnlocked = true;
    Object.values(soundEffects).forEach((sound) => {
      const base = sound.audio;
      base.muted = true;
      base.currentTime = 0;
      const playPromise = base.play();
      if (playPromise && typeof playPromise.then === 'function') {
        playPromise.then(() => {
          base.pause();
          base.currentTime = 0;
          base.muted = false;
        }).catch(() => {});
      } else {
        base.pause();
        base.currentTime = 0;
        base.muted = false;
      }
    });
    music.start();
    window.removeEventListener('pointerdown', unlockAudio);
    window.removeEventListener('keydown', unlockAudio);
  }

  function playSound(key) {
    const sound = soundEffects[key];
    if (!sound) return;
    const instance = sound.audio.cloneNode();
    instance.volume = sound.volume;
    const playPromise = instance.play();
    if (playPromise && typeof playPromise.catch === 'function') {
      playPromise.catch(() => {});
    }
  }

  // Install unlock listeners for gesture-gated playback.
  function initAudio() {
    window.addEventListener('pointerdown', unlockAudio);
    window.addEventListener('keydown', unlockAudio);
  }

  window.AudioManager = {
    initAudio,
    playSound,
    unlockAudio,
    music
  };
})();
