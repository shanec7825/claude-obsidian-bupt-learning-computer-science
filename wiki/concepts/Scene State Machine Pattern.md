---
type: concept
title: "Scene State Machine Pattern"
created: 2026-06-23
updated: 2026-06-29
tags:
  - concept
  - cpp
  - game-dev
  - design-pattern
  - state-machine
  - polymorphism
status: current
related:
  - "[[snake-game-raylib]]"
  - "[[Edge-Triggered Input]]"
---

# Scene State Machine Pattern

> [!summary] 逐层摘要
> **Layer 1 (初次理解)**: 
> **Layer 2 (加粗关键)**: 
> **Layer 3 (高亮核心)**: 
> **Layer 4 (2-3 句自写总结)**: 

---

## The Pattern

Instead of a giant `switch(state)` in the game loop, each game "screen" (menu, gameplay, credits, settings) is a separate class implementing a common interface:

```cpp
class Scene {
public:
    virtual void update() = 0;
    virtual void render() = 0;
    virtual SceneType next_scene() const = 0;
    virtual ~Scene() {}
};
```

The main loop is reduced to:

```cpp
Scene *current = new MenuScene();
while (!WindowShouldClose()) {
    current->update();
    SceneType next = current->next_scene();
    if (next != current_type) {
        delete current;
        current = create_scene(next);
    }
    BeginDrawing();
    current->render();
    EndDrawing();
}
```

## Implementation (Snake Game)

### Class Hierarchy

```
Scene (abstract base)
  ├── MenuScene
  │     ├── Main menu state (_showing_difficulty == false)
  │     └── Difficulty sub-menu state (_showing_difficulty == true)
  └── PlayScene
        ├── Classic mode (MODE_CLASSIC)
        ├── PvP mode (MODE_PVP)
        └── Battle mode (MODE_BATTLE)
```

### SceneType Enum

```cpp
enum SceneType {
    SCENE_MENU,   // transition target: MenuScene
    SCENE_PLAY,   // transition target: PlayScene
    SCENE_EXIT    // break from main loop
};
```

### Transition Logic

In `main.cpp`, scene switching is driven by `next_scene()` return values:

1. **MenuScene → PlayScene**: user presses 1 (classic), 2 (PvP), or 3→difficulty→1/2/3 (battle)
2. **PlayScene → MenuScene**: user presses ESC
3. **Any → SCENE_EXIT**: user presses ESC on main menu

The window is resized during transitions:
```cpp
if (entering game from menu) {
    if (mode == MODE_CLASSIC)
        SetWindowSize(CLASSIC_WINDOW_WIDTH, CLASSIC_WINDOW_HEIGHT);
    else
        SetWindowSize(BATTLE_WINDOW_WIDTH, BATTLE_WINDOW_HEIGHT);
}
```

### MenuScene: Nested State

The menu has a **sub-state** for difficulty selection, managed by a boolean flag rather than a separate scene:

```cpp
class MenuScene : public Scene {
    bool _showing_difficulty = false;
    AIDifficulty _selected_difficulty = AI_NORMAL;
    SceneType _next = SCENE_MENU;

    void update() override {
        if (_showing_difficulty) {
            // 1/2/3 = select difficulty + enter battle mode
            // ESC = back to main menu
        } else {
            // 1 = classic, 2 = PvP, 3 = show difficulty menu
            // ESC = exit
        }
    }
};
```

This avoids creating a third `DifficultyMenuScene` class for what is essentially a second page of the same menu.

### PlayScene: Owns GameState

The `PlayScene` holds a complete `GameState` struct as a member:

```cpp
class PlayScene : public Scene {
    GameState _game;       // full game state
    double _last_move;     // tick timer
    AIDifficulty _difficulty;
    SceneType _next = SCENE_PLAY;

    PlayScene(GameMode mode, AIDifficulty difficulty) {
        // Dispatch to correct init function based on mode
        if (mode == MODE_BATTLE)
            init_battle_game(&_game, difficulty);
        else if (mode == MODE_PVP)
            init_pvp_game(&_game);
        else
            init_game(&_game, mode);
    }
};
```

The constructor parameterizes the scene — no need for a `set_mode()` post-creation call. The scene is fully initialized and ready to run after construction.

## Key Design Decisions

### Polymorphism over switch statements

| Approach | Pros | Cons |
|----------|------|------|
| `switch(state)` in game loop | Simple, all code in one place | Grows unboundedly, hard to test |
| Polymorphic scenes | Each scene isolated, testable independently | Virtual dispatch overhead (negligible) |

For games with 2-5 screens, the polymorphic approach is cleaner and more extensible.

### Scene owns its data

The `PlayScene` owns the `GameState` — it's not passed in from outside. This means:
- Creating a `PlayScene` starts a fresh game
- Destroying it frees all game resources
- Restart is `delete old; new PlayScene(mode, difficulty)`

### Dynamic allocation with raw pointers

The code uses `new`/`delete` rather than smart pointers. For this scale (2 scene objects, no shared ownership), raw pointers are acceptable. A `std::unique_ptr<Scene>` would be more modern but adds a header dependency.

### Window resize on transition

The main loop resizes the window when switching between menu and gameplay — the menu is 700×620 while gameplay is 1140×920 (classic) or 1180×920 (battle). raylib's `SetWindowSize()` does this without closing/reopening the window (unlike EasyX).

## When to Use

- **Games with 2-8 distinct screens** (menu, gameplay, pause, settings, credits, game over)
- **Each screen has different input mappings** (menu uses 1/2/3, gameplay uses WASD)
- **Screen transitions involve resource changes** (window resize, audio state, asset loading)
- **You want to test screens in isolation** (instantiate PlayScene directly, feed synthetic input)

## When Not to Use

- Single-screen games (no benefit)
- Games where screens share most state (a flat state enum is simpler)
- Performance-critical transitions (virtual dispatch has overhead, though rarely measurable)

## Contrast with Other Patterns

| Pattern | Mechanism | Best for |
|---------|-----------|----------|
| **Scene hierarchy** (this) | Polymorphic classes | 2-8 distinct screens |
| **Finite state machine** | `switch(state)` + transitions | Simple flows, menu trees |
| **Entity-Component** | ECS with system enable/disable | Complex games with overlapping systems |
| **Stack-based** | Push/pop scenes | Pause menus, modal dialogs |
