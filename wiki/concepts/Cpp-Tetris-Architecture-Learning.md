---
type: concept
title: "Cpp-Tetris-Architecture-Learning"
created: 2026-06-29
updated: 2026-06-29
tags:
  - cpp
  - architecture
  - game
  - tetris
  - snake
  - learning-progression
  - design-patterns
status: developing
address: c-000005
related:
  - "[[cpp-tetris-raylib]]"
  - "[[snake-game-raylib]]"
  - "[[Scene State Machine Pattern]]"
  - "[[GitHub Repo Learning Evaluation Checklist]]"
  - "[[Code Reading as Learning Strategy]]"
sources:
  - "[[cpp-tetris-raylib]]"
---

# Cpp-Tetris-Architecture-Learning

> [!summary] 自写摘要
> Snake 和 Tetris 使用相同的技术栈（raylib + C++）但采用不同的架构模式。Snake 使用 Scene 多态 + GameState 数据结构驱动，Tetris 使用经典 Block 继承树 + Game 编排器。对比这两个架构是理解"同一技术栈、不同设计"的最佳入门案例。从 Snake 到 Tetris 是自然的复杂度升级：新增继承、多对象管理、状态循环旋转。

---

## Snake vs Tetris：架构对比

两个项目都在同一个技术平台上：raylib + C++ + Makefile。但它们的内部组织方式完全不同。

| 维度       | Snake（你的项目）                     | Tetris（educ8s）           |
| -------- | ------------------------------- | ------------------------ |
| **入口**   | `main.cpp` → Scene 切换           | `main.cpp` → Game 循环     |
| **核心抽象** | Scene 多态（MenuScene / PlayScene） | Block 继承树（Block → 7 子类）  |
| **游戏状态** | 集中式 `GameState` 结构体             | 分散在 Grid + Game 类中       |
| **渲染**   | 独立 `render.cpp` + 各 Scene       | Game::Draw() 内部渲染        |
| **输入**   | `input.cpp`，边沿触发                | Game::HandleInput()，即时响应 |
| **数据建模** | 蛇用 `deque<Point>`               | 方块用 4 个旋转状态的硬编码单元格       |
| **碰撞检测** | 11 步优先级决策树                      | 边界 + 单元格占用检查             |
| **音频**   | PCM 程序化合成                       | raylib 音频文件播放            |

## 两种架构模式

### Snake 的模式：Scene 状态机 + 数据驱动

```cpp
// Snake 的核心：场景切换
class Scene {
    virtual void update(GameState& state) = 0;
    virtual void render(const GameState& state) = 0;
};
// MenuScene, PlayScene 继承 Scene
// GameState 是所有场景共享的数据载体
```

**特点**: 
- 多态用于**行为切换**（菜单 vs 游戏 vs 游戏结束）
- 数据与行为分离（GameState 是纯数据，Scene 是纯行为）
- 适合有多种模式/界面的游戏

### Tetris 的模式：Block 继承树 + 编排器

```cpp
// Tetris 的核心：方块多态
class Block {
    vector<Position> cells[4];  // 4 个旋转状态
    virtual void draw() = 0;
};
// LBlock, IBlock, TBlock 等 7 个子类
// Game 类编排一切
```

**特点**:
- 多态用于**数据多态**（不同方块形状不同）
- 数据与行为耦合（每个 Block 子类自带形状数据 + 渲染行为）
- 适合同类对象有多种变体的游戏

## 从 Snake 到 Tetris 的学习路径

```
Snake（已掌握）
  ├── 场景切换（Scene 多态）
  ├── GameState 数据结构
  ├── BFS 寻路
  ├── 边沿触发输入
  └── 程序化音效
       │
       ▼ 迁移到 Tetris 时——
       │
Tetris（新概念）
  ├── 经典继承树（Block → 7 子类） ← 新增
  ├── Grid 二维数组建模 ← 新增（Snake 用 deque）
  ├── 旋转状态循环 ← 新增（无需数学）
  ├── 洗牌袋随机算法 ← 新增
  └── 消行 + 计分 ← 新增（Snake 无此机制）
```

## 为什么这个对比有价值

这是理解 **"同一技术栈、不同架构"** 的最小可行案例：

1. **相同点让你不恐慌**：Makefile 构建、raylib 渲染循环、C++ 语法——都是熟悉的
2. **不同点让你学新东西**：继承树、Grid 建模、状态循环旋转——每个都是新的概念
3. **对比让你理解设计选择**：为什么 Snake 用 Scene 多态而 Tetris 用 Block 继承？不是哪个更好——是**需求不同**

> [!key-insight] 核心洞察
> 从一个项目学到的架构模式不代表"正确做法"。同一个技术栈可以有不同的设计。Snake 的 Scene 多态和 Tetris 的 Block 继承都是合理的——它们服务于不同的游戏需求。真正的工程能力不是记住哪种模式"对"，而是理解**什么时候用哪种**。

## 学习建议

按 educ8s 推荐的顺序：Pong → Snake → Tetris。你已经在第二步和第三步之间。

1. **不要跳过 Snake 到 Tetris 的过渡**：这个过渡教你的东西比单独学任何一个项目都多
2. **先跑通 Tetris，再读代码**：看到游戏"活了"会大幅降低挫败感（见 [[Code Reading as Learning Strategy]]）
3. **读的时候刻意对比**：每读一个 Tetris 的类，想一下 Snake 里对应的部分是怎么做的
4. **继承是新增的复杂度**：Snake 用的是组合/多态（Scene），Tetris 用的是经典继承——这是你第一次接触两种多态模式的区别
