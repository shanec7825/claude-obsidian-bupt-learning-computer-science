#include "audio.h"
#include <raylib.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

// Audio parameters
#define SAMPLE_RATE 22050
#define SAMPLE_BITS 16
#define CHANNELS    1

// Sound handles
static Sound snd_eat;
static Sound snd_death;
static Sound snd_start;
static Sound snd_move;
static bool audio_ready = false;

// Generate a short sine-wave tone and return as Wave
static Wave gen_tone(float freq, float duration_sec, float volume)
{
    int frame_count = (int)(SAMPLE_RATE * duration_sec);
    Wave wave = {0};
    wave.sampleRate   = SAMPLE_RATE;
    wave.sampleSize   = SAMPLE_BITS;
    wave.channels     = CHANNELS;
    wave.frameCount   = frame_count;

    int data_size = frame_count * CHANNELS * (SAMPLE_BITS / 8);
    short *data = (short *)malloc(data_size);

    for (int i = 0; i < frame_count; i++)
    {
        float t = (float)i / SAMPLE_RATE;
        float envelope = 1.0f - (float)i / frame_count;  // linear fade-out
        float val = sinf(2.0f * 3.1415926f * freq * t) * envelope * volume;
        data[i] = (short)(val * 32767.0f);
    }

    wave.data = data;
    return wave;
}

// Generate a two-note "eat" sound (quick ascending blip)
static Wave gen_eat_sound()
{
    int frame_count = (int)(SAMPLE_RATE * 0.15f);
    int data_size = frame_count * CHANNELS * (SAMPLE_BITS / 8);
    short *data = (short *)malloc(data_size);

    for (int i = 0; i < frame_count; i++)
    {
        float t = (float)i / SAMPLE_RATE;
        float dur = 0.15f;
        float freq;
        if (t < dur * 0.5f)
            freq = 440.0f + (t / (dur * 0.5f)) * 440.0f;  // 440 -> 880 Hz
        else
            freq = 880.0f;

        float envelope = 1.0f - t / dur;  // decay
        float val = sinf(2.0f * 3.1415926f * freq * t) * envelope * 0.4f;
        data[i] = (short)(val * 32767.0f);
    }

    Wave wave = {0};
    wave.sampleRate = SAMPLE_RATE;
    wave.sampleSize = SAMPLE_BITS;
    wave.channels   = CHANNELS;
    wave.frameCount = frame_count;
    wave.data       = data;
    return wave;
}

// Generate death sound: descending tone
static Wave gen_death_sound()
{
    float dur = 0.5f;
    int frame_count = (int)(SAMPLE_RATE * dur);
    int data_size = frame_count * CHANNELS * (SAMPLE_BITS / 8);
    short *data = (short *)malloc(data_size);

    for (int i = 0; i < frame_count; i++)
    {
        float t = (float)i / SAMPLE_RATE;
        float freq = 400.0f * (1.0f - t / dur * 0.7f);  // descend from 400 to 120 Hz
        float envelope = 1.0f - t / dur;
        float val = sinf(2.0f * 3.1415926f * freq * t) * envelope * 0.5f;
        data[i] = (short)(val * 32767.0f);
    }

    Wave wave = {0};
    wave.sampleRate = SAMPLE_RATE;
    wave.sampleSize = SAMPLE_BITS;
    wave.channels   = CHANNELS;
    wave.frameCount = frame_count;
    wave.data       = data;
    return wave;
}

// Generate start sound: a quick ascending arpeggio
static Wave gen_start_sound()
{
    float dur = 0.3f;
    int frame_count = (int)(SAMPLE_RATE * dur);
    int data_size = frame_count * CHANNELS * (SAMPLE_BITS / 8);
    short *data = (short *)malloc(data_size);

    float notes[] = {523.0f, 659.0f, 784.0f};  // C5, E5, G5
    for (int i = 0; i < frame_count; i++)
    {
        float t = (float)i / SAMPLE_RATE;
        int note_idx = t < 0.1f ? 0 : (t < 0.2f ? 1 : 2);
        float freq = notes[note_idx];
        float note_t = t - note_idx * 0.1f;
        float envelope = 1.0f - note_t / 0.12f;
        if (envelope < 0.0f) envelope = 0.0f;
        float val = sinf(2.0f * 3.1415926f * freq * t) * envelope * 0.35f;
        data[i] = (short)(val * 32767.0f);
    }

    Wave wave = {0};
    wave.sampleRate = SAMPLE_RATE;
    wave.sampleSize = SAMPLE_BITS;
    wave.channels   = CHANNELS;
    wave.frameCount = frame_count;
    wave.data       = data;
    return wave;
}

// Generate a very short tick for movement
static Wave gen_move_sound()
{
    float dur = 0.03f;
    int frame_count = (int)(SAMPLE_RATE * dur);
    int data_size = frame_count * CHANNELS * (SAMPLE_BITS / 8);
    short *data = (short *)malloc(data_size);

    for (int i = 0; i < frame_count; i++)
    {
        float t = (float)i / SAMPLE_RATE;
        float envelope = 1.0f - t / dur;
        float val = sinf(2.0f * 3.1415926f * 120.0f * t) * envelope * 0.15f;
        data[i] = (short)(val * 32767.0f);
    }

    Wave wave = {0};
    wave.sampleRate = SAMPLE_RATE;
    wave.sampleSize = SAMPLE_BITS;
    wave.channels   = CHANNELS;
    wave.frameCount = frame_count;
    wave.data       = data;
    return wave;
}

void init_audio()
{
    if (audio_ready) return;

    InitAudioDevice();

    Wave w_eat   = gen_eat_sound();
    Wave w_death = gen_death_sound();
    Wave w_start = gen_start_sound();
    Wave w_move  = gen_move_sound();

    snd_eat   = LoadSoundFromWave(w_eat);
    snd_death = LoadSoundFromWave(w_death);
    snd_start = LoadSoundFromWave(w_start);
    snd_move  = LoadSoundFromWave(w_move);

    // Free wave data after loading to Sound
    free(w_eat.data);
    free(w_death.data);
    free(w_start.data);
    free(w_move.data);

    audio_ready = true;
}

void unload_audio()
{
    if (!audio_ready) return;
    UnloadSound(snd_eat);
    UnloadSound(snd_death);
    UnloadSound(snd_start);
    UnloadSound(snd_move);
    CloseAudioDevice();
    audio_ready = false;
}

void audio_play_eat()
{
    if (audio_ready) PlaySound(snd_eat);
}

void audio_play_death()
{
    if (audio_ready) PlaySound(snd_death);
}

void audio_play_start()
{
    if (audio_ready) PlaySound(snd_start);
}

void audio_play_move()
{
    if (audio_ready) PlaySound(snd_move);
}
