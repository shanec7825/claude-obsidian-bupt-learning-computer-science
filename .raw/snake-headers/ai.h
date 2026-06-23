#pragma once
#ifndef AI_H
#define AI_H

// ============================================================================
// ai.h — 贪吃蛇 AI 寻路模块
//
// 提供 ai_choose_direction() 函数，根据当前游戏状态和难度等级，
// 为 AI 蛇选择最优移动方向。
//
// 难度策略概述：
//   AI_EASY   —— 30% 随机移动 + 曼哈顿距离寻食物 + 最大空地回退
//   AI_NORMAL —— BFS 距离寻食物 + 15% 随机干扰 + 最大空地回退
//   AI_HARD   —— BFS 寻食物 + 长蛇尾部可达性安全检测 + 最大空地回退
//
// 所有策略均使用 BFS 在模拟地图上评估每条候选路径的安全性。
// ============================================================================

#include "game.h"

#ifdef __cplusplus
extern "C" {
#endif

// 为 AI 蛇选择下一步方向。
// 参数：
//   game       —— 当前游戏状态（只读），AI 读取地图、蛇身、食物位置等信息
//   difficulty —— AI 难度等级，控制决策策略
// 返回值：
//   推荐的 Direction 值（UP / DOWN / LEFT / RIGHT）
//   若所有合法方向均不可达（理论上极少发生），返回 game->ai_direction
Direction ai_choose_direction(const GameState *game, AIDifficulty difficulty);

#ifdef __cplusplus
}
#endif

#endif // AI_H
