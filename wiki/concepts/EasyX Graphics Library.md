---
type: concept
title: EasyX Graphics Library
created: 2026-06-22
updated: 2026-06-22
tags:
  - concept
  - cpp
  - graphics
  - library
  - windows
  - game-dev
status: current
related:
  - "[[snake-game-easyx]]"
  - "[[BFS Pathfinding for Game AI]]"
---

# EasyX Graphics Library

> A lightweight Windows graphics library for C/C++, providing simple 2D drawing primitives as an alternative to the legacy Turbo C `graphics.h`.

## Overview

EasyX is a free, lightweight Windows graphics library for C and C++. It provides a `graphics.h` header with simple 2D drawing functions, making it popular in **Chinese C++ education** as a modern replacement for the Borland Turbo C graphics library. The library works with Visual Studio, MinGW, and other Windows C++ compilers.

**Website**: [easyx.cn](https://easyx.cn) (Chinese)

## Key Functions Used in Snake Game

### Window Management
- `initgraph(width, height)` — create a graphics window
- `closegraph()` — close the graphics window
- `cleardevice()` — clear the drawing surface

### Drawing
- `solidrectangle(left, top, right, bottom)` — filled rectangle
- `solidellipse(left, top, right, bottom)` — filled ellipse/circle
- `rectangle(left, top, right, bottom)` — outline rectangle
- `setfillcolor(color)` — set fill color (RGB)
- `setlinecolor(color)` — set line color
- `setbkcolor(color)` — set background color
- `setbkmode(mode)` — set background mode (OPAQUE / TRANSPARENT)

### Text
- `outtextxy(x, y, text)` — draw text at position
- `settextcolor(color)` — set text color
- `settextstyle(height, weight, fontname)` — set font properties
- `textwidth(text)` / `textheight(text)` — measure text dimensions

### Double Buffering
- `BeginBatchDraw()` — start batch (offscreen) drawing
- `FlushBatchDraw()` — flush to screen
- `EndBatchDraw()` — end batch drawing

### Input Utilities
- `GetAsyncKeyState(vk)` — Win32 API, check key state (not EasyX, but used together)
- `GetHWnd()` — get the graphics window handle

## Characteristics

| Property | Detail |
|----------|--------|
| Platform | Windows only (Win32 GDI) |
| Language | C / C++ |
| License | Freeware (proprietary, free for non-commercial use) |
| Dependencies | GDI32, IMM32 (IME control) |
| Origin | Chinese developer community |
| Use Case | Educational, hobbyist 2D games |

## Limitations

- **No hardware acceleration** — relies on GDI, slow for complex scenes
- **No sprite system** — everything is drawn with primitives each frame
- **No audio support** — requires separate libraries (e.g., `mciSendString`)
- **No input system** — relies on Win32 `GetAsyncKeyState`
- **Windows only** — no cross-platform support
- **Not actively updated** — last update unknown (project appears stable/feature-complete)

## Comparison

| Feature              | EasyX     | SDL2       | SFML       | Raylib     |
| -------------------- | --------- | ---------- | ---------- | ---------- |
| Cross-platform       | ❌         | ✅          | ✅          | ✅          |
| Hardware accelerated | ❌         | ✅ (OpenGL) | ✅ (OpenGL) | ✅ (OpenGL) |
| Audio                | ❌         | ✅          | ✅          | ✅          |
| Input handling       | ❌ (Win32) | ✅          | ✅          | ✅          |
| Learning curve       | Very low  | Medium     | Low        | Very low   |
| C support            | ✅         | ✅          | ❌ (C++)    | ✅          |
| Chinese docs         | ✅         | Partial    | Partial    | Partial    |

## Ecosystem

EasyX is most commonly used in:
- **Chinese college C/C++ courses** — first graphics library students learn
- **Competitive programming visuals** — simple algorithm visualization
- **Hobbyist 2D games** — Snake, Tetris, Flappy Bird clones
- **Windows C++ tutorials** — many online tutorials target EasyX

> [!contradiction] Snake game ported away from EasyX
> The Snake game that originally used EasyX was subsequently **ported to raylib** (see [[snake-game-raylib]]). The raylib version replaces all EasyX API calls with cross-platform equivalents, adds procedural audio, and introduces proper C++ headers. The EasyX original ([[snake-game-easyx]]) remains as documentation of the Windows-only approach.
