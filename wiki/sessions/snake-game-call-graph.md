---
type: synthesis
title: "Snake Game (EasyX) — Complete Call Graph"
created: 2026-06-22
updated: 2026-06-22
tags:
  - reference
  - cpp
  - game
  - easyx
  - architecture
status: developing
related:
  - "[[snake-game-easyx]]"
  - "[[BFS Pathfinding for Game AI]]"
  - "[[EasyX Graphics Library]]"
question: "How do the C++ Snake Game functions call each other? What does each function mean?"
answer_quality: solid
---

# Snake Game (EasyX) — Complete Call Graph

> A function-level trace of the 6-file C++ Snake game using EasyX. Explains the call hierarchy, data flow, and what each key function does.

## Overview: The 6 Files

| File | Role |
|------|------|
| `main` | Entry point — creates window, runs the scene loop at ~60 FPS |
| `scene` | Scene state machine — MenuScene (menu + difficulty select) and PlayScene (game loop) |
| `game` | Core game logic — initialization, snake movement, collision resolution, PvP + battle modes |
| `ai` | AI decision-making — BFS pathfinding with 3 difficulty tiers |
| `input` | Keyboard input — `key_pressed()` (edge-detect) vs `key_held()` (continuous) |
| `render` | Drawing — map grid, info panel, menus, game over screen via EasyX |

## Scene Architecture (main + scene)

```
main()
  ├── initgraph()              # EasyX: create graphics window
  ├── disable_ime()            # input: turn off Chinese IME
  ├── BeginBatchDraw()         # EasyX: start double buffering
  │
  └── while(true) loop ─────────────► 60 FPS game loop
        ├── current->update()       # scene logic (Scene虚函数)
        │     ├── MenuScene::update()
        │     │     ├── key_pressed('1'..'3')  → set mode, set next scene
        │     │     └── key_pressed(Esc)       → exit or back
        │     │
        │     └── PlayScene::update()
        │           ├── key_pressed(Esc) → return to menu
        │           ├── key_pressed('R')  → restart_game()
        │           ├── key_pressed(Space) → toggle pause
        │           ├── WASD → set_direction()          # game: set P1 direction
        │           ├── Arrows → set_p2_direction()     # game: set P2 direction (PvP only)
        │           ├── key_held(Shift/F) → 4x speed    # 160ms→40ms interval
        │           └── Timer tick (160ms/40ms):
        │                 ├── move_snake()               # Classic mode
        │                 ├── move_pvp_snakes()          # PvP mode
        │                 └── move_battle_snakes()       # Battle mode (vs AI)
        │
        ├── current->render()       # scene: draw current scene
        │     ├── MenuScene::render()
        │     │     ├── draw_menu()              # 3 options + controls
        │     │     └── draw_difficulty_menu()   # Easy/Normal/Hard
        │     │
        │     └── PlayScene::render()
        │           └── draw_game()
        │                 ├── draw_board()       # grid + walls + snakes + food
        │                 ├── draw_panel()       # right-side info panel
        │                 └── draw_game_over()   # overlay text when dead
        │
        ├── FlushBatchDraw()       # EasyX: swap buffers (show frame)
        └── Sleep(16)              # hold for ~16ms to maintain 60 FPS
```

## Movement & Collision (game)

### move_snake() — Classic Mode (single snake)

```
move_snake(game)
  1. Check if started / paused / game_over → skip if so
  2. next_head(old_head, direction)       # compute where head will go
  3. Check if food is at next cell → will_eat
  4. Check auto-grow ((move_count+1) % grow_step == 0)
  5. can_enter_cell(next, will_grow)      # wall? obstacle? self body?
     ├── cell '#' → DEATH_WALL
     ├── cell 'O' → DEATH_OBSTACLE
     ├── cell 'B' → if it's tail and not growing → safe; else DEATH_BODY
     ├── cell 'C' → if it's AI tail and not growing → safe; else DEATH_ENEMY_BODY
     ├── cell 'H'/'A' → DEATH_HEAD_ON
     └── '.'/'F' → safe
  6. If dead → game_over = true
  7. If not dead:
     ├── Remove tail cell (if not growing) → map[tail] = '.'
     ├── Extend length (if growing)
     ├── Old head → body: map[old_head] = 'B'
     ├── Shift body forward: snake[i] = snake[i-1]
     ├── New head: snake[0] = next; map[next] = 'H'
     ├── If ate food: score+=10, place_food()
     └── turn_locked = false             # ready for next direction input
```

### move_pvp_snakes() — PvP Mode (two human snakes)

**Collision resolution order** (critical — checked in sequence):

```
1. Head-on (same cell) → both die
2. P1 head → P2 body → if not P2's moving tail → P1 dies, P2 eats P1's body
3. P2 head → P1 body → if not P1's moving tail → P2 dies, P1 eats P2's body
4. Wall / obstacle for either
5. Self-collision (own body, excluding own moving tail)
6. If one dies → game over, survivor's final move plays out
```

Key detail: **tail-passing** — if a head lands on the opponent's tail cell AND that tail is about to move (snake not growing), it's safe. This prevents unfair deaths when the tail is literally sliding away.

### move_battle_snakes() — Battle Mode (human vs AI)

Same collision logic as PvP, but AI can `respawn_ai()` after death instead of ending the game.

### Key Functions Table

| Function | File | Purpose |
|----------|------|---------|
| `next_head(head, dir)` | game | Given current head + direction, compute next cell |
| `can_enter_cell()` | game | Check if a target cell is passable (wall? body? tail-safe?) |
| `can_start_in_direction()` | game | Before game starts, verify direction won't insta-kill |
| `apply_snake_move()` | game | Execute one snake's move: shift body, update map, handle growth |
| `place_food()` | game | Place 'F' on a random empty '.' cell |
| `generate_obstacles()` | game | Scatter 'O' across map, capped at 25% of empty cells |
| `clear_snake_from_map()` | game | Remove a dead snake's body chars from the map grid |
| `respawn_ai()` | game | After AI death: find empty space, spawn 3-cell snake, ensure 2 food |
| `add_pending_growth()` | game | Queue growth credits (from eating opponent) for later ticks |
| `set_direction()` | game | Set P1 direction with 180° reversal protection and `turn_locked` |
| `set_p2_direction()` | game | Same as above but for P2 (PvP mode) |

## Input System (input)

```
key_pressed(vk)          # Edge-detected press: true for ONE frame
  1. GetAsyncKeyState(vk) & 0x8000  → current frame's state
  2. now_down && !was_down[vk]      → rising edge detection
  3. was_down[vk] = now_down        → remember for next frame

key_held(vk)             # Continuous hold: true every frame while held
  → just returns (GetAsyncKeyState(vk) & 0x8000)

disable_ime()            # Suppress Chinese IME popup during gameplay
  → ImmAssociateContext(GetHWnd(), NULL)

reset_key_state()        # Clear all remembered states (on scene transition)
  → memset(was_down, 0, 256)
```

**Why both exist**: `key_pressed` prevents repeating actions (one press = one direction change). `key_held` is for continuous effects (hold Shift to speed up).

## AI Decision Chain (ai)

```
ai_choose_direction(game_state, difficulty)
  │
  ├── ai_dir_easy()           # 30% random + Manhattan distance
  │     ├── 30%: random valid direction
  │     ├── else: minimize Manhattan distance to food |nr-fr| + |nc-fc|
  │     ├── fallback: maximize ai_free_count() (most free space)
  │     └── last resort: any valid direction
  │
  ├── ai_dir_normal()         # BFS shortest path + 15% random interrupt
  │     ├── ai_bfs_from_food() → compute full distance grid
  │     ├── pick direction minimizing dist[nr][nc]
  │     ├── 15%: random valid direction override
  │     ├── fallback: max free space
  │     └── No tail-reachability safety → still traps itself
  │
  └── ai_dir_hard()           # BFS + tail-reachability safety
        ├── ai_bfs_from_food() → distance grid
        ├── Stage gate: if length < 105 → greedy BFS (no safety)
        ├── if length ≥ 105 → ai_can_reach_tail_after_move() per direction
        │     ├── Copies game map
        │     ├── Simulates head moving in direction d
        │     ├── If eating food or auto-grow → tail stays
        │     ├── Else → remove tail from simulated map
        │     └── ai_bfs_path() → BFS from new head to new tail position
        ├── Only consider directions where tail is reachable
        └── Fallback: max free space
```

### AI Helper Functions

| Function | Complexity | Purpose |
|----------|-----------|---------|
| `ai_bfs_from_food()` | O(N²) | BFS outward from food; produces distance grid used for shortest-path decisions |
| `ai_free_count()` | O(N²) | Flood-fill count of reachable empty cells; used as survival fallback |
| `ai_can_reach_tail_after_move()` | O(N²) | Simulate move on a copied map, then BFS-check tail connectivity |
| `ai_bfs_path()` | O(N²) | General BFS on any simulated grid with obstacles |
| `collect_valid_dirs()` | O(1) | Returns which of 4 directions are passable from current head |

## Not in the Source Files

These are **referenced but not defined** in the provided `.raw/` files (they live in the missing `.h` headers):

- `GameState` struct — holds everything: map[][], both snakes, scores, flags, timers
- `Direction` enum — UP, DOWN, LEFT, RIGHT
- `GameMode` enum — MODE_CLASSIC, MODE_PVP, MODE_BATTLE
- `AIDifficulty` enum — AI_EASY, AI_NORMAL, AI_HARD
- `DeathReason` enum — DEATH_NONE, DEATH_WALL, DEATH_OBSTACLE, DEATH_BODY, DEATH_ENEMY_BODY, DEATH_HEAD_ON, DEATH_FULL_MAP
- `SceneType` enum — SCENE_MENU, SCENE_PLAY, SCENE_EXIT
- `Scene` class (abstract) with virtual `update()`, `render()`, `next_scene()`
- Constants: MAP_SIZE, BATTLE_MAP_SIZE, CELL_SIZE, MOVE_INTERVAL_MS, MAX_SNAKE_LEN, GROW_STEP
