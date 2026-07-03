---
type: source
title: "C++ Snake Game (EasyX)"
created: 2026-06-22
updated: 2026-06-22
tags:
  - source
  - cpp
  - game
  - easyx
status: current
related:
  - "[[BFS Pathfinding for Game AI]]"
  - "[[EasyX Graphics Library]]"
raw_file: ".raw/ai, .raw/game, .raw/input, .raw/main, .raw/render, .raw/scene"
---

# Source: C++ Snake Game (EasyX)

**Type**: Source code (6 C++ files)
**Date**: 2026-06-22
**Files**: `ai`, `game`, `input`, `main`, `render`, `scene` (no extension — raw .cpp)

## Summary

A complete Snake game for Windows written in C++ using the **EasyX** graphics library. The project demonstrates grid-based game architecture, BFS pathfinding AI, multi-player collision resolution, and scene-based state management. Note: header files (`.h`) referenced in the source are not included.

## Game Modes

| Mode | Map Size | Players | Description |
|------|----------|---------|-------------|
| **Classic** | 20×20 | 1 | Standard snake with obstacles, auto-grow every N moves |
| **PvP** | 35×35 | 2 | Two human players (WASD vs Arrows), head-on collision = death |
| **Battle** | 35×35 | 1 vs AI | Human vs computer with 3 difficulty levels |

## Architecture

### Scene State Machine

```
MenuScene → PlayScene → MenuScene (loop)
              ↓
          Scene.Exit (quit game)
```

- `MenuScene`: main menu + difficulty sub-menu
- `PlayScene`: game loop with input → update → render cycles at ~60 FPS
- Scene transitions resize the graphics window dynamically

### Game State (`GameState` struct)

Centralized state struct holding:
- Map grid (char array: `#` wall, `.` empty, `F` food, `O` obstacle, `H`/`B` player snake, `A`/`C` AI snake)
- Both snakes (position arrays, direction, length, score)
- Game flags (started, paused, game_over, pause lock)
- Timing (last move timestamp, move count)
- PvP-specific fields (P2 direction lock, pending growth credits)

### Movement System

- Tick-based: 160ms interval by default, **40ms (4×) when Shift or F held**
- `turn_locked` flag prevents multiple direction changes per tick
- `pending_growth` credits decouple growth from instant tail extension

### Collision Resolution (Ordered)

Critical implementation detail: collision checks follow a specific order with tail-passing rules:

1. **Head-on** (both heads same cell → both die)
2. **Head vs body** (check opponent tail first — safe if tail is about to move)
3. **Wall/obstacle**
4. **Self-collision** (own tail safe check)
5. Kill rewards: surviving snake grows by eating the dead snake's entire length

## AI System (BFS Pathfinding)

Three difficulty levels with increasing sophistication:

### Easy (30% random + Manhattan heuristic)
- 30% chance of random valid move
- Otherwise: Manhattan distance to food
- Fallback: move toward most free space
- No safety check — frequently traps itself

### Normal (BFS + 15% random override)
- Full BFS shortest-path to food
- 15% chance of random override (adds unpredictability)
- No tail-reachability safety — can still trap itself
- Mid-tier challenge

### Hard (BFS + tail-reachability + staged strategy)
- BFS shortest-path to food
- **Tail-reachability safety**: before committing to a move, simulates whether it can still reach its tail after the move (avoids trapping)
- Stage-gated: < 105 length = greedy BFS; ≥ 105 length = safety checks active
- Last resort: most-free-space heuristic
- Near-perfect play when not cornered

### Key AI Functions

| Function | Purpose |
|----------|---------|
| `ai_bfs_from_food()` | BFS from food outward, computes distance grid |
| `ai_free_count()` | Flood-fill count of reachable empty cells |
| `ai_can_reach_tail_after_move()` | Simulates move and checks if tail is still reachable |
| `ai_bfs_path()` | BFS on a simulated grid to test connectivity |

## Rendering (EasyX)

- Grid-based board rendering with `CELL_SIZE` pixel tiles
- Color scheme: green player, blue AI, red food, gray obstacles, dark walls
- Food rendered as circle on red square (distinctive visual)
- Info panel to the right of game board showing scores, status, controls
- Three separate rendering functions: `draw_menu()`, `draw_difficulty_menu()`, `draw_game()`

## Input Handling

- `key_pressed()`: edge-detected key press (requires `GetAsyncKeyState`)
- `key_held()`: continuous key hold detection
- IME disabled for gaming (no text composition popups)
- `reset_key_state()`: clears all stored key states on scene transitions

## Key Observations

- **No header files provided** — structure definitions (`GameState`, `Scene`, enums) are inferred
- **Windows-only** — depends on Win32 API (`GetAsyncKeyState`, `GetTickCount`, `ImmAssociateContext`)
- **EasyX-specific** — `initgraph()`, `BeginBatchDraw()`, `outtextxy()`, etc.
- **Educational value** — clean architecture for a beginner-intermediate C++ game project
- **Potential improvements**: fixed timestep, entity-component system, proper delta time

## Pages Created from This Source

- [[BFS Pathfinding for Game AI]] — AI concept page
- [[EasyX Graphics Library]] — platform concept page

> [!note] raylib port available
> A raylib-based rewrite with proper headers, audio, and cross-platform support exists at [[snake-game-raylib]]. The reverse-engineered curriculum headers are at [[snake-curriculum-headers]].
