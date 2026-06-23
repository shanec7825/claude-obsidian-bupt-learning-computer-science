#pragma once
#ifndef SCENE_H
#define SCENE_H

// ============================================================================
// scene.h — 贪吃蛇游戏场景管理模块
//
// 使用简单的场景状态机管理游戏流程：
//   MenuScene → PlayScene → MenuScene → … → SCENE_EXIT
//
// 类层次：
//   Scene（抽象基类）
//     ├── MenuScene（主菜单 / 难度选择）
//     └── PlayScene（游戏进行中，内部包含 GameState）
//
// 场景切换由 main.cpp 主循环驱动：
//   每帧调用 update() → render() → next_scene()，根据返回值决定是否切换。
// ============================================================================

#include "game.h"
#include <windows.h>   // DWORD

// ============================================================================
// 场景类型枚举 —— 主循环根据 next_scene() 的返回值切换场景
// ============================================================================
enum SceneType {
    SCENE_MENU,   // 主菜单（含难度选择子状态）
    SCENE_PLAY,   // 游戏进行中
    SCENE_EXIT    // 退出程序
};

// ============================================================================
// Scene —— 场景抽象基类
//
// 所有场景必须实现 update() / render() / next_scene() 三个纯虚函数。
// 虚析构函数确保通过基类指针 delete 子类对象时正确释放资源。
// ============================================================================
class Scene {
public:
    // 每帧调用，处理输入并更新场景内部状态
    virtual void update() = 0;

    // 每帧调用，绘制当前场景画面
    virtual void render() = 0;

    // 返回期望切换到的目标场景类型。
    // 若返回值与当前场景相同，主循环不做切换（用于"保持"语义）。
    virtual SceneType next_scene() const = 0;

    virtual ~Scene() {}
};

// ============================================================================
// MenuScene —— 主菜单场景
//
// 内部状态机：
//   主菜单（_showing_difficulty == false）：
//     按键 1 → MODE_CLASSIC → 进入 PlayScene
//     按键 2 → MODE_PVP     → 进入 PlayScene
//     按键 3 → 切换到难度选择子菜单
//     ESC   → SCENE_EXIT
//   难度选择子菜单（_showing_difficulty == true）：
//     按键 1/2/3 → 设置难度 + MODE_BATTLE → 进入 PlayScene
//     ESC       → 返回主菜单
// ============================================================================
class MenuScene : public Scene {
public:
    // 用户在菜单中选定的游戏模式（供 main.cpp 读取后创建 PlayScene）
    GameMode selected_mode;

    MenuScene();

    void update() override;
    void render() override;
    SceneType next_scene() const override;

    // 返回用户选定的 AI 难度（仅在 Battle 模式下有意义）
    AIDifficulty selected_difficulty() const;

private:
    bool _showing_difficulty;           // 当前是否显示难度选择子菜单
    AIDifficulty _selected_difficulty;  // 用户选定的 AI 难度
    SceneType _next;                    // 期望切换到的目标场景
};

// ============================================================================
// PlayScene —— 游戏进行中场景
//
// 持有完整的 GameState，负责：
//   - 键盘输入分发（WASD / 方向键 / 空格 / R / ESC）
//   - 基于 MOVE_INTERVAL_MS 的定时移动（支持 Shift/F 加速）
//   - 调用 game.cpp 中的 move_snake / move_battle_snakes / move_pvp_snakes
//   - 调用 render.cpp 中的 draw_game 绘制画面
//
// 构造时根据 GameMode 调用对应的 init_* 函数初始化 GameState。
// ============================================================================
class PlayScene : public Scene {
public:
    // 构造并初始化一场新游戏。
    // mode       —— 游戏模式，决定调用 init_game / init_pvp_game / init_battle_game
    // difficulty —— AI 难度（仅 MODE_BATTLE 时有效，其他模式忽略）
    PlayScene(GameMode mode, AIDifficulty difficulty);

    void update() override;
    void render() override;
    SceneType next_scene() const override;

private:
    GameState _game;       // 游戏运行时状态
    DWORD _last_move;      // 上次执行移动的 tick 时间戳（用于帧率控制）
    SceneType _next;       // 期望切换到的目标场景
};

#endif // SCENE_H
