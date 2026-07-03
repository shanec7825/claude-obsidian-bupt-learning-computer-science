---
type: meta
title: "Playground"
created: 2026-07-03
tags:
  - playground
  - learning
  - code
status: evergreen
---

# Playground

This is where you learn by doing. Each project lives in its own folder with source code and a `LEARN.md` guide.

## Why Playground Exists

The [[index|Wiki]] is your **map** — it tells you what exists, how things connect, and where to go next. But learning to code happens in the **IDE**, not in Obsidian. The Playground puts code and learning material side by side so you never switch apps mid-flow.

## How to Use

```
1. Open the vault folder in VS Code (or your IDE)
2. Navigate to playground/<project>/
3. Open LEARN.md on the left, src/ on the right
4. Read → Write → Run → Debug → Understand
5. After learning: come back to Obsidian to write a synthesis note
```

## Projects

| Project | Status | Stack | Key Concept |
|---------|--------|-------|-------------|
| [[snake/LEARN\|Snake]] | ✅ Complete | raylib + C++ | Scene polymorphism, game state machine |
| [[tetris/LEARN\|Tetris]] | ✅ Complete | raylib + C++ | Class inheritance, grid modeling |

## Adding a New Project

1. Create `playground/<name>/src/`
2. Add source files
3. Write `LEARN.md` (see snake/LEARN.md for template)
4. Link from `wiki/canvases/cs-knowledge-graph.canvas`

## Relation to Wiki

- **Playground**: Day-to-day learning. Code lives here. LEARN.md is your guide during coding sessions.
- **Wiki**: Long-term synthesis. Cross-project connections. Methodology.
- **Rule**: If it only applies to one project → in LEARN.md. If it spans multiple projects → in wiki/concepts/.

See [[index]] for the full knowledge map.
