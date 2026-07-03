---
type: source
title: "Snake Game Curriculum — Reverse-Engineered Headers"
created: 2026-06-23
updated: 2026-06-23
tags:
  - source
  - cpp
  - curriculum
  - reverse-engineering
  - headers
status: current
related:
  - "[[snake-game-easyx]]"
  - "[[snake-game-raylib]]"
  - "[[Snake Game Curriculum]]"
raw_file: ".raw/snake-headers/*.h (5 files)"
---

# Source: Snake Game Curriculum — Reverse-Engineered Headers

**Type**: Curriculum artifacts (5 header files)
**Date**: 2026-06-23
**Files**: `snake-headers/game.h`, `snake-headers/ai.h`, `snake-headers/input.h`, `snake-headers/render.h`, `snake-headers/scene.h`

## Summary

Five C++ header files reverse-engineered from the original EasyX snake game source code. These are **curriculum artifacts** — extensively commented in Chinese, designed as teaching materials for the [[Snake Game Curriculum]]. Unlike the raylib port's working headers (`.raw/*.h`), these document the **EasyX API** usage with full Chinese-language annotations explaining every constant, struct field, enum value, and function parameter.

## Purpose

The original EasyX source files (`.raw/ai`, `.raw/game`, etc.) had **no header files** — all type definitions, constants, and function signatures had to be inferred from the `.cpp` bodies. For the curriculum, these headers were reverse-engineered to:

1. **Make the architecture explicit** — students can read the headers to understand the full type system before diving into implementation
2. **Serve as API documentation** — every function has Chinese parameter/return-value documentation
3. **Teach header discipline** — demonstrates proper `#pragma once`, include guards, and `extern "C"` patterns
4. **Bridge EasyX to raylib** — the curriculum headers document the EasyX API while the actual `.h` files in `.raw/` use raylib

## Header-by-Header

### `game.h` (193 lines) — Core Data Structures

The largest and most important header. Documents:
- **15 `#define` constants**: map sizes, window dimensions, cell size, move interval, growth step
- **4 enums**: `Direction`, `GameMode`, `AIDifficulty`, `DeathReason` — each value with Chinese comments
- **`Point` struct**: row/col coordinate pair
- **`GameState` struct** (148 bytes of fields): the complete runtime state with `char map[][]`, both snake arrays, mode flags, turn locks, growth tracking
- **14 function declarations**: `init_game()`, `move_snake()`, `move_battle_snakes()`, `set_direction()`, etc.
- **Section headers**: organized by concern (map/网格常量, 游戏机制常量, 绘制/布局常量, 窗口尺寸常量)

Key detail: documents the **EasyX window management** functions (`initgraph`, `closegraph`) and **EasyX-specific layout** — differs from the raylib port's `SetWindowSize()` approach.

### `scene.h` (114 lines) — Scene State Machine

Documents the polymorphic scene system:
- **`SceneType` enum**: `SCENE_MENU`, `SCENE_PLAY`, `SCENE_EXIT`
- **`Scene` abstract base class**: `virtual update()`, `render()`, `next_scene()`
- **`MenuScene`**: main menu + difficulty sub-menu, selected mode/difficulty accessors
- **`PlayScene`**: owns `GameState`, handles input dispatch, timed movement, game restart

Includes detailed ASCII art of the transition flow and state machine logic in Chinese comments.

### `ai.h` (38 lines) — AI Pathfinding Interface

Minimal interface documenting:
- **`ai_choose_direction()`** signature with difficulty parameter
- Strategy summary for each difficulty tier in Chinese comments
- BFS usage note: "all strategies use BFS on a simulated map to evaluate candidate path safety"

### `input.h` (45 lines) — Keyboard Input (Win32)

Documents the EasyX-era input system:
- **`key_pressed()`**: edge-triggered via `GetAsyncKeyState` — for menu/direction
- **`key_held()`**: level-triggered — for sustained actions (speed-up)
- **`disable_ime()`**: IME suppression for Chinese Windows
- **`reset_key_state()`**: clears key history on window resize

Notable: this documents the **Win32 `GetAsyncKeyState`** API, while the raylib port uses `IsKeyPressed()`/`IsKeyDown()`.

### `render.h` (47 lines) — Rendering (EasyX)

Documents the EasyX graphics layer:
- **`draw_board()`**: grid rendering with tile-by-tile character-to-color mapping
- **`draw_panel()`**: info panel (mode-dependent layout)
- **`draw_game_over()`**: overlay text
- **`draw_menu()`** / **`draw_difficulty_menu()`**: menu screens
- **`draw_game()`**: orchestrator that calls the above in sequence

Notes that color constants are defined inline in `render.cpp` as `RGB()` macros — not exposed in the header.

## Contrast: Curriculum Headers vs raylib Headers

| Dimension | snake-headers/ (EasyX) | .raw/*.h (raylib) |
|-----------|----------------------|-------------------|
| Graphics API | EasyX (`initgraph`, `BeginBatchDraw`) | raylib (`InitWindow`, `BeginDrawing`) |
| Input | Win32 `GetAsyncKeyState` | raylib `IsKeyPressed`/`IsKeyDown` |
| Audio | Not documented | 4 SFX via `audio.h` |
| Language | Chinese comments (教学用) | English identifiers (production) |
| Level of detail | Every field/constant explained | Standard header style |
| Window resize | `closegraph()` + `initgraph()` | `SetWindowSize()` in-place |
| Font | EasyX `settextstyle()` | raylib `LoadFontEx()` |
| Target audience | Students learning C++ game dev | Working game code |

## Educational Value

These headers serve as a **bridge document** between the original EasyX source and the raylib port. Students can:
1. Read the curriculum headers to understand the architecture in Chinese
2. Compare with the raylib `.h` files to see how the API surface evolved
3. Use as a reference when working through the [[Snake Game Curriculum]] chapters

## Pages Created from This Source

- Cross-references were added to existing pages rather than creating new ones (the concepts are already covered)
