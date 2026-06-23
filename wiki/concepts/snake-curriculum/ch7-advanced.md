---
type: concept
title: "第七章：后续进阶方向"
created: 2026-06-22
tags: [curriculum, cpp, game-dev, roadmap, advanced]
status: developing
related:
  - "[[Snake Game Curriculum]]"
  - "[[BFS Pathfinding for Game AI]]"
  - "[[EasyX Graphics Library]]"
sessions: 2
duration: "3-4 小时"
goal: "知道学完 Snake 之后往哪走，怎么继续提升"
---

# 第七章：后续进阶方向

> **目标**：这一章不教你写新代码，而是给你一张"地图"——前面有什么路、每条路通向哪里、你怎么选。
> **总课时**：2 课时（约 3-4 小时）
> **前置要求**：前六章全部学完，手里有一个能跑的 Snake 项目

先花 30 秒想一个问题：你现在能写一个完整的贪吃蛇了——地图渲染、键盘输入、碰撞检测、AI 寻路、双人对战，全部跑通。那然后呢？

这就是第七章要回答的问题。我会把"然后"分成五段来讲：技术深挖、图形库升级、功能扩展、学习路线、推荐资源。你不必全看——挑你感兴趣的路线读，其他当参考就行。

说在前头：这一章没有"对错"。我给的每一条建议都是"你可以这么走"，不是"你必须这么走"。选你真正想做的方向，比选"看起来高级"的方向重要一百倍。

---

## 7.1 技术深挖

> **阅读时间**：约 30 分钟
> **适合谁**：不想止步于"能用"，想真正理解底层的人

前六章你写了很多代码，但你有没有一种感觉——有些东西"能用，但不知道为什么这样设计"？这一节就是补这个"为什么"的。

### 7.1.1 数据结构进阶 —— 从固定数组到手写链表

**为什么学这个？**

你现在蛇的身体是用固定数组存的：

```cpp
Point snake[MAX_SNAKE_LEN];  // 最多 500 节，够了，但……
```

能跑，但有个问题：`MAX_SNAKE_LEN` 是个常量。你说"地图就 20x20=400 格，蛇最多 400 节"。但如果以后你把地图改成 100x100 呢？或者你想做一个"无限长度"的贪吃蛇变体呢？数组的"固定上限"就成了天花板。

链表解决的就是这个问题：**长度完全动态，没有上限**。而且学链表的过程会逼你真正搞懂指针——动态分配、节点连接、遍历、释放，全是 C/C++ 的核心功夫。

**从 Snake 的哪个位置开始？**

把你现在 `game.h` 里的这行：

```cpp
Point snake[MAX_SNAKE_LEN];
```

改成你自己手写的链表。蛇身每个节点是一个 `struct Node`，包含一个 `Point` 和一个 `next` 指针：

```cpp
struct SnakeNode {
    Point pos;
    SnakeNode *next;
};

// 蛇用 head 指针和 length 维护
SnakeNode *snake_head = NULL;
int snake_length = 0;
```

你会发现 `move_snake()` 里的身体前移逻辑（现在是用 for 循环把 `snake[i] = snake[i-1]`）变成了链表的"头插尾删"——时间复杂度从 O(n) 变成 O(1)。这是你第一次直观感受到"数据结构选对了，性能就对了"。

**推荐学习路径**：

```
链表 → 栈/队列 → 哈希表 → 二叉树 → 图
```

你会发现在第五章 AI 寻路里你已经用过队列了（BFS 的 `std::queue`！）——但你自己实现过队列吗？底层是一个循环数组还是一个链表？现在你可以自己写了。

**推荐资源**：
- 《大话数据结构》（程杰）—— 中文，口语化，适合入门
- VisuAlgo（visualgo.net）—— 用动画看数据结构怎么运作，震撼
- LeetCode 上的链表/栈/队列标签题 —— 每学一种数据结构就刷 5 道题巩固

### 7.1.2 设计模式 —— 你已经在用了，只是不知道名字

**为什么学这个？**

在第六章你实现了 Scene 类的多态——`WelcomeScene`、`ClassicScene`、`BattleScene` 都继承自 `Scene`，主循环只调 `scene->update()` 和 `scene->render()`，不用管具体是哪个模式。这就是**状态模式**——对象的行为随内部状态改变而改变。

你不是"学了设计模式才用上"，而是"已经用上了，现在该回头给它一个名字"。

**具体学什么？**

从 Snake 出发，继续学这三种：

| 模式 | Snake 里在哪 | 为什么继续学 |
|------|-------------|------------|
| 观察者模式 | 你还没用——但它能彻底解耦你的代码 | 键盘输入通知蛇、碰撞通知 UI、分数变化通知面板——不用每帧都去轮询 |
| 命令模式 | 你还没用——但它能实现"撤销"和"回放" | 把每一步蛇的移动包装成 `MoveCommand`，存到一个队列里——你就能做**回放**功能，甚至做"时光倒流"道具 |
| 对象池 | 你还没用——但它能消除 new/delete 的开销 | 粒子系统、子弹、道具——高频创建销毁的对象，用对象池预分配复用 |

**推荐资源**：

《Game Programming Patterns》—— Robert Nystrom 著，**完全免费在线阅读**（gameprogrammingpatterns.com）。作者是 EA 和暴雪的前引擎程序员，每个模式都用一个游戏场景来解释。这本书和你在 Snake 里遇到的问题直接对应，是我见过最"对胃口"的设计模式书。

阅读建议：先看前三章（Command、Flyweight、Observer），再看 State（你已经会了，巩固一下），最后看 Object Pool。每一章独立，跳着读没问题。

### 7.1.3 算法进阶 —— BFS 之后的世界

**为什么学这个？**

第五章你写了 BFS，AI 蛇能在地图上找到最短路径了。但 BFS 是**盲目搜索**——它向四个方向均匀扩展，不管目标在哪个方向。就像你在一个大房间里找门，BFS 的做法是"以我为中心一圈一圈往外找"，而不是"门在右边我就往右找"。

A* 算法加了一个**启发函数**（heuristic）——告诉搜索"目标大概在哪个方向"。效果就是搜索范围大幅缩小，速度大幅加快。

**在 Snake 里对比 BFS 和 A\* 的效率**：

假设地图 20x20，蛇头在 (2,2)，食物在 (18,18)：

```
BFS 探索节点数：约 350+（几乎扫了整个地图）
A* 探索节点数：约 50-80（沿着对角线方向直奔目标）
```

你可以自己在 Snake 里加一个计数器，统计 BFS 每次搜索访问了多少格子，然后换成 A* 再跑一次。效率差异会让你直观理解"算法的力量"。

**A* 的伪代码**（对照你的 BFS 代码看）：

```cpp
// BFS 用的是普通队列，A* 用的是优先队列（按 f = g + h 排序）
//
// g = 从起点走到当前节点的步数（和 BFS 一样）
// h = 曼哈顿距离：|curr.row - food.row| + |curr.col - food.col|
// f = g + h  ← f 越小，优先级越高

priority_queue<Node> open_set;  // 替代 BFS 里的 queue
open_set.push(start, f = 0 + heuristic(start, goal));

while (!open_set.empty()) {
    Node curr = open_set.top();  // 取 f 值最小的，而非最先入队的
    open_set.pop();

    if (curr == goal) { /* 找到了！ */ break; }

    for (每个邻居 dir) {
        Node next = curr + dir;
        int g = curr.g + 1;
        int h = manhattan(next, goal);
        int f = g + h;
        open_set.push(next, f);
    }
}
```

关键区别就一行：BFS 从队列**头部**取（FIFO），A* 从优先队列**取 f 最小的**。其他逻辑几乎一样。

**继续学什么？**

```
BFS → A* → Dijkstra（边有权重时用）→ 博弈树搜索（Minimax）
```

博弈树尤其有意思——如果 Snake 是一个"零和博弈"（你和 AI 对抗，你吃的分 = AI 丢的分），AI 可以用 Minimax 算出"对我最有利的一步"。这就是 AlphaGo 的基础思路，只不过 Snake 简单得多，你手写就能理解。

**推荐资源**：
- Red Blob Games（redblobgames.com）—— 交互式路径算法教程，A* 那篇有动画演示
- LeetCode —— 按标签刷：BFS、DFS、图论、动态规划。从 Easy 开始，找"和 Snake 最像"的题（网格+最短路径）
- 如果你对竞赛感兴趣：洛谷 / Codeforces 的图论入门题单

### 7.1.4 性能优化 —— 你的 Snake 为什么"没必要优化"但仍然值得学

**为什么学这个？**

你的 Snake 每帧画 400 到 1225 个格子。在 EasyX 里，一帧大概几毫秒。以 60 FPS 算，你有 16ms 的预算，你的渲染大概只用了 1-2ms。所以你的 Snake **不需要优化**。

那我为什么还要讲这一节？因为"不需要优化"的小项目和"必须优化"的大项目之间，隔着的不是代码量，而是**优化思维**。学会在简单项目上练习优化，以后遇到复杂项目你就有了本能反应。

**优化思路一：脏矩形（Dirty Rectangle）**

现在你每帧调用 `draw_board()` 重画**所有**格子。但每一帧只有几个格子变了——蛇头往前一格，尾巴消失一格。其他 398 个格子画了也是白画。

脏矩形的思路：**只重画变了的部分**。

```cpp
// 当前做法 —— 每帧画全部
void draw_frame(GameState *game) {
    for (int r = 0; r < map_size; r++)
        for (int c = 0; c < map_size; c++)
            draw_cell(r, c, game->map[r][c]);  // 400 次！
}

// 脏矩形做法 —— 只画变了的部分
void draw_frame(GameState *game) {
    // 擦掉旧蛇头位置
    draw_cell(old_head.row, old_head.col, 'B');  // 1 次
    // 画新蛇头位置
    draw_cell(new_head.row, new_head.col, 'H');  // 1 次
    // 擦掉旧尾巴
    if (!will_grow)
        draw_cell(old_tail.row, old_tail.col, '.');  // 1 次
    // 总共 3 次，而不是 400 次
}
```

从 400 次降到 3 次——这就是优化思维的力量。

**优化思路二：空间分区（Spatial Partitioning）**

当蛇很长（比如 300 节），你在 `can_enter_cell()` 里每次都要遍历整个 `snake[]` 数组来判断某个格子是不是蛇身——O(n) 的线性搜索，n 是蛇的长度。

空间分区的做法：维护一个和地图一样大的 bool 数组 `occupied[20][20]`，蛇移动时同步更新它。查询"某个格子有没有蛇身"从 O(n) 变成 O(1)。

```cpp
// 优化前：遍历蛇身数组
bool is_snake_body(GameState *game, Point p) {
    for (int i = 1; i < game->length; i++)  // O(length)
        if (is_same_point(game->snake[i], p))
            return true;
    return false;
}

// 优化后：查哈希表
bool occupied[20][20] = {false};  // 和 map 同步维护
bool is_snake_body_fast(Point p) {
    return occupied[p.row][p.col];  // O(1)！
}
```

**性能分析工具**：

- Visual Studio 自带的**性能探查器**（Debug → Performance Profiler）—— 能告诉你每行代码花了多少时间
- 以后你写更复杂的东西（比如 A* 在大地图上跑），就可以用这个工具找瓶颈

---

## 7.2 升级图形库 —— EasyX 之后用什么

> **阅读时间**：约 30 分钟
> **适合谁**：想把游戏做得更好看、更跨平台、更接近工业标准的人

EasyX 很好——它是你第一个图形库，简单、中文文档丰富、短时间就能画出棋盘。但它有天花板：只支持 Windows、API 有限、社区小。

下面四个库，按"什么时候换"排列。每个我都会对比：同样写一个 Snake，代码量差多少、学习曲线多陡、什么场景下值得换。

### 7.2.1 四个库一览

| 库 | 一句话定位 | Snake 代码量 | 学习曲线 | 平台 |
|----|----------|-------------|---------|------|
| **SDL2** | C 语言的工业级多媒体库，Steam 上一大堆独立游戏用它 | +30%（主要是初始化代码） | 2 周入门 | Windows / macOS / Linux / iOS / Android |
| **SFML** | C++ 风格的游戏库，"像写 C++ 一样写游戏" | +20%（类封装更简洁） | 1 周入门 | Windows / macOS / Linux |
| **Raylib** | 为教学和原型设计的 C 库，API 极其清爽 | +10%（和 EasyX 最接近） | 3 天入门 | Windows / macOS / Linux / Web / Android |
| **OpenGL** | 如果你想自己做引擎，这是图形底层 | +200%（你得自己写一切——shader、矩阵、VAO/VBO） | 3 个月+ | 全平台 |

### 7.2.2 怎么选？

一个简单规则：

```
想做更多小游戏原型       → Raylib（最快出效果）
想认真做 2D 游戏          → SDL2 或 SFML（看你喜欢 C 还是 C++）
想做 3D                  → 先学 OpenGL 基础，再去碰 Unity/Unreal
想进游戏行业做引擎        → OpenGL → Vulkan → 自己写一个小引擎
```

我的个人建议：**先试 Raylib**。它和 EasyX 的思维模型最接近——你不需要理解渲染管线、不需要写 shader，调一个 `DrawRectangle()` 就能画格子。但它的功能比 EasyX 多得多：支持音频、支持纹理、支持 3D、能导出到 Web。你可以在一个周末把 Snake 从 EasyX 移植到 Raylib，然后立刻开始玩纹理、音频和粒子效果。

### 7.2.3 迁移策略：核心逻辑不用改

这里有个非常重要的认知，我希望能刻在你脑子里：

**移植图形库时，game.cpp 一行都不用动。**

为什么？因为你用了第六章讲的"关注点分离"——游戏逻辑（`move_snake`、`can_enter_cell`、`BFS`）和渲染（`draw_board`）是分开的。游戏逻辑只操作数据，不碰任何图形 API。

你需要改的只有两个文件：

```
render.cpp  →  重写绘图函数
input.cpp   →  重写键盘输入
```

其他文件（`game.cpp`、`ai.cpp`、`scene.cpp`）原封不动。这就是好架构的回报。

### 7.2.4 SDL2 迁移示例

下面用 SDL2 重写 `draw_board()`，对比 EasyX 版本。两个版本实现同一个功能——遍历地图，每个格子画一个颜色矩形。

**EasyX 版**（你现在用的）：

```cpp
void draw_board(GameState *game, int offset_x, int offset_y)
{
    for (int r = 0; r < game->map_size; r++)
    {
        for (int c = 0; c < game->map_size; c++)
        {
            int x = offset_x + c * CELL_SIZE;
            int y = offset_y + r * CELL_SIZE;

            switch (game->map[r][c])
            {
                case '.':  setfillcolor(RGB(40, 40, 40)); break;   // 空地
                case '#':  setfillcolor(RGB(60, 60, 60)); break;   // 墙
                case 'H':  setfillcolor(RGB(0, 255, 0));   break;  // 蛇头
                case 'B':  setfillcolor(RGB(0, 180, 0));   break;  // 蛇身
                case 'F':  setfillcolor(RGB(255, 80, 80)); break;  // 食物
                case 'O':  setfillcolor(RGB(100, 100, 100)); break;// 障碍物
            }

            fillrectangle(x, y, x + CELL_SIZE - 1, y + CELL_SIZE - 1);
        }
    }
}
```

**SDL2 版**（改写后）：

```cpp
#include <SDL2/SDL.h>

void draw_board(SDL_Renderer *renderer, GameState *game,
                int offset_x, int offset_y)
{
    SDL_Rect rect;
    rect.w = CELL_SIZE;
    rect.h = CELL_SIZE;

    for (int r = 0; r < game->map_size; r++)
    {
        for (int c = 0; c < game->map_size; c++)
        {
            rect.x = offset_x + c * CELL_SIZE;
            rect.y = offset_y + r * CELL_SIZE;

            switch (game->map[r][c])
            {
                case '.':  SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);   break;
                case '#':  SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);   break;
                case 'H':  SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);    break;
                case 'B':  SDL_SetRenderDrawColor(renderer, 0, 180, 0, 255);    break;
                case 'F':  SDL_SetRenderDrawColor(renderer, 255, 80, 80, 255);  break;
                case 'O':  SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); break;
            }

            SDL_RenderFillRect(renderer, &rect);
        }
    }
}
```

看到了吗？结构几乎一模一样——`setfillcolor` 换成 `SDL_SetRenderDrawColor`，`fillrectangle` 换成 `SDL_RenderFillRect`。所有判断逻辑（`switch`、颜色值）原封不动。移植的核心工作是"API 名称替换"，而你的游戏逻辑完全不受影响。

初始化部分确实会多一些（SDL2 需要 `SDL_Init`、`SDL_CreateWindow`、`SDL_CreateRenderer` 三步），但那是写一次就忘的样板代码。

---

## 7.3 游戏功能扩展 —— 你的"功能菜单"

> **阅读时间**：约 20 分钟
> **适合谁**：不想止步于课程 Snake，想加点"自己的东西"的人

下面是八个扩展方向，按难度从低到高排列。每个都标注了：描述、涉及的知识点、难度、从哪个位置开始改。挑你喜欢的试——不需要全做。

### 扩展菜单

| # | 功能 | 涉及知识点 | 难度 | 从哪里切入 |
|---|------|-----------|------|-----------|
| 1 | **鼠标操控**：点哪蛇去哪，或者鼠标拖拽方向 | Win32 消息处理（`WM_MOUSEMOVE`）、向量计算 | ⭐ | `input.cpp`：新增 `process_mouse()` 函数，在主循环里调用 |
| 2 | **计分板**：保存最高分到文件，启动时读取显示 | 文件 IO（`fopen`/`fprintf`/`fscanf`） | ⭐ | `game.cpp`：死亡时写文件；`scene.cpp`：菜单里显示历史最高分 |
| 3 | **暂停菜单 / 保存读档**：按 ESC 弹出菜单，能存档读档 | 文件 IO + 序列化（把 `GameState` 写成二进制） | ⭐⭐ | `scene.cpp`：新增 `PauseScene`；`game.cpp`：新增 `save_game()` 和 `load_game()` |
| 4 | **道具系统**：加速、减速、穿墙、隐形、双倍分数 | 状态管理（给蛇增加 `power_ups[]` 数组 + 计时器） | ⭐⭐ | `game.h`：新增 `PowerUp` 枚举和计时字段；`move_snake()` 里根据道具修改行为 |
| 5 | **地图编辑器**：用鼠标点击放置墙/障碍物，保存为自定义地图 | 文件 IO + 可视化交互 | ⭐⭐ | 新建 `editor.cpp`：独立的地图编辑模式，输出 `.map` 文件；`init_game()` 里加载自定义地图 |
| 6 | **音效 / 背景音乐**：吃食物有声音、死了有声音 | EasyX 不自带音频 → 需引入额外库（SDL2_mixer 或 Win32 `PlaySound`） | ⭐⭐ | 新建 `audio.cpp`：封装所有音效播放；在 `move_snake()` 和游戏结束处调用 |
| 7 | **粒子效果**：蛇死亡时爆开成粒子动画 | EasyX 动画（多帧绘制 + 物理模拟） | ⭐⭐ | 新建 `particle.cpp`：定义粒子系统和物理更新；`draw_game()` 里渲染粒子层 |
| 8 | **联网对战**：两台电脑通过局域网对战 | Socket 编程（`Winsock2`）、网络同步、延迟处理 | ⭐⭐⭐⭐ | 这是个完整的模块——需要客户端/服务器架构，把键盘输入通过网络发送，把对方蛇状态收回来渲染。建议先做一个最简单的版本：一方做服务器，另一方用 IP 连接，用 TCP 传输方向指令 |

### 为什么不建议一上来就做联网对战？

联网对战看起来最酷，但它踩的坑和"游戏编程"没什么关系——而是"网络编程"的坑：丢包怎么办、延迟怎么办、两台机器的帧率不一样怎么办、一方卡了另一方要不要等？这些问题每一个都能花掉你一周时间。

我的建议：**先把前七个都试一两个**。等你对自己写的 Snake 的每一行代码都了如指掌了，再碰网络。那时候你遇到网络问题，至少不用同时纠结"是不是我游戏逻辑写错了"。

---

## 7.4 学习路线图 —— 三条路你走哪条

> **阅读时间**：约 15 分钟
> **适合谁**：需要明确方向，不想"什么都想学但什么都只学了开头"

学完 Snake 之后你站在一个路口。前面至少有三条路，没有哪条比哪条更好——看你想去哪。

### 路线一：想做更多游戏（游戏开发路线）

```
你现在在这里 → SDL2 / SFML / Raylib 入门
                    │  耗时：1-2 周，挑一个库把 Snake 移植过去
                    ▼
              做 3-5 个小游戏
              ├── 贪吃蛇（已做 ✓）
              ├── 俄罗斯方块（练状态机 + 碰撞检测）
              ├── 打砖块（练物理反弹 + 道具系统）
              ├── 太空侵略者（练子弹管理 + 对象池）
              └── 平台跳跃（练关卡加载 + 角色控制）
                    │  耗时：每个 1-3 周，边做边学
                    ▼
              参加 Game Jam（Ludum Dare / Global Game Jam）
              └── 48 小时做一款游戏，检验你所有能力
                    │  耗时：3-6 个月到能参加
                    ▼
              Unity / Unreal / Godot
              └── 系统学习引擎：场景管理、物理引擎、动画系统
                    │  耗时：1-2 年
                    ▼
              游戏行业实习 / 全职
```

**预估时间**：3-6 个月到能参加 Game Jam；1-2 年到能找到实习。

### 路线二：想做算法 / 竞赛（算法竞赛路线）

```
你现在在这里 → LeetCode 刷题
              ├── BFS / DFS（你已经在 Snake 里写过了！）
              ├── 图论（最短路、拓扑排序、并查集）
              ├── 动态规划（背包、区间 DP、状态压缩）
              └── 贪心 / 二分
                    │  耗时：每天 1-2 题，3 个月能覆盖基础
                    ▼
          A* / Dijkstra / 博弈树（Minimax → Alpha-Beta 剪枝）
              └── 在 Snake 里实战：让 AI 更强
                    │
                    ▼
          参加算法竞赛
          ├── 蓝桥杯（中国，入门友好）
          ├── ACM-ICPC（国际，团队，难度最高）
          └── Google Code Jam / Meta Hacker Cup
                    │  耗时：6 个月-1 年到能拿奖
                    ▼
              大厂算法面试 / 保研机试
```

**预估时间**：3 个月到能通过中等难度的面试算法题；6 个月到能在竞赛里拿名次。

### 路线三：想深入 C++（语言深度路线）

```
你现在在这里 → 《Effective C++》（Scott Meyers）
              └── 55 个条款，每个都是一条"血的教训"
                    │  耗时：1-2 个月精读
                    ▼
          现代 C++（C++11 / 14 / 17 / 20 / 23）
          ├── 智能指针（unique_ptr / shared_ptr）
          ├── Lambda 表达式
          ├── 移动语义（move semantics）—— 告别不必要的拷贝
          ├── constexpr 编译期计算
          ├── STL 算法库（<algorithm> 里的宝藏）
          └── Concepts（C++20）—— 模板的"类型约束"
                    │  耗时：3-6 个月，选一个特性深入
                    ▼
          《C++ Concurrency in Action》
          └── 多线程编程：你的 Snake 可以同时运行多条蛇的 AI
                    │
                    ▼
          模板元编程（Template Metaprogramming）
          ├── 编译期计算、类型萃取、SFINAE
          └── 这是 C++ 最深的角落，学了它你再看任何 C++ 代码都不怕
                    │  耗时：1 年以上
                    ▼
          参与开源项目（LLVM / Chromium / Unreal Engine）
          读 C++ 标准提案 → C++ 标准委员会
```

**预估时间**：1 年能自信地用现代 C++ 写项目；2-3 年能参与大型开源项目。

### 三条路不是互斥的

我知道你会想——"能不能三条都走？"

答案是：能，但不是同时。没有人能同时刷 LeetCode、写 Game Jam、深读 C++ 标准，一天只有 24 小时。

建议：**先选一条路走 3 个月**。3 个月后回头看看，你对这条路是真的有热情，还是"好像也没那么有意思"。然后你可以换一条路，或者把两条路结合——比如"用现代 C++ 特性重写 Snake 的 AI 模块"就把路线二和路线三串起来了。

真正厉害的人不是"同时走三条路"的人，而是"走完一条路再走第二条，两条路之间找到交点的"人。

---

## 7.5 推荐资源 —— 我用过觉得好的

> **阅读时间**：约 15 分钟

最后一部分，不讲道理了，直接列书单和资源。每一条都是我自己翻过或者身边朋友推荐的。标注了：难度、语言、是否免费。

### 入门书（适合刚学完这个课程的）

| 书名 | 作者 | 语言 | 付费 | 说明 |
|------|------|------|------|------|
| 《C++ Primer Plus》（第 6 版） | Stephen Prata | 中文译本 | 付费 | 从变量讲到模板，适合当字典——哪里不会查哪里 |
| 《啊哈！算法》 | 啊哈磊 | 中文 | 付费 | 用故事讲算法，配全彩插图，适合初中级 |

### C++ 进阶书

| 书名 | 难度 | 说明 |
|------|------|------|
| 《Effective C++》 | 中级 | C++ 程序员的"行为准则"。读完之后你会开始嫌弃自己以前写的代码 |
| 《Effective Modern C++》 | 中高级 | C++11/14 的 42 个条款。如果你要写现代 C++，这本必读 |
| 《C++ Concurrency in Action》 | 高级 | C++ 多线程的圣经。写高性能应用之前先读这本 |
| 《A Tour of C++》 | 入门-中级 | Bjarne Stroustrup（C++ 之父）写的 200 页精简版。如果你只想读一本 C++ 书，读这本 |

### 游戏开发书

| 书名 | 难度 | 说明 |
|------|------|------|
| 《Game Programming Patterns》 | 中级 | **免费**（gameprogrammingpatterns.com）。模式 + 游戏场景，和 Snake 课程直接衔接 |
| 《游戏引擎架构》 | 高级 | 1200 页，原名 *Game Engine Architecture*。讲一个完整游戏引擎的每一个子系统。建议有 1-2 年游戏开发经验后再读 |

### 算法 / 面试

| 资源 | 说明 |
|------|------|
| LeetCode（leetcode.cn / leetcode.com） | 按标签刷：BFS、DFS、动态规划、图论。先做 Easy，再 Medium |
| 《算法导论》（CLRS） | 经典但厚，当参考书查，不要试图通读 |
| 牛客网（nowcoder.com） | 国内公司的笔试真题，准备面试用 |

### 视频 / YouTube 频道

| 频道 | 说明 |
|------|------|
| **The Cherno** | C++ 系列是最好的视频 C++ 教程之一。从指针到 OpenGL，覆盖面极广 |
| **javidx9** (OneLoneCoder) | 用 C++ 和控制台做小游戏（俄罗斯方块、平台跳跃、3D 引擎）。他的 ConsoleGameEngine 和 Snake 课程的精神一脉相承 |
| **3Blue1Brown** | 数学和算法可视化的天花板。看他的神经网络和线性代数系列，你会重新理解你写过的代码 |
| **GDC（Game Developers Conference）** | GDC Vault 上有大量免费讲座——真正的业界老兵分享实战经验 |

### 社区

| 社区 | 说明 |
|------|------|
| **r/cpp** (Reddit) | C++ 社区，标准委员会的讨论也在这里 |
| **r/gamedev** (Reddit) | 独立游戏开发者社区 |
| **CppCon / C++Now** (YouTube) | 每年的 C++ 大会演讲录像，全部免费。搜"CppCon Back to Basics"系列入门 |
| **Ludum Dare** (ldjam.com) | 最大的线上 Game Jam，每年两次。去参加一次，你会成长巨快 |

### 值得复刻的经典游戏（从易到难）

这个列表不是给玩家玩的——是给你**复刻**的。每复刻一个游戏，你都在练不同的知识点。

| 游戏 | 难度 | 主要练什么 |
|------|------|-----------|
| **俄罗斯方块** | ⭐⭐ | 状态机、旋转矩阵、消行逻辑 |
| **打砖块（Breakout）** | ⭐⭐ | 碰撞反弹物理、道具系统、关卡加载 |
| **扫雷** | ⭐⭐ | 递归展开、二维数组操作 |
| **Flappy Bird** | ⭐⭐ | 简单物理、碰撞检测、帧独立性 |
| **太空侵略者（Space Invaders）** | ⭐⭐⭐ | 子弹管理（对象池）、敌群 AI（编队移动）、逐关难度递增 |
| **吃豆人（Pac-Man）** | ⭐⭐⭐ | AI 追逐/逃跑行为、地图图论、动画状态机 |
| **超级玛丽（第一关）** | ⭐⭐⭐⭐ | 角色控制器（加速度/跳跃/重力）、关卡编辑器、摄像机跟随 |
| **Agar.io 简化版** | ⭐⭐⭐⭐ | 网络同步、实体插值、排行榜系统 |

我的建议：**先复刻俄罗斯方块**。它和 Snake 一样是网格+回合制思维，但多了消除逻辑和旋转。做完俄罗斯方块后，你对"网格游戏"的理解会上升一个层次。

---

## 7.6 结业寄语

恭喜你——如果你一字一句学完了前六章，你已经完成了一次从"什么都不会"到"能写一个完整游戏"的旅程。我算了一下：你写了至少 8 个 `.cpp` 文件、完成了三个游戏模式、手写了 BFS 寻路算法、理解了多态和场景管理。在任何一所大学的编程入门课里，这个项目量至少够两个大作业。

但更重要的不是这些代码。而是你现在拥有了一个**可以被审视的作品**——一个你自己能从头解释的游戏。当面试官问"你做过什么项目"，你可以打开 VS，跑起 Snake，说："这个。我从地图渲染到 AI 寻路全部手写的。你要不要看看我的碰撞检测逻辑？" 这句话比简历上写一百个"熟悉 C++"都有用。

从下一行开始，代码是你的了。地图是你的了。方向也是你的了。

选一条路，走下去。

我保证前面有比 Snake 好玩一百倍的东西在等你。

---

## 本章回顾

这一章没有要背的知识点，只有一张"地图"：

| 节 | 主题 | 核心内容 |
|----|------|---------|
| 7.1 | 技术深挖 | 数据结构（链表）、设计模式（State/Observer/Command）、算法（A*）、性能优化（脏矩形/空间分区） |
| 7.2 | 升级图形库 | SDL2 / SFML / Raylib / OpenGL 对比；迁移策略（只改 render.cpp 和 input.cpp） |
| 7.3 | 游戏功能扩展 | 8 个扩展方向，从鼠标操控（⭐）到联网对战（⭐⭐⭐⭐） |
| 7.4 | 学习路线图 | 三条路径：游戏开发 / 算法竞赛 / C++ 深度，每条都标了时间预估 |
| 7.5 | 推荐资源 | 书、视频、社区、复刻游戏列表——按难度和类别分类 |
| 7.6 | 结业寄语 | 你走了多远，前面有什么 |
