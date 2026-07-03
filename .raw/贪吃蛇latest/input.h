#pragma once
#include <raylib.h>

// Edge-triggered: true only on the frame the key transitions from up to down
bool is_key_pressed(int key);

// Level-triggered: true every frame the key is held down
bool is_key_held(int key);
