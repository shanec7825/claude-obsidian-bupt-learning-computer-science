---
type: concept
title: "Snake Game Curriculum — Zero to C++ Game Dev"
created: 2026-06-22
updated: 2026-06-22
tags:
  - curriculum
  - cpp
  - game-dev
  - education
  - easyx
  - snake
  - complete-course
status: complete
aliases:
  - "贪吃蛇课程"
  - "Snake Curriculum"
related:
  - "[[snake-game-easyx]]"
  - "[[snake-game-raylib]]"
  - "[[BFS Pathfinding for Game AI]]"
  - "[[EasyX Graphics Library]]"
  - "[[snake-game-call-graph]]"
  - "[[snake-game-final-design]]"
---

# Snake Game Curriculum -- 零基础 C++ 游戏开发课

> 一套用 C++ 贪吃蛇游戏做教学载体的零基础编程课程。8 章、30+ 课时，从安装开发环境到写出双人对战 + AI 寻路，附带完整头文件、逐章讲稿、每节练习和参考答案。

| 项目 | 说明 |
|------|------|
| 适用人群 | 编程零基础 / C++ 初学者 / 想用游戏项目学编程的学生 |
| 总时长 | 30-40 课时，每课时约 1.5-2 小时 |
| 最终产出 | 经典模式 + PvP 双人对战 + 人机 Battle 三模式 Snake 游戏 |
| 开发环境 | Windows + Visual Studio 2022 Community + EasyX |
| 前置要求 | 基本电脑操作能力，无编程经验要求 |

---

## 课程目录

### 第〇章 [[snake-curriculum/ch0-environment|环境配置]]
装好 Visual Studio 和 EasyX，跑通人生第一个图形程序。**2 课时。**
交付物：IDE 编译运行，红色圆出现在窗口中。

### 第一章 [[snake-curriculum/ch1-cpp-basics|C++ 快速入门]]
只学 Snake 源码用到的 C++，不贪多。**6 课时。**
交付物：能读懂 90% 语法 -- 变量、控制流、函数、数组、结构体、枚举、指针。

### 第二章 [[snake-curriculum/ch2-rendering|游戏框架与绘图]]
理解游戏主循环，用 EasyX 画出所有图形元素。**4 课时。**
交付物：窗口正常显示，双缓冲、网格渲染、场景切换。

### 第三章 [[snake-curriculum/ch3-movement|键盘输入与蛇移动]]
蛇能动 -- 转向、不掉头、不抽搐。**4 课时。**
交付物：按键上升沿检测、方向锁、tick 定时驱动移动。

### 第四章 [[snake-curriculum/ch4-game-logic|食物、碰撞与游戏模式]]
经典模式从头到尾完整可玩。**5 课时。**
交付物：随机食物放置、五层碰撞判定、尾巴安全规则、自动生长。

### 第五章 [[snake-curriculum/ch5-ai-pathfinding|BFS AI 寻路]]
手写 BFS 广度优先搜索，让 AI 对手"会思考"。**5 课时。**
交付物：BFS 寻路、Easy / Normal / Hard 三级 AI、尾巴可达性检查。

### 第六章 [[snake-curriculum/ch6-multiplayer|多玩家与战斗系统]]
PvP 和 Battle 模式全部完成。**4 课时。**
交付物：双蛇状态管理、同步碰撞、击杀奖励、场景状态机（多态）。

### 第七章 进阶方向 _（末章，内容嵌入本页，无独立讲稿）_
学完能做什么？技术深入、图形库升级、功能拓展、学习路线图。**2 课时。**

---

## 学习路线图

```
  ┌──────────────┐
  │  第〇章       │  环境配置（2 课时）
  └──────┬───────┘
         ▼
  ┌──────────────┐
  │  第一章       │  C++ 快速入门（6 课时）
  └──────┬───────┘
         ▼
  ┌──────────────┐
  │  第二章       │  框架与绘图（4 课时）
  └──────┬───────┘
         ▼
  ┌──────────────┐
  │  第三章       │  输入与移动（4 课时）
  └──────┬───────┘
         ▼
  ┌──────────────────────────────────┐
  │          第四章                   │  经典模式可玩（5 课时）
  └────────┬─────────────┬───────────┘
           ▼             ▼
  ┌────────────┐  ┌──────────────┐
  │  第五章     │  │  第六章       │
  │  AI 寻路    │  │  多人对战     │
  │  （5课时）  │  │  （4课时）    │
  └──────┬─────┘  └──────┬───────┘
         └───────┬───────┘
                 ▼
  ┌──────────────────────────────┐
  │  第七章  进阶方向（2 课时）   │
  └──────────────────────────────┘
```

> 第五章和第六章可**互换顺序**或**并行学习**。

---

## 使用方式

### 自学者
按顺序逐章学习，每节有练习和参考答案。讲稿里的代码自己敲一遍，遇到 bug 翻讲稿里的"常见坑"和 FAQ。每章结尾有里程碑检查点。

### 授课老师
每章讲稿可直接作课堂讲义（概念讲解 + 源码截取 + 板书示例 + 练习和答案）。建议每章间隔 1-2 天。第五章和第六章可根据班级情况调序。

### 速查表

| 想学什么 | 在哪一章 |
|----------|----------|
| 装 VS + EasyX | [[snake-curriculum/ch0-environment\|第〇章]] |
| 变量、循环、函数、数组、结构体 | [[snake-curriculum/ch1-cpp-basics\|第一章]] |
| 游戏循环、双缓冲、网格渲染 | [[snake-curriculum/ch2-rendering\|第二章]] |
| 按键检测、方向锁、蛇移动 | [[snake-curriculum/ch3-movement\|第三章]] |
| 食物放置、碰撞检测、尾巴安全 | [[snake-curriculum/ch4-game-logic\|第四章]] |
| BFS 寻路、三级 AI、可达性检查 | [[snake-curriculum/ch5-ai-pathfinding\|第五章]] |
| 双蛇管理、PvP、场景状态机、多态 | [[snake-curriculum/ch6-multiplayer\|第六章]] |
| 下一步学什么 | 第七章（本页末章） |

---

## 相关 Wiki 页面

- [[snake-game-easyx]] -- Snake 源码分析（项目主页）
- [[BFS Pathfinding for Game AI]] -- BFS 寻路算法深度解析
- [[EasyX Graphics Library]] -- EasyX 图形库参考
- [[snake-game-call-graph]] -- 源码函数调用关系图

---

## 课程文件清单

**头文件**（`.raw/snake-headers/`）：
`game.h`（状态结构体/枚举/常量）、`ai.h`（AI 寻路）、`input.h`（键盘输入）、`render.h`（渲染）、`scene.h`（场景基类/多态）

**讲稿**（`wiki/concepts/snake-curriculum/`）：

| 文件 | 内容 |
|------|------|
| `ch0-environment.md` | 第〇章：VS + EasyX 安装（417 行） |
| `ch1-cpp-basics.md` | 第一章：C++ 快速入门（1682 行） |
| `ch2-rendering.md` | 第二章：游戏循环、EasyX 绘图、双缓冲、场景系统 |
| `ch3-movement.md` | 第三章：按键检测、方向锁、蛇移动、tick 定时 |
| `ch4-game-logic.md` | 第四章：食物放置、碰撞检测、自动生长、经典模式组装 |
| `ch5-ai-pathfinding.md` | 第五章：BFS 原理、队列实现、三级 AI 难度、可达性检查 |
| `ch6-multiplayer.md` | 第六章：双蛇状态、同步碰撞、击杀奖励、场景状态机 |
