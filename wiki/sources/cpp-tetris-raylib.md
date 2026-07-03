---
type: source
title: "C++ Tetris Game (raylib) — educ8s 教程项目"
created: 2026-06-29
updated: 2026-06-29
tags:
  - source
  - cpp
  - game
  - raylib
  - tetris
  - tutorial
  - beginner
status: current
source_type: github-repo
source_url: https://github.com/educ8s/Cpp-Tetris-Game-with-raylib
repo_author: educ8s
repo_stars: 113
address: c-000003
related:
  - "[[snake-game-raylib]]"
  - "[[Scene State Machine Pattern]]"
  - "[[GitHub Repo Learning Evaluation Checklist]]"
  - "[[Code Reading as Learning Strategy]]"
  - "[[Snake Game Curriculum]]"
pages_created:
  - "[[Cpp-Tetris-Architecture-Learning]]"
---

# Source: C++ Tetris Game (raylib)

**Type**: GitHub 仓库（C++ 100%, 113 stars, MIT License）
**Date**: 2026-06-29
**Author**: educ8s（YouTube 教程频道）
**Tutorial**: [youtu.be/wVYKG_ch4yM](https://youtu.be/wVYKG_ch4yM)
**Platform**: Windows, macOS, Linux

## Summary

一个完整的俄罗斯方块游戏，使用 C++ 和 raylib 构建。包含全部 7 种方块、旋转、碰撞检测、消行、计分、下一块预览、背景音乐和音效。**每行代码都有视频教程讲解**——这是它与纯源码仓库的关键区别。

仓库作者推荐先看 raylib 安装教程，并以 Pong 和 Snake 教程作为热身，再做这个项目。

## 架构概览

```
educ8s/Cpp-Tetris-Game-with-raylib
│
├── Grid 类（20×10 二维整数数组）
│   └── 0 = 空, 1-7 = 对应颜色方块
│
├── Block 基类 → 7 个子类
│   ├── LBlock, IBlock, TBlock, ...
│   └── 每个子类存储 4 个旋转状态的单元格位置
│
├── Game 类（编排器）
│   ├── 持有 Grid、当前 Block、下一个 Block
│   ├── HandleInput() — 处理键盘输入
│   └── Draw() — 渲染游戏画面
│
└── main.cpp（游戏循环）
    └── UpdateDrawFrame() → Game::HandleInput() + Game::Draw()
```

## 关键设计决策

| 决策 | 做法 | 替代方案 | 评价 |
|------|------|---------|------|
| 旋转实现 | 状态循环（rotationState 索引） | 数学矩阵变换 | 新手友好，但代码量大（每个状态硬编码） |
| 方块随机 | 洗牌袋（7 个各出现一次再洗） | 纯随机 | 经典俄罗斯方块行为，避免连续重复 |
| 游戏结束 | 新生方块与已锁定格子重叠 | 顶部触碰检测 | 简单直观 |
| 构建系统 | Makefile（单行构建） | CMake | 极简，适合新手 |

## 12 步教程路径

| 步骤  | 主题                  | 学习的编程概念                         |
| --- | ------------------- | ------------------------------- |
| 1   | Game loop & window  | raylib 窗口创建、帧率控制、update/draw 循环 |
| 2   | Grid class          | 二维数组建模、颜色编码渲染                   |
| 3   | Block classes       | 数据建模、多状态表示、bounding-grid 方法     |
| 4   | Inheritance         | 基类/派生类、虚函数、多态                   |
| 5   | Movement & rotation | 键盘输入、状态机旋转、无效操作回滚               |
| 6   | Collision detection | 边界检查、单元格占用验证                    |
| 7   | Block locking       | 状态转换、新方块生成时机                    |
| 8   | Row clearing        | 数组扫描、行位移                        |
| 9   | Scoring             | 计分逻辑、不同操作的权重                    |
| 10  | UI                  | 界面渲染、信息面板                       |
| 11  | Audio               | raylib 音频 API、循环播放、事件触发音效       |
| 12  | Game reset          | 状态重置、重启逻辑                       |

## 新手友好度评估

按 [[GitHub Repo Learning Evaluation Checklist]] 评分：

| 评分项 | 得分 | 说明 |
|--------|------|------|
| 文件数量 | 3 | src/ 目录下清晰组织，入口明确 |
| 代码注释 | 2 | 有视频讲解但代码注释不多 |
| 命名质量 | 2 | 类名清晰（LBlock, IBlock），变量名可改进 |
| 函数长度 | 2 | 部分函数较长（教程风格） |
| 测试 | 1 | 无测试文件 |
| 构建简单度 | 3 | Makefile 单行构建 |
| 技术栈匹配 | 3 | raylib + C++，与蛇项目 100% 匹配 |
| 难度梯度 | 3 | 从 Snake 过渡到 Tetris 是自然的复杂度升级 |
| 视觉反馈 | 3 | 图形游戏，立即可见 |
| 教学支持 | 3 | **每行代码有视频讲解** |

**综合推荐指数：83/100 → 🥇 强烈推荐**

## 与 Snake Game 的关系

参见 [[Cpp-Tetris-Architecture-Learning]] 的详细架构对比。

- **技术栈相同**：raylib + C++，构建工具一致（Makefile）
- **复杂度升级**：单一游戏对象（蛇）→ 多游戏对象（7 种方块 + Grid）
- **新概念**：继承（Snake 用组合/多态，Tetris 用经典继承树）、状态循环旋转
- **作者推荐的顺序**：Pong → Snake → Tetris（你就是在这个路径上）
