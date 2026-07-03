#pragma once

// Initialize all game sounds (call once at startup)
void init_audio();

// Clean up audio resources
void unload_audio();

// Sound effects
void audio_play_eat();
void audio_play_death();
void audio_play_start();
void audio_play_move();
