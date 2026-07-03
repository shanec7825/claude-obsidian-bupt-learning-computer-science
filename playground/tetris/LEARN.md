---
type: learn
project: tetris
created: 2026-07-03
tags:
  - learn
  - tetris
  - cpp
  - raylib
  - game
status: complete
prev: "[[../snake/LEARN|Snake]]"
wiki:
  - "[[Cpp-Tetris-Architecture-Learning]]"
  - "[[GitHub Repo Learning Evaluation Checklist]]"
  - "[[Agent-Generated Learning Path]]"
---

# Tetris Game — Learning Guide

> **Open in IDE.** Left: LEARN.md. Right: src/. `make && make run`.

## Quick Start

```bash
make && make run    # build + play
make clean          # reset
```

---

## Architecture Overview

```
main.cpp
  │
  └── Game  (orchestrator — no Scene abstraction!)
        ├── Grid        — 2D array of cell states (occupied/empty/color)
        ├── Block       — abstract base class for pieces
        │     ├── IBlock    — 4×1 straight piece
        │     ├── JBlock    — L-shaped
        │     ├── LBlock    — reverse L
        │     ├── OBlock    — 2×2 square
        │     ├── SBlock    — S/Z shaped
        │     ├── TBlock    — T-shaped
        │     └── ZBlock    — Z/S shaped
        │
        ├── Game::HandleInput()   — instant response (not edge-triggered!)
        ├── Game::Draw()          — inline rendering
        └── Game::MoveBlockDown() — gravity + lock + row clear
```

**Key Pattern**: Classic OOP inheritance. `Block` defines 4 rotation states as `vector<Position> cells[4]`, each subclass fills in its shape. The `Game` class is a monolithic orchestrator — no Scene abstraction, no separate input/render modules.

**Contrast with Snake**: Snake's `Scene` polymorphism handles MODE switching (menu/play/exit). Tetris has no mode switching — there's only one game mode — so it doesn't need Scene. Different problem → different architecture. This is the core lesson of [[Cpp-Tetris-Architecture-Learning]].

---

## Learning Path (12 Steps)

Follow this order. Each step builds on the previous.

### Step 1: `grid.h` + `grid.cpp` — The Board
- **Why first**: Everything happens on the grid. Understand the 2D array before anything else.
- **What to trace**: How is a cell represented? What does `IsCellEmpty(row, col)` return?

### Step 2: `block.h` + `block.cpp` — The Piece Base Class
- **Key data**: `cells[4]` — 4 rotation states, each is a `vector<Position>`
- **Key method**: `Rotate()` — cycles through states[0..3]
- **Self-explanation**: Why 4 hardcoded states instead of a rotation matrix? (Hint: Tetris pieces have at most 4 orientations, and hardcoding avoids floating-point math)

### Step 3: `blockI.cpp` through `blockZ.cpp` — The 7 Pieces
- Read one subclass. Notice how `cells[0..3]` is initialized with specific positions.
- **Pattern**: Each piece type is defined by its shape, not its behavior. Behavior lives in `Block` base.

### Step 4: `game.h` — The Orchestrator Interface
- What data does `Game` own? (grid, current block, next block, score, game state)
- What operations does it expose?

### Step 5: `game.cpp: Game::Game()` — Constructor
- Initialization order: grid init → block spawn → score reset
- Trace what happens in the first frame

### Step 6: `game.cpp: HandleInput()` — Player Input
- **Differences from Snake**: Instant response (not edge-triggered). Left/right moves immediately every frame. Why? Because Tetris needs responsive horizontal movement.
- **Contrast**: Snake uses edge-triggered input to prevent multiple turns per frame. Different game → different input strategy.

### Step 7: `game.cpp: MoveBlockDown()` — Gravity + Lock + Clear
- **Core loop**: move down → if blocked → lock piece → check rows → clear + shift → spawn next
- This is the heart of Tetris. Trace it with a piece at the top, then at the bottom.

### Step 8: `game.cpp: LockBlock()` — Freezing a Piece
- What happens to the grid when a piece locks?
- How does the grid "remember" which cells are occupied?

### Step 9: `game.cpp: ClearFullRows()` — Row Clearing
- How are full rows detected?
- How are rows above shifted down? (This is subtle — trace with a 2-row clear)

### Step 10: `game.cpp: Draw()` — Rendering
- How does each cell's color propagate from Block to Grid to screen?
- Raylib drawing calls

### Step 11: `game.cpp: GameOver()` + Scoring
- Game over condition
- Score calculation (lines cleared → points)

### Step 12: `main.cpp` — The Game Loop
- Minimal wrapper. Contrast with Snake's `main.cpp` which manages Scene transitions.

---

## Hands-On Exercises

### Exercise 1: Speed Up Over Time
- **Goal**: Tetris gets faster as the score increases
- **Learn**: Game loop timing, difficulty curves

### Exercise 2: Add a "Hold Piece" Feature
- **Goal**: Press a key to store current piece, swap with held piece (once per drop)
- **Files**: `game.h`, `game.cpp`, `block.h`
- **Learn**: Adding game mechanics across data + logic layer

### Exercise 3: Add a Ghost Piece
- **Goal**: Show a translucent preview of where the piece will land
- **Learn**: Projection logic (simulate dropping without locking)

### Exercise 4: Extract the Grid to a Separate Class
- **Goal**: `Game` is too big. Move all grid operations into their own class.
- **Learn**: Refactoring for separation of concerns

---

## Snake vs Tetris: Architecture Comparison

Open both `LEARN.md` files side by side. These two projects use the SAME tech stack (raylib + C++) but COMPLETELY different architectures:

| Aspect | Snake | Tetris |
|--------|-------|--------|
| **Core abstraction** | Scene polymorphism | Block inheritance tree |
| **Game state** | Centralized `GameState` struct | Scattered across Grid + Game |
| **Input** | Edge-triggered (separate file) | Instant response (inline) |
| **Rendering** | Dedicated `render.cpp` | `Game::Draw()` inline |
| **Audio** | Procedural sine-wave synthesis | raylib audio file playback |
| **Data structure** | `deque<Point>` for snake body | `int grid[20][10]` 2D array |
| **Why this design?** | Multiple modes (menu/play/ai) | Single mode, piece variety |

**The meta-lesson**: There is no "correct" architecture. The architecture serves the problem. Snake needs mode switching → Scene polymorphism. Tetris needs piece variety → class inheritance. Understanding WHY is more important than remembering WHICH.

Full analysis: [[Cpp-Tetris-Architecture-Learning]]

---

## When Stuck

Same rules as Snake:
1. **20-minute rule**: Zoom in to one function.
2. **Print debug**: Add output in `MoveBlockDown()`.
3. **Comment out**: What breaks? That's what the line does.
4. **Wiki concepts**: Each concept has a [[wiki/concepts/|wiki page]] for theory.
5. **Compare with Snake**: Same raylib calls, different architecture patterns.

---

## After You've Finished

1. Write a TIL: "What Tetris taught me about class inheritance that Snake's Scene system didn't"
2. File one synthesis note in wiki if it connects both projects
3. Next: Pick a project WITHOUT a tutorial and use the [[Agent-Generated Learning Path|Agent path workflow]]
