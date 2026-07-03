---
type: concept
title: "Procedural Audio for Games"
created: 2026-06-23
updated: 2026-06-29
tags:
  - concept
  - audio
  - game-dev
  - cpp
  - raylib
  - procedural-generation
status: current
related:
  - "[[snake-game-raylib]]"
  - "[[Scene State Machine Pattern]]"
---

# Procedural Audio for Games

> [!summary] 逐层摘要
> **Layer 1 (初次理解)**: 
> **Layer 2 (加粗关键)**: 
> **Layer 3 (高亮核心)**: 
> **Layer 4 (2-3 句自写总结)**: 

---

## The Pattern

Instead of loading `.wav`/`.mp3` files, the game synthesizes sounds programmatically:

```
mathematical formula → sample buffer → audio API → speaker
```

This approach is ideal for:
- **Small games** that don't want asset management overhead
- **Retro-style games** where simple waveforms (sine, square, triangle) fit the aesthetic
- **Prototypes** where sounds will change frequently
- **Educational projects** that want zero external dependencies

## Implementation (Snake Game Audio)

The snake game's audio system (`audio.cpp`) generates 4 sound effects using **sine wave synthesis with linear fade-out envelopes**.

### Architecture

```
gen_tone(freq, duration, volume)     ← general-purpose sine wave generator
  ├── gen_eat_sound()                ← ascending blip (440→880 Hz, 0.15s)
  ├── gen_death_sound()              ← descending tone (400→120 Hz, 0.5s)
  ├── gen_start_sound()              ← C5-E5-G5 arpeggio (0.3s)
  └── gen_move_sound()               ← 120Hz tick (0.03s)

init_audio()                         ← generates all Wave objects → loads to Sound
  ├── audio_play_eat()
  ├── audio_play_death()
  ├── audio_play_start()
  └── audio_play_move()
```

### Core Waveform Generator

```cpp
static Wave gen_tone(float freq, float duration_sec, float volume)
{
    int frame_count = (int)(SAMPLE_RATE * duration_sec);
    short *data = (short *)malloc(frame_count * sizeof(short));

    for (int i = 0; i < frame_count; i++) {
        float t = (float)i / SAMPLE_RATE;
        float envelope = 1.0f - (float)i / frame_count;  // linear decay
        float val = sinf(2.0f * PI * freq * t) * envelope * volume;
        data[i] = (short)(val * 32767.0f);
    }
    // ... wrap in raylib Wave struct
}
```

### Sound Design Details

| Sound | Technique | Duration | Character |
|-------|-----------|----------|-----------|
| **Eat** | Frequency sweep 440→880 Hz, split at midpoint | 0.15s | Quick ascending "blip" |
| **Death** | Frequency sweep 400→120 Hz (descending) | 0.5s | Sad descending tone |
| **Start** | Three-note arpeggio: C5(523), E5(659), G5(784) | 0.3s | Triumphant ascending triad |
| **Move** | Single 120 Hz low sine | 0.03s | Barely-audible tick |

### Envelope Design

All sounds use a **linear fade-out envelope**:

```
envelope(t) = 1.0 - t/duration
```

This prevents audible clicks at sound boundaries (the "pop" that occurs when a waveform stops at non-zero amplitude). Professional audio uses more sophisticated envelopes (ADSR), but linear fade-out is sufficient for simple game SFX.

## Key Design Decisions

### C malloc (not C++ new)

The sample buffers use `malloc()`/`free()` because raylib's `Wave` struct expects raw memory. The `Wave` is loaded into a `Sound` via `LoadSoundFromWave()`, then the `Wave.data` is immediately freed — no memory leak.

### Static Sound Handles

```cpp
static Sound snd_eat;
static Sound snd_death;
static Sound snd_start;
static Sound snd_move;
static bool audio_ready = false;
```

All sounds are loaded once at startup (`init_audio()`) and persist for the program lifetime. The `audio_ready` flag prevents double-initialization and guards against playing sounds before the audio device is ready.

### Sample Rate: 22050 Hz

Half the CD-quality rate. Adequate for simple sine-wave SFX — the sounds are intentionally lo-fi. Reduces memory: each sound's buffer is `duration × 22050 × 2 bytes`.

## raylib Audio API Used

| Function | Purpose |
|----------|---------|
| `InitAudioDevice()` | Initialize audio subsystem |
| `LoadSoundFromWave(Wave)` | Convert raw samples to GPU-ready Sound |
| `PlaySound(Sound)` | Play one-shot (non-blocking) |
| `UnloadSound(Sound)` | Free GPU audio resource |
| `CloseAudioDevice()` | Shutdown audio subsystem |

## Contrast with Asset-Based Audio

| Dimension | Procedural | Asset-Based |
|-----------|-----------|-------------|
| File size | 0 bytes (generated) | KB-MB per sound |
| Flexibility | Infinite variation possible | Fixed by recording |
| Quality ceiling | Simple waveforms only | Any recorded sound |
| Iteration speed | Recompile to change | Swap file, reload |
| CPU cost | Negligible (short sounds) | None (just decode) |
| Dependency | Math library only | Audio codec support |

## Extensions

The pattern can be extended to:
- **ADSR envelopes** — attack/decay/sustain/release for more natural sound shaping
- **Multi-waveform** — square, triangle, sawtooth, noise for varied timbres
- **Frequency modulation** — vibrato, pitch bends
- **Polyphony** — multiple simultaneous tones (chords, layered effects)
- **Randomization** — slight pitch/volume variation per play for organic feel
