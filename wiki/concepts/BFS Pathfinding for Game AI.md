---
type: concept
title: "BFS Pathfinding for Game AI"
created: 2026-06-22
updated: 2026-06-29
tags:
  - concept
  - ai
  - pathfinding
  - bfs
  - game-dev
status: current
related:
  - "[[snake-game-easyx]]"
  - "[[EasyX Graphics Library]]"
---

# BFS Pathfinding for Game AI

> [!summary] 逐层摘要
> **Layer 1 (初次理解)**: 
> **Layer 2 (加粗关键)**: 
> **Layer 3 (高亮核心)**: 
> **Layer 4 (2-3 句自写总结)**: 

---

## Overview

BFS (Breadth-First Search) is a natural fit for grid-based game AI where:
- The map is a uniform-cost grid (each move costs 1)
- The goal is the shortest path to a target (food)
- The environment changes every tick (snakes move, food gets eaten)

BFS guarantees the shortest path in unweighted graphs, which makes it ideal for snake AI where every cell move costs exactly one turn.

## Architecture in Snake Game

### Core Functions

| Function | Time Complexity | Purpose |
|----------|----------------|---------|
| `ai_bfs_from_food()` | O(N²) | Flood distance grid from food outward |
| `ai_free_count()` | O(N²) | Flood-fill count of reachable empty cells |
| `ai_can_reach_tail_after_move()` | O(N²) | Simulation + BFS connectivity check |
| `ai_bfs_path()` | O(N²) | BFS on simulated grid with obstacles |

Where N = map size (20 for classic, 35 for battle/PvP).

### Difficulty Tiers

```
Easy        Normal          Hard
  │           │               │
  ├─30% random ├─BFS shortest  ├─BFS shortest
  ├─Manhattan  ├─15% random    ├─Tail safety (len ≥ 105)
  └─No safety  └─No safety     └─Free-space fallback
```

#### Easy
- 30% chance of a random valid direction (chaotic play)
- Otherwise: minimizes **Manhattan distance** to food (not BFS)
- No tail-reachability check → frequently traps itself
- Fallback: most free space → any valid direction

#### Normal
- Full BFS shortest path to food
- 15% random override (keeps it beatable)
- **No tail-reachability safety** — still dies by self-trapping, just slower
- Best balance of challenge and beatability

#### Hard
- BFS shortest path to food with **tail-reachability safety**
- Stage-gated behavior:
  - **Early game** (length < 105): greedy BFS, no safety (fast aggressive play)
  - **Late game** (length ≥ 105): safety checks active, prioritizes survival
- `ai_can_reach_tail_after_move()` simulates the move on a copy of the map, then BFS-checks if the tail cell is reachable from the new head position
- If no safe food path exists, falls back to most-free-space heuristic

### Tail-Reachability Safety (Hard Mode Only)

The key insight that separates Hard from Normal:

```cpp
static bool ai_can_reach_tail_after_move(const GameState *game, int d)
{
    // 1. Simulate head moving in direction d
    // 2. Check if food will be eaten (which changes tail behavior)
    // 3. Copy map, remove tail (if snake won't grow), place head
    // 4. BFS from new head to new tail position
    // 5. Return whether a path exists
}
```

This prevents the classic snake AI death spiral: chasing food until the snake fills the map and has no escape route.

## Related Patterns

- **A\*** — would give better performance for large maps (heuristic-guided, vs BFS which explores all directions equally)
- **Hamiltonian cycle** — guarantees survival but reduces food-seeking efficiency
- **Flood fill** — used here for `ai_free_count()` fallback (counts reachable space)
- **Simulation testing** — the tail-reachability check runs BFS on a *copy* of the game state, simulating the move before committing

## Performance Considerations

- O(N²) per tick × 4 directions = ~4N² worst-case per frame
- At N=35 (battle/PvP map), each BFS touches up to 1,225 cells
- Three BFS passes per Hard AI move (food distance + 4× tail check + fallback)
- Acceptable at 60 FPS for a grid this size — would need optimization for larger maps

## Use Cases Beyond Snake

The same BFS + simulation + safety-check pattern applies to:
- **Pac-Man ghost AI** (shortest path to player + flee mode)
- **Tower defense** (unit pathfinding with obstacles)
- **Roguelike enemy movement** (grid-based tactical AI)
