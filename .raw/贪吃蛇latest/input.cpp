#include "input.h"

bool is_key_pressed(int key)
{
    return IsKeyPressed(key);
}

bool is_key_held(int key)
{
    return IsKeyDown(key);
}
