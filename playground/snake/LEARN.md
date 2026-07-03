---
type: learn
project: snake
created: 2026-07-03
tags:
  - learn
  - snake
  - cpp
  - raylib
  - game
status: complete
next: "[[../tetris/LEARN|Tetris]]"
wiki:
  - "[[Scene State Machine Pattern]]"
  - "[[BFS Pathfinding for Game AI]]"
  - "[[Procedural Audio for Games]]"
  - "[[Edge-Triggered Input]]"
  - "[[Cpp-Tetris-Architecture-Learning]]"
---

# Snake Game — Learning Guide

> **Open this file in your IDE, not Obsidian.**
> Keep `LEARN.md` on the left panel, `src/` on the right.
> Read → trace → run → modify → understand.

## Quick Start

```bash
# Build
make

# Run
make run

# Clean
make clean
```

---

## Architecture Overview

```
main.cpp
  │
  └── Scene (abstract base class)
        ├── MenuScene     — title screen + mode/difficulty selection
        └── PlayScene     — the actual game
              │
              ├── GameState   — all game data (snake, food, map, scores)
              ├── input.cpp   — edge-triggered keyboard handling
              ├── render.cpp  — raylib drawing
              ├── ai.cpp      — BFS pathfinding (3 difficulty tiers)
              └── audio.cpp   — procedural sine-wave sound effects
```

**Key Pattern**: Scene polymorphism. `MenuScene` and `PlayScene` both inherit from `Scene`. The `main.cpp` loop calls `update()` and `render()` without knowing which scene is active. This is the **State Pattern** at the application level.

**Contrast**: In Tetris, the same job is done with a `Game` class that orchestrates everything — no Scene abstraction. See [[Cpp-Tetris-Architecture-Learning]] for the comparison.

---

## File-by-File Guide

### 1. `game.h` — Data Structures (Start Here)

**Why first**: Everything depends on this. If you don't understand `GameState`, you can't understand anything else.

```cpp
struct Point { int x, y; };
enum Direction { UP, DOWN, LEFT, RIGHT };       // char values: 'W','S','A','D'
enum GameMode { MODE_CLASSIC, MODE_FREE, MODE_AI };
enum AIDifficulty { AI_EASY, AI_NORMAL, AI_HARD };

struct GameState {
    deque<Point> snake1, snake2;   // snake body (front=head)
    Point food, food2;
    int scores[2];
    bool paused, game_over;
    GameMode mode;
    // ... timers, drop mechanics
};
```

**Self-explanation task**: Write down why the snake body is a `deque<Point>` and not a `vector<Point>`. (Hint: what operations happen at BOTH ends?)

<details>
<summary>Answer (check after your attempt)</summary>

The snake grows at the head (push_front) and shrinks at the tail (pop_back) every frame. `deque` supports O(1) push/pop at BOTH ends. `vector` only supports O(1) at the back (push_back), but pop_front would be O(n).
</details>

### 2. `scene.h` + `scene.cpp` — Scene State Machine

Read these next. They define:
- `Scene` abstract base class with `update()`, `render()`, `next_scene()`
- `MenuScene`: handles title screen, mode selection, difficulty selection
- `PlayScene`: owns the `GameState`, orchestrates the game loop

**Key question**: When `PlayScene::update()` returns and `next_scene()` says `SCENE_MENU`, what happens in `main.cpp`?

<details>
<summary>Trace it</summary>

`main.cpp` checks `current_scene->next_scene()` after each frame. If it changed, `current_scene` is deleted and a new scene is created. This is a clean state transition — the old scene is destroyed, no state leaks.
</details>

### 3. `input.cpp` + `input.h` — Edge-Triggered Input

**Critical concept**: Edge-triggered vs level-triggered.

- **Level-triggered**: "Is W pressed right now?" → True for EVERY frame while held → snake moves 60 times per second.
- **Edge-triggered**: "Was W NOT pressed last frame, but IS pressed this frame?" → True only ONCE per press.

This is why your snake doesn't fly off the screen when you hold a key. Read `input.cpp` and find where the "last frame" state is stored.

### 4. `game.cpp` — The Core Loop (~800 lines)

This is the big one. Don't try to read it all at once. Instead:

1. **First pass**: Find the 4 main functions (`update`, `render`, `apply_snake_move`, collision helpers)
2. **Second pass**: Trace `apply_snake_move()` for a SINGLE frame with these inputs:
   - Snake at [(10,10), (10,9), (10,8)], direction RIGHT, no food at head
   - Write down what `deque` looks like after the move
3. **Third pass**: Trace what happens when food IS at the head position
4. **Fourth pass**: Add the `pending_growth` counter — how does it delay growth by one frame?

**The 11-step collision check**: Find the collision function. Draw the priority order on paper:
```
1. Wall collision?    → game over
2. Self collision?    → game over
3. Other snake?       → game over
4. Food?              → eat + grow
5. Drop item?         → eat
... (find the rest)
```

### 5. `ai.cpp` — BFS Pathfinding

Three difficulty tiers, all based on Breadth-First Search:

| Tier | Strategy | When it fails |
|------|----------|---------------|
| **Easy** | Random valid move | Always wanders |
| **Normal** | BFS to food | Traps itself in dead ends |
| **Hard** | BFS to food + tail-reachability check | Perfect play (almost unbeatable) |

**Self-explanation task**: Why does Normal AI sometimes die? Draw a scenario where BFS finds a path to food, but the snake's body blocks the escape route after eating.

### 6. `render.cpp` — Drawing

Read to understand raylib's drawing API:
- `DrawRectangle()`, `DrawText()`
- Custom font rendering with TTF
- Color scheme for different game elements

### 7. `audio.cpp` — Procedural Sound

**Key concept**: No audio files. Sound is generated at runtime using sine wave synthesis.

```cpp
// Simplified: generate a tone
for (int i = 0; i < sample_count; i++) {
    float t = (float)i / sample_rate;
    sample[i] = sin(2.0f * PI * frequency * t) * volume * fade_out(t);
}
```

Four sound effects: eat food, die, menu select, game start. Each has a different frequency, duration, and envelope.

---

## Hands-On Exercises

Do these in order. Each teaches a specific concept.

### Exercise 1: Change Snake Speed
- **Find**: The timer constant that controls snake speed
- **Modify**: Make it 2x faster
- **Learn**: Game loop timing, how `update()` frequency differs from `render()` frequency

### Exercise 2: Add a New Food Type
- **Goal**: Add a "golden food" that gives 3x points and appears randomly
- **Files to modify**: `game.h` (data), `game.cpp` (logic), `render.cpp` (visual)
- **Learn**: How to add a feature across the data→logic→render pipeline

### Exercise 3: Make AI Use A* Instead of BFS
- **Goal**: Replace BFS with A* pathfinding using Manhattan distance heuristic
- **Learn**: Difference between uninformed and informed search; heuristic design

### Exercise 4: Add a Pause Menu Scene
- **Goal**: Create `PauseScene` between Play and Menu
- **Files to modify**: `scene.h` (new class), `scene.cpp` (implementation), `main.cpp` (or input routing)
- **Learn**: Extending the Scene state machine pattern

---

## When Stuck

1. **20-minute rule**: If you've been staring at the same function for 20 minutes without being able to explain it, zoom IN. One loop. One `if` statement. Explain just that.
2. **Add print statements**: Add `printf("snake head at (%d,%d)\n", x, y)` in the update loop. Watch the output as you play.
3. **Comment out a line**: What breaks? The error tells you what that line was doing.
4. **Read the wiki concept**: Each file maps to a [[wiki/concepts/|wiki concept page]] for deeper theory.
5. **Compare with Tetris**: Open `../tetris/LEARN.md` and find the equivalent concept. Different architecture, same problem — why?

---

## After You've Finished

1. Write one TIL entry: "The most surprising thing I learned from Snake's architecture"
2. File a synthesis note in the wiki (only if it connects to other projects)
3. Move to [[../tetris/LEARN|Tetris]]
