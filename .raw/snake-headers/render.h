#pragma once
#ifndef RENDER_H
#define RENDER_H

// ============================================================================
// render.h — 贪吃蛇图形渲染模块（EasyX 图形库）
//
// 负责所有屏幕绘制工作：
//   - draw_board()         ：绘制棋盘网格（墙 / 空地 / 食物 / 蛇 / 障碍）
//   - draw_panel()         ：绘制右侧信息面板（分数 / 长度 / 状态 / 操作提示）
//   - draw_game_over()     ：绘制游戏结束覆盖层
//   - draw_menu()          ：绘制主菜单界面
//   - draw_difficulty_menu() ：绘制 AI 难度选择子菜单
//   - draw_game()          ：统筹调用 draw_board + draw_panel + draw_game_over
//
// 颜色常量在 render.cpp 中以 RGB 宏内联定义，未暴露到头文件。
// ============================================================================

#include "game.h"

#ifdef __cplusplus
extern "C" {
#endif

// 绘制棋盘区域（根据 game->map_size 和 game->map 逐格渲染）
void draw_board(const GameState *game);

// 绘制右侧信息面板（根据 game->mode 显示不同布局：经典 / 对战 / PvP）
void draw_panel(const GameState *game);

// 绘制游戏结束覆盖文字（居中显示 GAME OVER / YOU LOSE 及重试提示）
void draw_game_over(const GameState *game);

// 绘制主菜单（清屏后居中渲染标题和三种模式选项）
void draw_menu(void);

// 绘制 AI 难度选择子菜单（清屏后居中渲染 Easy / Normal / Hard 选项）
void draw_difficulty_menu(void);

// 一帧完整绘制：清屏 -> 棋盘 -> 面板 -> 游戏结束覆盖层
void draw_game(const GameState *game);

#ifdef __cplusplus
}
#endif

#endif // RENDER_H
