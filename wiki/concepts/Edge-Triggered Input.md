---
type: concept
title: "Edge-Triggered Input"
created: 2026-06-23
updated: 2026-06-29
tags:
  - concept
  - game-dev
  - input
  - cpp
status: current
related:
  - "[[snake-game-raylib]]"
  - "[[Scene State Machine Pattern]]"
---

# Edge-Triggered Input

> [!summary] 逐层摘要
> **Layer 1 (初次理解)**: 
> **Layer 2 (加粗关键)**: 
> **Layer 3 (高亮核心)**: 
> **Layer 4 (2-3 句自写总结)**: 

---

## The Problem

Naive key polling reads the key state every frame:

```cpp
// BAD: level-triggered for everything
if (IsKeyDown(KEY_W)) move_up();  // fires 60 times/second!
```

With level-triggered input, holding W for 0.2 seconds (12 frames at 60 FPS) triggers 12 moves — the snake teleports off-screen. The player meant "go up once."

## The Solution: Two Detection Modes

```cpp
// Edge-triggered: true only on the frame the key transitions from up→down
bool is_key_pressed(int key);

// Level-triggered: true every frame the key is held down
bool is_key_held(int key);
```

## Implementation (Snake Game)

The snake game's `input.cpp` wraps raylib's built-in functions:

```cpp
bool is_key_pressed(int key) {
    return IsKeyPressed(key);   // raylib: internal edge detection
}

bool is_key_held(int key) {
    return IsKeyDown(key);      // raylib: internal key state
}
```

raylib handles the edge detection internally by tracking the previous frame's key state — the wrapper exists to keep all input logic behind a consistent project interface.

## Usage in the Snake Game

### Edge-triggered (is_key_pressed)

Used for **discrete actions** that should happen exactly once per press:

| Key | Action | Why Edge |
|-----|--------|----------|
| W/A/S/D | Change direction | One direction change per keypress |
| 1/2/3 | Menu selection | One mode select per keypress |
| Space | Toggle pause | One toggle per keypress |
| R | Restart game | One restart per keypress |
| ESC | Return to menu | One transition per keypress |

```cpp
// Direction change: edge-triggered to prevent multi-turn per frame
if (is_key_pressed(KEY_W)) set_direction(&_game, UP);

// Menu selection: edge-triggered to prevent multi-trigger
if (is_key_pressed(KEY_ONE)) {
    selected_mode = MODE_CLASSIC;
    _next = SCENE_PLAY;
}
```

### Level-triggered (is_key_held)

Used for **continuous actions** where holding is intentional:

| Key | Action | Why Level |
|-----|--------|-----------|
| Shift / F | Speed up movement | Hold to accelerate gameplay |

```cpp
// Speed-up: level-triggered — hold to sustain acceleration
if (is_key_held(KEY_LEFT_SHIFT) || is_key_held(KEY_F))
    interval = (MOVE_INTERVAL_MS / 4) / 1000.0;  // 4x speed
```

## Turn Locking: Edge Detection Alone Isn't Enough

Edge detection prevents 60×/second triggering, but doesn't prevent **multiple direction changes within a single game tick**. Consider:

```
Frame 1: is_key_pressed(W) → direction = UP     [ok]
Frame 2: is_key_pressed(D) → direction = RIGHT   [ok, different frame]
Frame 3: is_key_pressed(S) → direction = DOWN    [180° reversal! snake eats itself]
```

The snake is 2 cells long. On frame 1 it goes UP (head at row 5→4). Frame 2 it goes RIGHT. Frame 3 it goes DOWN — but the body is still at row 4, col 6 (from the RIGHT move). This is a legal turn, but without additional protection, rapid keypresses can still cause 180° reversals.

The solution: **turn locking** per tick.

```cpp
// game.h
bool turn_locked;   // reset each tick, set on first direction change

// scene.cpp update()
if (is_key_pressed(KEY_W)) set_direction(&_game, UP);  // sets turn_locked

// game.cpp
void set_direction(GameState *game, Direction dir) {
    if (game->turn_locked) return;  // already turned this tick
    if (is_opposite(dir, game->direction)) return;  // no 180° reversal
    game->direction = dir;
    game->turn_locked = true;
}

// move_snake() — reset at end of tick
game->turn_locked = false;
```

## When to Use Each Mode

| Situation | Use | Reason |
|-----------|-----|--------|
| Menu navigation | Edge | One selection per press |
| Direction changes | Edge | One turn per press |
| Fire/attack | Edge | One action per press (or level for auto-fire) |
| Pause toggle | Edge | One toggle per press |
| Movement (continuous) | Level | Holding = sustained movement |
| Speed modifier | Level | Holding = sustained speed change |
| Crouch/sneak | Level | Holding = sustained state |

## Edge Cases

### Key repeat (OS-level)

Most operating systems generate repeated key-down events when a key is held. Good input libraries (like raylib) filter these out — `IsKeyPressed()` returns true only on the first frame after the physical transition. If implementing from scratch, you must track the previous frame's state:

```cpp
// Manual edge detection (if not using a library that provides it)
bool is_key_pressed_manual(int key) {
    bool current = is_key_down_raw(key);
    bool was_down = previous_frame_state[key];
    previous_frame_state[key] = current;
    return current && !was_down;  // rising edge
}
```

### Scene transitions

When switching scenes, pending edge triggers from the transition frame can cause unintended actions. The snake game handles this by creating the new scene *after* checking `next_scene()` — the new scene's input state starts clean on the next frame.

### Window focus loss

If the player alt-tabs away while holding a key, the key-up event is lost. Most libraries handle this by clearing all key states on focus loss. Verify your library's behavior if it matters for your game.
