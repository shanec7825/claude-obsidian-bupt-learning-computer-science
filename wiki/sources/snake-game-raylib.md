---
type: source
title: "C++ Snake Game (raylib Port)"
created: 2026-06-23
updated: 2026-06-23
tags:
  - source
  - cpp
  - game
  - raylib
status: current
related:
  - "[[snake-game-easyx]]"
  - "[[BFS Pathfinding for Game AI]]"
  - "[[Procedural Audio for Games]]"
  - "[[Scene State Machine Pattern]]"
  - "[[EasyX Graphics Library]]"
raw_file: ".raw/*.cpp, .raw/*.h (13 files)"
---

# Source: C++ Snake Game (raylib Port)

**Type**: Source code (7 `.cpp` + 6 `.h` files)
**Date**: 2026-06-23
**Files**: `audio.cpp`, `audio.h`, `ai.cpp`, `ai.h`, `game.cpp`, `game.h`, `input.cpp`, `input.h`, `main.cpp`, `render.cpp`, `render.h`, `scene.cpp`, `scene.h`

## Summary

A **complete rewrite** of the EasyX Snake Game using the **raylib** graphics library. This is a significant architectural upgrade: the game was ported from Windows-only EasyX/GDI to cross-platform raylib, gaining proper header files, a procedural audio system, a polymorphic C++ scene architecture, and edge-triggered input handling.

> [!key-insight] This is a raylib port, not the original EasyX source
> The original EasyX version (ingested 2026-06-22, see [[snake-game-easyx]]) used Windows-only GDI. This version replaces EasyX with raylib throughout, adds an audio layer, and introduces proper C++ headers with complete type definitions.

## What Changed from the EasyX Version

| Dimension          | EasyX Original                   | raylib Port                             |
| ------------------ | -------------------------------- | --------------------------------------- |
| **Graphics**       | `initgraph()` / GDI              | `InitWindow()` / OpenGL                 |
| **Audio**          | None                             | Procedural waveform synthesis (4 SFX)   |
| **Headers**        | None (extensionless files)       | 6 `.h` files with full type definitions |
| **Scene system**   | Ad-hoc state enum                | Polymorphic C++ class hierarchy         |
| **Input**          | `GetAsyncKeyState` (Win32)       | raylib `IsKeyPressed` / `IsKeyDown`     |
| **Rendering**      | `BeginBatchDraw()` double buffer | `BeginDrawing()` / `EndDrawing()`       |
| **Fonts**          | EasyX LOGBRUSH text              | raylib `LoadFontEx()` with TTF          |
| **Window resize**  | `closegraph()` + `initgraph()`   | `SetWindowSize()` in-place              |
| **Cross-platform** | Windows only                     | Windows, Linux, macOS (raylib)          |

## Architecture

### File Dependency Graph

```
main.cpp
  ├── audio.h → raylib.h
  ├── input.h → raylib.h
  ├── scene.h → game.h → raylib.h
  └── render.h → game.h
```

### Scene State Machine

Uses C++ polymorphism with a virtual base class:

```
class Scene (abstract)
  ├── MenuScene   → handles mode/difficulty selection
  └── PlayScene   → game loop, input dispatch, timed movement
```

Scene transitions in `main.cpp`:
```
MenuScene → (1/2/3 key) → PlayScene → (ESC) → MenuScene
              ↓ ESC                       ↓ R key: restart
          SCENE_EXIT
```

### Game State

The `GameState` struct (defined in `game.h`) is the single source of truth:
- **Map**: `char map[MAX_MAP_SIZE][MAX_MAP_SIZE]` grid
- **Player snake**: `Point snake[]` array, head at index 0
- **AI/P2 snake**: `Point ai_snake[]` array (reused for PvP P2 and Battle AI)
- **Turn locking**: `turn_locked` / `p2_turn_locked` prevent double-turns per tick
- **Growth**: `pending_growth` credits (not instant tail extension)
- **High score**: persisted to file in classic mode

### Movement System

- **Tick interval**: 160ms standard, 40ms (4×) with Shift/F held
- **Speed-up via `is_key_held()`**: level-triggered, not edge-triggered
- **Turn locking**: `turn_locked` flag resets each tick — one direction change per move
- **Growth**: `pending_growth` decouples eating from instant growth; applied one segment per tick

### Collision Resolution Order

1. **Head-on** (both heads same cell → both die)
2. **Head vs body** (tail-passing: safe if opponent tail is about to vacate)
3. **Wall/obstacle**
4. **Self-collision** (own tail safe check)
5. Kill rewards: survivor grows by `dead_snake.length - 1` segments

## Audio System

See [[Procedural Audio for Games]] for the full concept page.

Key design:
- **4 SFX**: eat (ascending blip), death (descending tone), start (C5-E5-G5 arpeggio), move (120Hz tick)
- **Procedural generation**: all sounds are sine-wave waveforms generated at runtime — no asset files
- **Linear fade-out envelope** on every sound for clean decay
- **raylib `Sound` API**: `LoadSoundFromWave()` + `PlaySound()`

## AI System

The BFS pathfinding AI is preserved from the EasyX version. See [[BFS Pathfinding for Game AI]].

The `ai.h` header declares:
```cpp
Direction ai_choose_direction(const GameState *game, AIDifficulty difficulty);
```

Three difficulty tiers unchanged:
- **Easy**: 30% random, otherwise Manhattan distance to food
- **Normal**: BFS to food + 15% random override
- **Hard**: BFS + tail-reachability safety check (active when snake length ≥ 105)

## Rendering (raylib)

See `render.h` for the full color palette (18 named `Color` constants).

- **Font**: TTF loaded via `LoadFontEx()`, supports Chinese characters
- **Colors**: dark theme with semantic accents (green = player, blue = AI, red = food, orange = obstacle)
- **Board**: grid lines drawn with `DrawRectangleLines()`
- **Panel**: score, controls, status displayed in a side panel
- **Game over**: overlay with final score and restart hint

## Input Handling

See `input.h` for the edge-triggered vs level-triggered pattern.

```cpp
bool is_key_pressed(int key);  // edge: true only on frame key goes down
bool is_key_held(int key);     // level: true every frame key is held
```

Uses raylib's `IsKeyPressed()` and `IsKeyDown()` internally — no Win32 dependency.

## Key Observations

- **Proper C++ headers** — unlike the EasyX version which had no `.h` files, every module has a complete header
- **Cross-platform ready** — raylib runs on Windows, Linux, macOS (no Win32 API calls)
- **Audio from scratch** — procedural waveform synthesis avoids asset dependency
- **Polymorphism over enums** — `virtual update()/render()` replaces manual dispatch
- **IME disabled on Windows** — `ImmAssociateContext(NULL)` prevents text input popups during gameplay
- **Educational upgrade** — the headers and architecture make this a better teaching tool than the EasyX original

## Pages Created from This Source

- [[Procedural Audio for Games]] — concept page for the audio system
- [[Scene State Machine Pattern]] — concept page for the polymorphic scene architecture
- [[Edge-Triggered Input]] — concept page for input handling pattern
