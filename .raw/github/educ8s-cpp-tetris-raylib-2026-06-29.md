---
source_type: github-repo
source_url: https://github.com/educ8s/Cpp-Tetris-Game-with-raylib
fetched: 2026-06-29
repo_author: educ8s
repo_name: Cpp-Tetris-Game-with-raylib
language: C++ (100%)
stars: 113
forks: 22
license: MIT
platform: Windows, macOS, Linux
---

# educ8s/Cpp-Tetris-Game-with-raylib

## Repository Overview

A complete Tetris game built with C++ and raylib. Features all 7 tetrominoes, rotation, collision detection, row clearing, scoring, next-block preview, background music, and sound effects. Accompanied by a YouTube video tutorial that walks through every line of code.

**Author recommendation**: Watch the raylib setup tutorial first, and complete the Pong and Snake tutorials as warm-ups before this project.

## File Structure

```
├── .vscode/          # VS Code workspace configuration
├── Font/             # Game font assets
├── Sounds/           # Background music and SFX
├── lib/              # raylib library dependencies
├── src/              # Source code (classes, game logic)
├── .gitattributes
├── .gitignore
├── Makefile
├── README.md
├── main.code-workspace
└── preview.jpg
```

## Architecture

- **Grid**: 20-row × 10-column grid as a 2D integer array. Empty = 0; locked cells = color ID 1–7.
- **Blocks**: Each of 7 tetrominoes stores occupied cell positions across all 4 rotation states. Rotation cycles through a `rotationState` index rather than using transformation math.
- **Game class**: Orchestrator holding the grid, current block, and next block. Exposes `HandleInput()` and `Draw()`.
- **Inheritance**: Base `Block` class with 7 derived classes: `LBlock`, `IBlock`, `TBlock`, etc.
- **Randomization**: 7 tetrominoes drawn in a shuffled bag — each appears once before repeats (classic Tetris behavior).
- **Game over**: When a newly spawned block overlaps an already-locked cell. Any key press restarts.

## Tutorial Learning Path (12 Steps)

1. Game loop & window setup (raylib window, frame rate, update/draw loop)
2. Grid class (2D array representation, color-coded cell drawing)
3. Block & tetromino classes (7 pieces, 4 rotation states, bounding-grid approach)
4. Inheritance (base Block → 7 derived classes)
5. Movement & rotation (arrow keys, state-cycling rotation with undo on invalid moves)
6. Collision detection (boundary checking, cell-occupancy validation)
7. Block locking (freezing into grid, spawning next piece)
8. Row clearing (detecting full rows, shifting rows downward)
9. Scoring (100/300/500 points for 1/2/3 lines; 1 point per manual drop)
10. UI (score display, next-block preview, Game Over message)
11. Audio (looping background music, SFX for rotation and row clears)
12. Game reset (restart on any key press after Game Over)

## Video Tutorial

YouTube: `youtu.be/wVYKG_ch4yM`
Channel: `youtube.com/channel/UC3ivOTE5EgpmF2DHLBmWIWg` (educ8s)
