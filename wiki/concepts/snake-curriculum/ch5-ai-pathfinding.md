---
type: concept
title: "第五章：AI 寻路——BFS 算法"
created: 2026-06-22
tags: [curriculum, cpp, game-dev, ai, bfs, algorithm]
status: developing
related:
  - "[[Snake Game Curriculum]]"
  - "[[BFS Pathfinding for Game AI]]"
  - "[[snake-game-call-graph]]"
sessions: 5
duration: "8-10 小时"
goal: "理解 BFS 广度优先搜索，理解三级 AI 难度怎么实现的"
---

# 第五章：AI 寻路——BFS 算法

> **目标**：学完本章后，你不仅知道贪吃蛇 AI 怎么"思考"，还能自己写出 BFS 算法，并且理解为什么 Easy / Normal / Hard 三种难度会让你在游戏里感觉到完全不同的对手。
> **总课时**：5 课时（约 8-10 小时）
> **前置要求**：第四章（经典模式已通关，能看懂控制流、数组操作、函数和指针）

这一章是整个课程最"算法"的一章，也是最有成就感的一章。你会发现，那些看似聪明的游戏 AI，核心其实就是一个叫 BFS 的算法。说白了就是"一圈一圈往外找"，跟你在商场找厕所的思路一模一样。

更关键的是，BFS 不是只给游戏用的。它是算法入门的敲门砖——学完 BFS，后续的 DFS、Dijkstra、A* 都会变得触手可及。所以这一章请你慢下来，每个练习都动手写一遍，不要光看。

---

## 5.1 什么是 BFS（1课时） 

> **课时**：1 课时（约 1.5-2 小时）
> **目标**：能用手算的方式完成一个陌生网格的 BFS 距离计算，理解为什么 BFS 能找到最短路径

### 5.1.1 在商场里找厕所——两种找法

假设你在一个陌生的商场一楼，突然肚子不舒服，要找最近的厕所。你会怎么找？

**方法一：沿着一条路走到黑。**
你看到左边有条走廊，于是一直往里走。走到尽头发现是死路，退回来。又看到右边有个通道，走进去绕了一大圈，结果发现厕所其实就在你出发点背后三步远的地方——但你已经绕了整个商场半圈。

这就是 **DFS（深度优先搜索）**。"深度"的意思是一条路走到尽头再回头。运气好可能很快找到，运气不好就绕远路。

**方法二：一圈一圈往外找。**
你站在出发点，先看紧挨着自己的四个方向（前、后、左、右）有没有厕所。没有？那再看"两步远"的所有位置。还没有？三步远、四步远……像水波纹一样，一圈一圈往外扩散。

这就是 **BFS（广度优先搜索）**。"广度"的意思是先把近的地方全部看完，再看更远的。因为你是一层一层往外找的，所以**第一个找到的厕所，一定就是最近的厕所**。

### 5.1.2 BFS 的"水波纹"类比

想象一颗石子扔进池塘，波纹从中心往外扩散：

```
第 0 圈：石子落点（距离 = 0）
第 1 圈：紧挨着的四个方向（距离 = 1）
第 2 圈：再往外一圈（距离 = 2）
第 3 圈：再往外一圈（距离 = 3）
...
```

BFS 做的事情就是把食物当作石子落点（距离为 0），然后往外扩散，给地图上每个能走的格子标上一个数字——"从这里走到食物需要几步"。

### 5.1.3 5x5 网格 BFS 逐步演示

我们用一张 5x5 的地图来演示。假设食物在中心位置 (2, 2)，墙 `#` 围住边界。

**初始状态**（所有空地标为 `?`，代表距离未知）：

```
  # # # # #
  # ? ? ? #
  # ? F ? #
  # ? ? ? #
  # # # # #
```

**步骤 1**：食物位置 `F` 距离 = 0。

```
  # # # # #
  # ? ? ? #
  # ? 0 ? #
  # ? ? ? #
  # # # # #
```

**步骤 2**：从 0 往外扩散一圈——上下左右四个邻居距离 = 1。

```
  # # # # #
  # ? 1 ? #
  # 1 0 1 #
  # ? 1 ? #
  # # # # #
```

**步骤 3**：从所有 1 再往外扩散——每个 1 的邻居如果还没数字，标为 2。

```
  # # # # #
  # 2 1 2 #
  # 1 0 1 #
  # 2 1 2 #
  # # # # #
```

**完成！** 数字网格里的每一个值，就是从那个格子走到食物的**最短步数**。

**有障碍物的情况**（加入一堵墙 `O`）：

```
  # # # # #
  # 3 2 1 #
  # O 1 0 #
  # 3 2 1 #
  # # # # #
```

食物右边的格子明明是 (2, 3)，为什么没有标 1？因为它右边是墙 `#`。同样，(1, 1) 是障碍物 `O`，不能走，所以它左上方的 (0, 0) 和左上方的格子只能绕路，距离变成了更远的数字——BFS 自动绕开了障碍物。

这就是曼哈顿距离做不到的事情——曼哈顿距离只看坐标差 `|r2-r1| + |c2-c1|`，它会"穿墙"，以为墙后面的格子很近。**BFS 不会穿墙，因为它只走实际能走的格子**。

### 5.1.4 为什么 BFS 适合贪吃蛇？

贪吃蛇每走一步，所有格子的代价都一样——都是 1 步。这种情况叫"无权图"（所有边的权重相同）。

对于无权图，BFS 找到的一定是**最短路径**。证明的思路很简单：BFS 是逐层遍历的。如果一个格子在第 k 层被访问到，意味着从起点到它需要走 k 步，而且不可能有更短的路径（因为如果有，它早就应该在第 k-1 层或更早的层被访问到了）。

所以对于 Snake：
- **BFS 找到的第一条通往食物的路径，一定是步数最少的路径。**
- 步数最少 = 蛇用最快的速度吃到食物 = 不容易把自己绕死。

### 5.1.5 曼哈顿距离 vs BFS：什么时候够用？

Easy AI 用的是曼哈顿距离，Normal / Hard AI 用的是 BFS。那么问题来了：什么时候曼哈顿够用，什么时候必须 BFS？

**曼哈顿距离**：`|头.row - 食物.row| + |头.col - 食物.col|`

```
空地图（没有障碍物，蛇很短）：
  曼哈顿距离 = 实际最短距离 ✓ 够用

有障碍物 / 蛇很长占了很多格子：
  曼哈顿距离 < 实际距离（因为曼哈顿会"穿墙"） ✗ 不够用
  蛇按曼哈顿方向走，走到障碍物前面发现过不去，
  但已经浪费了好几步，可能把自己困死了
```

一个具体的例子：

```
  # # # # # # #
  # . . . . . #
  # . . O . . #
  # H O . . F #
  # . . O . . #
  # . . . . . #
  # # # # # # #
```

蛇头 `H` 在 (3, 1)，食物 `F` 在 (3, 5)。曼哈顿距离 = `|3-3| + |1-5|` = 4。但实际能走的路要绕开障碍物——可能要走 6 步甚至更多。

Easy AI 在这种情况下就会被障碍物"骗到"——它朝着食物的方向走，走到障碍物前卡住，如果附近没有别的可走方向，就死了。Normal / Hard AI 用 BFS，能算出真实的绕路距离，不会被障碍物迷惑。

### 5.1.6 练一练

**题目**：在下面这个 6x6 网格中（边界是墙 `#`，`O` 是障碍物），食物在 (1, 1)，请用手算的方式，给每个空地填上 BFS 距离数字。不能走的格子标 `X`。

```
   # # # # # #
   # F . . . #
   # . . O . #
   # . O . . #
   # . . . . #
   # # # # # #
```

拍下你的草稿纸，或者直接在纸上画好。做完再往下翻看答案。

<details>
<summary>点击展开参考答案</summary>

```
   #  #  #  #  #  #
   #  0  1  2  3  #
   #  1  2  X  3  #
   #  2  X  4  4  #
   #  3  3  4  5  #
   #  #  #  #  #  #
```

逐层推导过程：
- 第 0 层：食物(1,1) = 0
- 第 1 层：食物周围：(1,2) 和 (2,1) = 1
- 第 2 层：(1,3) 和 (3,1) = 2，（(2,2) 是障碍物 O，跳过）
- 第 3 层：(1,4) 和 (4,1) = 3
- 第 4 层：(1,5) 被墙挡，只能从 (3,1) 往下→(4,2) = 4，(3,3) 和 (3,4) 也 = 4
- 第 5 层：(4,5) = 5

</details>

---

## 5.2 用队列实现 BFS（1课时）

> **课时**：1 课时（约 1.5-2 小时）
> **目标**：看懂 `ai_bfs_from_food()` 每一行，能在控制台独立写出一个 BFS 距离计算程序

### 5.2.1 前置概念：什么是队列（FIFO）

BFS 需要一个"待处理列表"来记住"接下来要去处理哪些格子"。这个待处理列表有一个关键特性：**先放进来的要先处理**。就像排队买奶茶——先到的先买，晚到的排后面。

这种"先入先出"（FIFO = First In, First Out）的数据结构就叫**队列（Queue）**。

```
入队（push）→ [ B ] [ C ] [ D ] [   ] [   ] → 出队（pop）
                ↑                     ↑
              front                  back
```

BFS 为什么必须用队列？因为 BFS 是按层遍历的——第 0 层的格子先进队列，处理完它们才会轮到第 1 层的；第 1 层的处理完才轮到第 2 层的……队列天然保证了"早进来的早处理"，完美匹配逐层遍历的需求。

### 5.2.2 代码拆解：ai_bfs_from_food()

这是 AI 模块最核心的函数。别看它只有二十几行，但它实现了完整的 BFS 距离计算。我们从外到里逐段拆开。

#### 第一步：方向数组

```cpp
static const int AI_DR[4] = {-1, 0, 1, 0};
static const int AI_DC[4] = {0, -1, 0, 1};
static const Direction AI_DIR[4] = {UP, LEFT, DOWN, RIGHT};
```

三个数组，下标 0、1、2、3 分别对应**上、左、下、右**。为什么定义成数组而不是写四段重复代码？

因为数组可以配合 for 循环：

```cpp
for (int d = 0; d < 4; d++)
{
    int nr = cur.r + AI_DR[d];  // 新行 = 当前行 + 方向偏移
    int nc = cur.c + AI_DC[d];  // 新列 = 当前列 + 方向偏移
    // 现在 (nr, nc) 就是当前格子的上/左/下/右邻居
}
```

没有数组的话你就得写四遍几乎一样的代码——"向上走一格检查……向左走一格检查……"——代码又长又容易出错。数组把"四个方向"变成了数据，用循环统一处理。这种"用数据代替重复逻辑"的思路在编程里非常重要。

**常见错误**：方向数组的排列顺序有讲究。`AI_DR` 和 `AI_DC` 必须一一对应——下标 0 是上（-1, 0），下标 1 是左（0, -1），下标 2 是下（1, 0），下标 3 是右（0, 1）。顺序写错一个，BFS 就会朝错误的方向找。

```cpp
// WRONG：第2项对不上——DR是左(-0)但DC是下(+1)，方向就乱了
// static const int AI_DR[4] = {-1, 0, 1, 0};
// static const int AI_DC[4] = {0, 1, 0, -1};  // 下标1变成了DOWN的方向偏移！

// CORRECT：每一项的三个数组表示同一方向
//          上       左       下       右
// AI_DR:  {-1,      0,      1,      0}
// AI_DC:  { 0,     -1,      0,      1}
// AI_DIR: {UP,    LEFT,   DOWN,  RIGHT}
```

#### 第二步：队列——用数组模拟

```cpp
typedef struct node
{
    int r, c;
} Node;
static Node q[MAX_SNAKE_LEN];
```

`q` 是队列的存储器，用一个固定大小 400 的数组。两个变量 `front`（队头）和 `back`（队尾）控制入队和出队。

你可能会问：为什么不用 C++ 标准库的 `std::queue`？因为这个项目用了 EasyX 图形库，整体风格是"最小依赖"，尽量避免模板和 STL 的复杂性。用数组模拟队列是 C 语言的经典做法——简单、透明、无额外开销。学会了数组队列，以后用任何语言的队列库你都能理解底层在干什么。

`MAX_SNAKE_LEN = 400`：为什么选这个值？20x20 经典模式地图内部空地是 18x18 = 324 格，35x35 对战地图内部空地是 33x33 = 1089 格。队列大小只要够装 BFS 访问到的节点就行——实际上双蛇挤压空间后，BFS 通常访问不到 400 个节点。这是一个工程估算，如果将来地图更大，这个值也需要跟着调。

#### 第三步：距离数组 dist[][]

```cpp
static int dist[MAX_MAP_SIZE][MAX_MAP_SIZE];
```

`dist[r][c]` 表示从食物走到格子 `(r, c)` 需要几步。初始化全部为 -1。

**为什么是 -1？** 因为距离不可能是负数。用 -1 表示"这个格子还没访问过"，是一个信号值（sentinel value）。如果你用 0 表示"没访问过"，那食物本身的距离 0 就跟"没访问过"冲突了。

```cpp
for (int r = 0; r < game->map_size; r++)
    for (int c = 0; c < game->map_size; c++)
        dist[r][c] = -1;
```

#### 第四步：BFS 主循环

```cpp
int front = 0, back = 0;
q[back++] = {fr, fc};   // 食物入队，队列里现在有1个元素
dist[fr][fc] = 0;       // 食物到自己的距离 = 0

while (front < back)    // 队列不为空就一直循环
{
    Node cur = q[front++];  // 出队：取队头元素，front后移

    // 遍历四个方向
    for (int d = 0; d < 4; d++)
    {
        int nr = cur.r + AI_DR[d];
        int nc = cur.c + AI_DC[d];

        // 检查1：这个格子AI能走吗？
        if (!ai_can_go(game, nr, nc))
            continue;

        // 检查2：这个格子访问过了吗？
        if (dist[nr][nc] != -1)
            continue;

        // 标记距离（关键行！）
        dist[nr][nc] = dist[cur.r][cur.c] + 1;

        // 入队：这个格子将来也要往外扩散
        q[back++] = {nr, nc};
    }
}
```

**这个循环是 BFS 的灵魂。** 看清楚它做了什么：

1. 从队头取一个格子 `cur`（出队）
2. 看 `cur` 的四个邻居
3. 对于每个"能走 + 没访问过"的邻居，标上距离 = `cur` 的距离 + 1（入队）
4. 重复，直到队列为空

**为什么 `dist[nr][nc] != -1` 能防止重复访问？** 因为 BFS 第一次访问到一个格子时，一定是走了最短路径到达的。第二次再遇到它，距离只会更长，没必要再处理。所以每个格子只入队一次。

**常见错误**：

```cpp
// WRONG：忘记检查 dist[nr][nc] != -1
// 后果：同一个格子反复入队，队列爆炸（无限循环或超出数组边界）
for (int d = 0; d < 4; d++)
{
    int nr = cur.r + AI_DR[d];
    int nc = cur.c + AI_DC[d];
    if (!ai_can_go(game, nr, nc))
        continue;
    // 缺少 if (dist[nr][nc] != -1) continue; ← 漏了这一行！
    dist[nr][nc] = dist[cur.r][cur.c] + 1;
    q[back++] = {nr, nc};
}

// CORRECT：加上已访问检查
for (int d = 0; d < 4; d++)
{
    int nr = cur.r + AI_DR[d];
    int nc = cur.c + AI_DC[d];
    if (!ai_can_go(game, nr, nc))
        continue;
    if (dist[nr][nc] != -1)      // ← 这一行不能少
        continue;
    dist[nr][nc] = dist[cur.r][cur.c] + 1;
    q[back++] = {nr, nc};
}
```

### 5.2.3 ai_can_go() —— AI 的"可通行"判断

```cpp
static bool ai_can_go(const GameState *game, int r, int c)
{
    if (r <= 0 || r >= game->map_size - 1 || c <= 0 || c >= game->map_size - 1)
        return false;
    char ch = game->map[r][c];
    return (ch == '.' || ch == 'F');
}
```

AI 的"可通行"只认两个字符：空地 `'.'` 和食物 `'F'`。注意它**不看蛇身**——因为 BFS 只算"从食物出发当前能不能走"，它不需要知道哪条路是蛇的身体。实际 AI 选方向的时候，它会分别尝试四个方向，每个方向只走一步，这一步落在空地或食物上就行。

对比玩家蛇的 `can_enter_cell()`（如果你在第四章写过的话），玩家的判断会包含 `'B'`（蛇身）——撞到自己身体也算死。AI 不需要这个，因为 AI 的 BFS 是在**当前地图状态**下算的，只要当前这个格子是空地或食物，就认为可通行。

### 5.2.4 练一练：控制台 BFS 程序

**题目**：在控制台写一个独立的 C++ 程序，用 BFS 计算一个 5x5 网格的距离。具体要求：

1. 定义一个 5x5 的 char 数组，内容如下（`#` 边界，`.` 空地，`F` 食物，`O` 障碍物）
2. 食物位置：map[1][1]
3. 从食物出发跑 BFS，计算每个空格的步数
4. 输出最终的距离网格（-1 输出为 X，数字直接输出）

**地图**：
```
# # # # #
# F . . #
# . O . #
# . . . #
# # # # #
```

**预期输出**：
```
X X X X X
X 0 1 2 X
X 1 X 3 X
X 2 3 4 X
X X X X X
```

<details>
<summary>点击展开参考答案</summary>

```cpp
#include <stdio.h>

#define N 5

int main()
{
    // 0. 地图定义
    char map[N][N] = {
        {'#', '#', '#', '#', '#'},
        {'#', 'F', '.', '.', '#'},
        {'#', '.', 'O', '.', '#'},
        {'#', '.', '.', '.', '#'},
        {'#', '#', '#', '#', '#'}
    };

    // 1. 方向数组
    int dr[4] = {-1, 0, 1, 0};
    int dc[4] = {0, -1, 0, 1};

    // 2. 距离数组（初始全 -1）
    int dist[N][N];
    for (int r = 0; r < N; r++)
        for (int c = 0; c < N; c++)
            dist[r][c] = -1;

    // 3. 队列
    struct Node { int r, c; };
    Node q[100];
    int front = 0, back = 0;

    // 4. 食物入队
    int food_r = 1, food_c = 1;
    q[back++] = {food_r, food_c};
    dist[food_r][food_c] = 0;

    // 5. BFS
    while (front < back)
    {
        Node cur = q[front++];
        for (int d = 0; d < 4; d++)
        {
            int nr = cur.r + dr[d];
            int nc = cur.c + dc[d];
            if (nr < 0 || nr >= N || nc < 0 || nc >= N)
                continue;
            if (dist[nr][nc] != -1)
                continue;
            if (map[nr][nc] != '.' && map[nr][nc] != 'F')
                continue;
            dist[nr][nc] = dist[cur.r][cur.c] + 1;
            q[back++] = {nr, nc};
        }
    }

    // 6. 打印结果
    for (int r = 0; r < N; r++)
    {
        for (int c = 0; c < N; c++)
        {
            if (map[r][c] == '#')
                printf("X ");
            else if (map[r][c] == 'O')
                printf("X ");
            else if (dist[r][c] == -1)
                printf("? ");
            else
                printf("%d ", dist[r][c]);
        }
        printf("\n");
    }
    return 0;
}
```

注意点：
- `map[nr][nc] != '.' && map[nr][nc] != 'F'` 这个判断等价于 AI 的 `ai_can_go()`，只有空地或食物才能走。
- 输出时障碍物 `O` 也标为 X，因为它不可通行。
- 如果你得到了和预期一模一样的输出，恭喜你——你已经写出了 BFS。

</details>

---

## 5.3 三种难度设计（1课时）

> **课时**：1 课时（约 1.5-2 小时）
> **目标**：理解 Easy / Normal / Hard 三种策略的差异，能解释为什么每个难度要那么设计

### 5.3.1 ai_choose_direction —— 调度器

在实际的 Snake 游戏里，AI 并不直接调用 `ai_bfs_from_food()`。调用链是这样的：

```
游戏主循环 → move_battle_snakes() → ai_choose_direction()
                                         ↓
                                   switch (difficulty)
                                   ├── AI_EASY   → ai_dir_easy()
                                   ├── AI_NORMAL → ai_dir_normal()
                                   └── AI_HARD   → ai_dir_hard()
```

`ai_choose_direction()` 是一个**调度器（dispatcher）**——它自己不干活，只是根据难度"分发"到对应的具体函数。

```cpp
Direction ai_choose_direction(const GameState *game, AIDifficulty difficulty)
{
    switch (difficulty)
    {
    case AI_EASY:   return ai_dir_easy(game);
    case AI_NORMAL: return ai_dir_normal(game);
    case AI_HARD:
    default:        return ai_dir_hard(game);
    }
}
```

`default` 走 Hard——这是一种防御性设计：如果因为某种原因传了一个非法难度值进来，宁可按最高难度处理，也别什么都不做。

### 5.3.2 Easy 难度 —— 偶尔犯傻的对手

```cpp
static Direction ai_dir_easy(const GameState *game)
{
    Point hd = game->ai_snake[0];

    // 第一层：30% 概率随机走一个合法方向
    if (rand() % 100 < 30)
    {
        int valid[4], cnt = collect_valid_dirs(game, valid);
        if (cnt > 0)
            return AI_DIR[valid[rand() % cnt]];
    }

    // 第二层：找到食物，用曼哈顿距离选最近方向
    // ... 遍历四个方向，计算 abs(nr - fr) + abs(nc - fc)
    // ... 选曼哈顿距离最小的方向

    // 第三层：找不到食物路径 → 用 ai_free_count() 选"最宽敞"的方向

    // 第四层：以上都失败 → 随便选一个能走的方向
}
```

Easy AI 的核心设计思想：**它不完美，但它会朝食物走**。

**`rand() % 100 < 30` 是什么意思？**

`rand()` 返回一个 0 到 RAND_MAX 之间的随机整数。`rand() % 100` 把它映射到 0 到 99。如果这个值小于 30（概率 30/100 = 30%），就走随机方向。另外 70% 的时间，AI 会用曼哈顿距离正经找食物。

30% 的随机率产生了一个有趣的游戏体验：AI 大部分时间看起来很聪明（朝着食物走），但偶尔会"犯傻"走错方向。这让玩家感觉"这个 AI 我能赢"——如果 AI 每次都走最优解，新手根本打不过。

**Easy AI 没有安全检测**。它只看眼前——能朝食物走就朝食物走，不管走了之后会不会把自己困死。这也是 Easy 难度"该输的时候会输"的原因。

### 5.3.3 Normal 难度 —— 聪明的但偶尔失误的对手

```cpp
static Direction ai_dir_normal(const GameState *game)
{
    // 第一层：BFS 找食物的最短路径
    ai_bfs_from_food(game, fr, fc);
    for (int d = 0; d < 4; d++)
    {
        // 如果这个方向可达且距离更短 → 候选
        if (dist[nr][nc] < best_dist)
        {
            best_dist = dist[nr][nc];
            best_dir = d;
        }
    }

    // 选好方向后，15% 概率改成随机
    if (rand() % 100 < 15)
    {
        int valid[4], cnt = collect_valid_dirs(game, valid);
        if (cnt > 0)
            return AI_DIR[valid[rand() % cnt]];
    }

    // 第二层 → 自由空间回退
    // 第三层 → 任意合法方向
}
```

Normal AI 和 Easy AI 有两个本质区别：

| 维度 | Easy | Normal |
|------|------|--------|
| 寻路方式 | 曼哈顿距离（会"穿墙"） | **BFS（绕过障碍物）** |
| 随机干扰 | 30% | **15%** |
| 随机时机 | 决策前（可能走任何方向） | 决策后（先算好最优，再"搞破坏"） |

关键区别在于随机干涉的**时机**。Easy 的 30% 是"先随机，后算路"——如果随机了，就完全不走脑子。Normal 的 15% 是"先算好最优方向，再有一定概率把它覆盖成随机"——这意味着即使"犯傻"，也是从合法方向里挑一个，不会朝着墙走。

**为什么 Normal 而不是"Perfect"？** 因为游戏必须有意思。如果一个 AI 每次都走完美的 BFS 最短路径，而且地图上没有蛇身障碍（Normal 不考虑自己的尾巴会不会堵路），那它基本上不会死。玩家要赢它纯靠运气——AI 必须在某一帧恰好没有 BFS 路径（地图被蛇身占满了），或者被玩家逼到死角。这会让对局又长又无聊。

15% 的随机干扰给玩家留了一个窗口——Normal AI 偶尔会失误，玩家抓住失误就能赢。这个数字是通过反复试玩找到的平衡点：太小了 AI 看起来从不犯错（无聊），太大了 AI 看起来像傻子（没挑战）。

**Normal 仍然没有安全检测**——即使 BFS 找到最短路径，那条路径可能把蛇带进一个死胡同。当蛇很长时，BFS 算出来的"最短路径"可能刚好堵住了自己尾巴的出路。这就是我们 5.4 要解决的核心问题。

### 5.3.4 Hard 难度 —— 几乎不败的对手

```cpp
static Direction ai_dir_hard(const GameState *game)
{
    // 第一层：BFS 找食物路径
    ai_bfs_from_food(game, fr, fc);
    for (int d = 0; d < 4; d++)
    {
        if (game->ai_length < 105)
        {
            // 蛇短 → 激进吃食物（不用担心把自己困死）
            if (dist[nr][nc] < best_dist)
            { best_dist = dist[nr][nc]; best_dir = d; }
        }
        else
        {
            // 蛇长 → 加安全检查：走这步之后还能不能回到尾巴？
            if (!ai_can_reach_tail_after_move(game, d))
                continue;                    // 回不去 → 放弃这个方向
            if (dist[nr][nc] < best_dist)
            { best_dist = dist[nr][nc]; best_dir = d; }
        }
    }

    // 第二层 → 自由空间回退
    // 第三层 → 任意合法方向
}
```

Hard AI 的核心创新在于**阶段切换**：

- **长度 < 105（早期）**：蛇还很短，占的地图面积小，基本上 BFS 路径不会把自己堵死。这时候激进吃食物 —— 因为吃得越快，积分越高，优势越大。
- **长度 >= 105（中后期）**：蛇已经很长了，随便走都可能堵住自己的退路。这时候每个候选方向都要经过 `ai_can_reach_tail_after_move()` 检查——"如果我走这一步，我还能不能回到我的尾巴？"如果走完之后尾巴被堵死了，这个方向直接放弃。

**为什么阈值是 105？** 这是一个经验值的估算。对战地图是 35x35 = 1225 格（内部可走区域 33x33 = 1089 格）。当蛇长度为 105 时，蛇身占整个可走区域的比例大约是 105/1089 ≈ 9.6%，接近 10%。这时候蛇开始有"缠绕自己"的风险——BFS 路径可能会穿过自己的身体区域。

这不是一个严格的数学推导值，而是试出来的。你可以在代码里改成 50 或 150 试试看，AI 的行为会完全不一样——阈值太低（安全检查过早开始），AI 会过于保守，半天吃不到食物；阈值太高（安全检查开始太晚），蛇已经把自己堵死了才想起来检查，来不及了。

**注意**：Hard AI 没有随机干扰——它走的每一步都是经过计算的，不会"故意犯傻"。这也是为什么它叫 Hard。

### 5.3.5 三个难度行为对比

假设同一局对战，玩家不动，观察 AI 在前 30 步的表现：

| 观察项目 | Easy | Normal | Hard |
|----------|------|--------|------|
| 前 10 步 | 大约 3 步随机、7 步朝食物走 | 约 1.5 步随机、其余 BFS | 每一步都是 BFS 最优 |
| 遇到障碍物 | 可能被迷惑，绕远路 | BFS 自动绕开 | BFS 自动绕开 |
| 蛇长 < 105 | 可能自己困死自己 | 可能自己困死自己 | 激进吃食物 |
| 蛇长 >= 105 | 大概率困死自己 | 有概率困死自己 | 每一步都验证尾巴可达 |
| 玩家胜率（新手） | ~60% | ~30% | ~5% |

### 5.3.6 练一练：调参实验

**题目**：把 Easy AI 的随机概率从 30% 改成 70%，编译运行，观察 AI 的行为变化。再把改成 5%，再试试。

要求回答两个问题：
1. 随机概率 70% 时的 AI 行为像什么？这种 AI 好玩吗？为什么？
2. 随机概率 5% 时的 AI 行为有什么变化？对玩家的挑战感怎么样？

<details>
<summary>点击参考答案（分析，非代码）</summary>

**1. 70% 随机概率：** AI 每 10 步有 7 步在乱走。它看起来完全不像在"玩"——大部分时间在做无效移动，偶尔"歪打正着"碰到食物。这种 AI 不好玩，因为玩家感受不到对手的意图，赢了也没成就感。这像是"智障模式"。

**2. 5% 随机概率：** AI 每 20 步才犯一次错。在简单地图上（蛇短、障碍物少），AI 几乎不失手，玩家很容易被打败。但如果曼哈顿距离被障碍物欺骗，AI 仍然会傻傻朝墙走——它的根本短板（不会绕障）没有改善。玩家的体验是：有时候 AI 聪明得让人绝望，有时候突然犯傻——但犯傻不是因为随机，而是因为曼哈顿算法本身的局限。这种"不可预测的聪明"反而比纯随机更让人恼火。

结论：概率只是一个调味料——如果底层算法本身就不行（Easy = 曼哈顿），调大概率只是让 AI 从"偶尔笨"变成"一直笨"。AI 的水平是由**算法选择**决定的，概率只负责调节"失误频度"。

</details>

---

## 5.4 尾部可达性检查（1课时）——本章最难内容

> **课时**：1 课时（约 1.5-2 小时）
> **目标**：理解 `ai_can_reach_tail_after_move()` 的每一步，能手动模拟一个安全检查场景

**警告**：这一节是整章最难的，也是整本课程在算法层面最深入的。读懂这一节，你就掌握了游戏 AI 安全决策的核心思路。如果第一遍没完全懂，不要气馁——画图，跟着步骤走，第二遍就通了。

### 5.4.1 问题引入：Normal AI 死在什么样的局面？

想象这样一个场景——Normal AI 的蛇已经比较长了，地图是这样的：

```
  # # # # # # # # # #
  # C C C C H . . . #    H = 蛇头（朝右）
  # C . . . . . . . #    C = 蛇身
  # . F . . . . . . #    F = 食物
  # . . . . . . . . #
  # # # # # # # # # #
```

蛇头 `H` 在 (1, 5)。食物 `F` 在 (3, 2)。BFS 算出来：向右走 food 距离近。

AI 向右走了一步：

```
  # # # # # # # # # #
  # C C C C C H . . #    蛇头到了 (1, 6)
  # C . . . . . . . #    尾巴还在 (1, 1)
  # . F . . . . . . #
  # . . . . . . . . #
  # # # # # # # # # #
```

继续向右、向右……直到蛇头到了最右边，它必须往下转：

```
  # # # # # # # # # #
  # C C C C C C C H #    蛇头在 (1, 8)，下面是墙
  # . . . . . . . . #    唯一可走方向：DOWN → (2, 8)
  # . F . . . . . . #
  # . . . . . . . . #
  # # # # # # # # # #
```

往下走，然后向左去找食物（因为食物在左边）：

```
  # # # # # # # # # #
  # C C C C C C C C #    蛇头 (3, 8)，正在往左走
  # . . . . . . . C #    身体形成了一圈围墙
  # . F . . . . . H #
  # . . . . . . . . #
  # # # # # # # # # #
```

**死局来了**：蛇把上边和右边全堵死了，蛇头在里面。它得往左走才能吃食物，但吃着吃着就会发现——自己的身体把自己围在一个越来越小的圈里。最终，当蛇身完全占据所有可走空间时，蛇头无路可走，撞上自己的身体，死了。

Normal AI 不会预见这个问题——它只看当前 BFS 距离，不管走了这步之后"还能不能回到尾巴"。Hard AI 的 `ai_can_reach_tail_after_move()` 就是专门解决这个问题的。

### 5.4.2 ai_can_reach_tail_after_move() 完整拆解

```cpp
static bool ai_can_reach_tail_after_move(const GameState *game, int d)
{
    const Point *body = game->ai_snake;
    int len = game->ai_length;

    Point hd = body[0];        // 当前蛇头
    Point tail = body[len - 1]; // 当前蛇尾

    // Step 1: 计算假设移动后的新头位置
    int nr = hd.row + AI_DR[d];
    int nc = hd.col + AI_DC[d];

    // Step 2: 复制一份地图（模拟用，不能改真实地图）
    char sim[MAX_MAP_SIZE][MAX_MAP_SIZE];
    for (int r = 0; r < game->map_size; r++)
        for (int c = 0; c < game->map_size; c++)
            sim[r][c] = game->map[r][c];

    // Step 3: 判断这一步会不会导致蛇变长
    bool eat = (sim[nr][nc] == 'F');      // 吃到食物？
    bool auto_grow = ((game->move_count + 1) % game->grow_step) == 0; // 自动增长？
    bool will_grow = eat || auto_grow;

    // Step 4: 如果不变长 → 尾巴会消失（尾部的格子变回空地）
    if (!will_grow)
        sim[tail.row][tail.col] = '.';

    // Step 5: 在模拟地图上画新蛇的位置
    sim[hd.row][hd.col] = 'C';  // 旧头变成身体
    sim[nr][nc] = 'A';           // 新头

    // Step 6: 新尾巴在哪？
    Point new_tail;
    if (will_grow)
        new_tail = tail;         // 变长 → 尾巴位置不变
    else
        new_tail = body[len - 2]; // 不变长 → 尾巴前进一格

    // Step 7: BFS 问：从新头能走到新尾巴吗？
    int prev_dir[MAX_MAP_SIZE][MAX_MAP_SIZE];
    memset(prev_dir, -1, sizeof(prev_dir));
    return ai_bfs_path(sim, game->map_size, nr, nc,
                       new_tail.row, new_tail.col, prev_dir);
}
```

这个函数做了七件事。我们一步一步用**具体数字**走一遍。

#### 用数字走一遍

假设你要检查方向 `d = 3`（RIGHT = 向右）是否安全。

当前状态：
- `hd = (5, 8)`, `tail = (5, 2)`, `len = 7`
- 食物在 `(8, 3)`，新头位置 `(5, 9)` 不是食物
- `move_count = 15 → (15+1) % 8 = 0 →` 这步会触发自动增长！

**Step 1**: `nr = 5 + 0 = 5`, `nc = 8 + 1 = 9` → 新头 (5, 9)

**Step 2**: 复制整张地图到 `sim`（模拟用，不影响真实 game->map）

**Step 3**: `eat = false`（(5,9) 不是 F）。`auto_grow = true`（因为 (15+1) % 8 == 0）。`will_grow = true`。

**Step 4**: `will_grow = true` → **不删除尾巴**。尾巴 (5, 2) 保持为蛇身 `'C'`。

**Step 5**:
- `sim[5][8] = 'C'` —— 旧头变身体
- `sim[5][9] = 'A'` —— 标注新头位置

**Step 6**: `will_grow = true` → `new_tail = old_tail = (5, 2)`

**Step 7**: 从新头 (5, 9) 出发，BFS 问"能不能走到 (5, 2)？"。能 → 返回 `true`（安全！）。不能 → 返回 `false`（危险，不能走这个方向）。

#### 回到之前的死局，模拟检查

在死局中，蛇头在 (3, 8)，蛇身围住了上边和右边。蛇头只有两个合法方向：UP 和 LEFT。

`ai_can_reach_tail_after_move(game, UP)` —— 检查上：
- 新头 (2, 8)。在 sim 上，蛇身占满了 (1, 2) 到 (1, 8)，像一个盖子。
- BFS 从 (2, 8) 出发找尾巴……尾巴可能在左下角很远的地方。检查路径时发现蛇身形成了一堵墙，从新头到不了尾巴。
- **返回 false → 这个方向不安全！**

`ai_can_reach_tail_after_move(game, LEFT)` —— 检查左：
- 新头 (3, 7)。这个方向在身体围墙内部，暂时有路到尾巴。
- **返回 true → 安全的方向。**

Hard AI 跳过 UP（不安全），选 LEFT（安全且朝着食物的 BFS 距离最短）。AI 不会把自己往死路上引。

### 5.4.3 ai_bfs_path() vs ai_bfs_from_food() —— 两个 BFS 有什么区别？

虽然都叫 BFS，但这俩函数的用途完全不同：

| | ai_bfs_from_food | ai_bfs_path |
|---|---|---|
| **用途** | 计算"从食物到每个格子的距离" | 判断"从 A 点能不能走到 B 点" |
| **输入** | 真实地图 + 食物坐标 | 模拟地图 + 起点 + 终点 |
| **返回值** | 无（结果存在全局 dist[][]） | bool（true=能到达，false=不能） |
| **目标** | 给每个格子标上步数 | 找到一条路就停 |
| **使用场景** | AI 选方向——"往哪走离食物最近" | 安全性检查——"走了这步还能活下去吗" |
| **遇到终点** | 不关心终点，要算全图 | 找到终点立刻返回 true |

`ai_bfs_path` 的一个关键细节：它把**终点也当作可通行**。看代码：

```cpp
if (nr == tr && nc == tc)
{
    vis[nr][nc] = 1;
    prev_dir[nr][nc] = d;
    queue[back++] = {nr, nc};
    continue;
}
```

即使终点在地图上不是 `'.'` 也不是 `'F'`（比如终点是蛇身），BFS 仍然可以"到达"它。这是合理的——蛇的尾巴确实有身体，但在"安全性检查"的语境里，尾巴是可到达的目标（蛇移动后尾巴可能消失或保持）。

### 5.4.4 常见错误

```cpp
// WRONG：在模拟地图上没删旧尾巴
// 后果：BFS 被旧尾巴挡住，明明安全的路径被误判为不安全
if (!will_grow)
    // 忘了 sim[tail.row][tail.col] = '.';
// → BFS 看到尾巴位置还是蛇身，走不过去

// WRONG：搞混了"不变长时的尾巴位置"
if (!will_grow)
    new_tail = tail;  // 错！不变长时尾巴往前走了一格
// CORRECT: new_tail = body[len - 2];  // 尾巴前移

// WRONG：在真实地图上做模拟（没有先复制）
// 后果：污染了 game->map，渲染会错乱
sim = game->map;  // 这只是指针赋值，没有复制内容！
// CORRECT：用双层 for 循环逐格复制
```

### 5.4.5 练一练：纸笔模拟

**题目**：下面是一个 corner case。蛇头在 (3, 3)，方向朝右。自己的身体围成了一个圈，只有一个缺口。请在纸上画出以下场景，判断四个方向中哪个能通过 `ai_can_reach_tail_after_move()` 的安全检查。

```
  # # # # # # #      H = 蛇头 (3,3)
  # . . . . . #      C = 蛇身
  # . C C C . #      蛇身：(2,2)(2,3)(2,4)(3,2)  ← 注意头也在蛇身上
  # . C H C . #      尾巴：(2,2)
  # . C C C . #      食物：(1,1)
  # . . . . . #
  # # # # # # #
```

分析四个方向：
1. 左(LEFT)：新头 (3, 2) —— 但 (3, 2) 是蛇身！这个方向不是合法方向（`ai_can_go` 不会让 AI 往蛇身上走），所以不会进入安全检查。
2. 下(DOWN)：新头 (4, 3) —— 空地，但走了之后身体变成 2x3 的矩形 + 一个角，BFS 从新头到尾巴……能通吗？
3. 右(RIGHT)：新头 (3, 4) —— 蛇身！不合法方向。
4. 上(UP)：新头 (2, 3) —— 也是蛇身！不合法方向。

最终：蛇只有 DOWN 是合法方向，DOWN 的安全检查结果决定了 AI 的下一步。

<details>
<summary>点击展开参考答案</summary>

**DOWN (d=2, 新头 4,3) 的安全检查：**

假设蛇长度 = 5（头 3,3 + 身 3,2|2,4|2,3|2,2），当前步不计入自动增长。

Step 1: `nr=4, nc=3`。不是食物。`auto_grow = false`。`will_grow = false`。

Step 2: 复制地图到 sim。

Step 3: `!will_grow` → `sim[2][2] = '.'`（尾巴消失）。

Step 4: `sim[3][3] = 'C'`（旧头变身体）。`sim[4][3] = 'A'`（新头）。

Step 5: `new_tail = body[3]` → 原 snake[3] 是 (2, 3)。

Step 6: BFS 从 (4, 3) 出发找 (2, 3)。

现在的 sim 地图上蛇身位置：`(3,3)(3,2)(2,4)(2,3)`——其中 (2, 3) 是新尾巴（目标），(2, 2) 已清空为 `'.'`。

BFS 从 (4,3) → 上(3,3)是蛇身走不通，左(4,2)是空地可走，右(4,4)是空地可走。

从 (4,2) → 上(3,2)是蛇身走不通。
从 (4,4) → 上(3,4)是空地可走。

从 (3,4) → 上(2,4)是蛇身走不通，左(3,3)是蛇身走不通。

**结论：BFS 从新头到不了新尾巴。`ai_can_reach_tail_after_move(game, DOWN)` 返回 false。这个方向不安全！**

这意味着：如果蛇走 DOWN，之后的身体会堵住自己，形成一个死胡同。

那 AI 怎么办？回顾 Hard AI 的逻辑：所有方向的安全检查都失败 → 跳到第二层"自由空间回退"。选一个"能走且自由空间最大"的方向拖延时间，等食物出现在更好的位置，或者等身体自然缩短。

</details>

---

## 5.5 自由空间回退 + 完整 AI 串联（1课时）

> **课时**：1 课时（约 1.5-2 小时）
> **目标**：理解 AI 决策的完整链路，能画出 AI 决策树，能在控制台模拟 AI 选方向

### 5.5.1 ai_free_count() —— 当找不到食物路径时的保底策略

当 BFS 找不到通往食物的路径时（食物被障碍物或蛇身隔绝了），AI 不能站在原地不动——它得选一个方向继续走，等食物重新出现在可达位置。但走哪个方向？

答案是：**走"最宽敞"的方向。** 即——假设走了这个方向后，从新位置能到达的空地最多的那个方向。

```cpp
static int ai_free_count(const GameState *game, int sr, int sc)
{
    int vis[MAX_MAP_SIZE][MAX_MAP_SIZE];
    memset(vis, 0, sizeof(vis));

    Point queue[MAX_SNAKE_LEN];
    int front = 0, back = 0;
    queue[back++] = {sr, sc};
    vis[sr][sc] = 1;
    int cnt = 0;

    while (front < back)
    {
        Point cur = queue[front++];
        cnt++;                   // 每取出一个格子，计数+1
        for (int d = 0; d < 4; d++)
        {
            int nr = cur.row + AI_DR[d];
            int nc = cur.col + AI_DC[d];
            if (nr <= 0 || nr >= game->map_size - 1 || nc <= 0 || nc >= game->map_size - 1)
                continue;
            if (vis[nr][nc])
                continue;
            char ch = game->map[nr][nc];
            if (ch != '.' && ch != 'F')
                continue;
            vis[nr][nc] = 1;
            queue[back++] = {nr, nc};
        }
    }
    return cnt;   // 返回从起点能到达的空地总数
}
```

这本质上也是一个 BFS——只是它不记录距离，只数格子。它做的事情叫**洪水填充（Flood Fill）**。想象从起点倒一瓶水，水会顺着空地向四面八方扩散，直到碰壁为止。最后数一下水覆盖了多少个格子，这个数字就是"从起点能走的自由空间"。

**为什么算"自由空间"？** 因为贪吃蛇的核心生存法则不是"吃得更快"，而是"活得够久"——只要你不死，食物迟早刷新到你够得到的地方。"选最宽敞的方向"等于"给自己留最大的活动余地"，这是拖延时间的数学最优策略。

类比：你被困在一个迷宫里，迷宫有几条路，你不知道出口在哪。最优策略是选那条"能通向最多岔路口"的路走——因为岔路越多，你后续的选择就越多，找到出口的概率就越大。

### 5.5.2 完整 AI 决策树

```
                        ai_choose_direction(game, difficulty)
                                      │
                            ┌─────────┼─────────┐
                            │         │         │
                         AI_EASY  AI_NORMAL  AI_HARD
                            │         │         │
                    ┌───────┴───┐  ┌──┴────┐  ┌─┴───────────┐
                    │           │  │       │  │             │
                 30%随机?    找到食物? BFS路径? 找到食物?   找到食物?
                    │        │    │    │  │    │           │
                   Yes       No  Yes   No Yes   No      Yes│       No
                    │        │    │    │  │    │      ┌───┴──┐    │
               随机方向  曼哈顿方向 │ 自由空间 BFS方向 任意  len<105? 自由空间→任意
                            │    │         │         ├───┴───┐
                         找到?   │         │         Yes      No
                         │  │   │         │         │        │
                        Yes No  │         │      激进BFS  安全检查
                         │  │   │         │         │      + BFS
                       返回 自由空间→任意  │         │        │
                                         │         │      找到?
                                         │         │      │  │
                                         │         │     Yes No
                                         │         │      │  │
                                         │         │     返回 自由空间→任意
                                         │         │
                                         15%随机干扰
                                             │
                                           返回
```

文字总结每条路径：

| 难度 | 第1步 | 第2步 | 第3步 | 第4步（最后兜底） |
|------|-------|-------|-------|------------------|
| Easy | 30%随机 | 曼哈顿距离最优 | 自由空间最大 | 任意合法方向 |
| Normal | BFS最短路径 → 15%随机干扰 | 自由空间最大 | 任意合法方向 | - |
| Hard | BFS + 安全检查(len>=105) | 自由空间最大 | 任意合法方向 | - |

可以看到一个共同的模式：

1. **主策略**（算路找食物）
2. **降级策略**（食物不可达→选最宽敞方向拖延）
3. **最终兜底**（所有都失败→随便选一个能走的）

这个三级回退模式是游戏 AI 的经典设计：**主策略负责赢，降级策略负责不输，兜底策略负责不死机。**

### 5.5.3 完整调用链回顾

从你按下键盘到 AI 蛇移动一步，函数调用链是这样的：

```
main.cpp: 游戏主循环
  │
  ├─ input.cpp: process_input()        ← 读你的按键
  │
  ├─ game.cpp: move_battle_snakes()   ← 双蛇移动逻辑
  │     │
  │     ├─ move_snake()（玩家蛇）
  │     │   ├─ 检查碰撞
  │     │   ├─ 吃到食物？→ 增长 + place_food()
  │     │   └─ 自动增长逻辑
  │     │
  │     └─ ai_choose_direction()       ← AI 蛇选方向 ★
  │           │
  │           └─ ai_dir_easy / _normal / _hard()
  │                 │
  │                 ├─ ai_bfs_from_food()           ← BFS 距离计算
  │                 ├─ ai_can_reach_tail_after_move() ← 安全检查(Hard)
  │                 ├─ ai_free_count()              ← 自由空间
  │                 └─ ai_can_go()                  ← 可通行判断
  │
  └─ render.cpp: render_game()         ← 画帧
```

### 5.5.4 练一练：自己做 AI 决策

**题目**：写一个控制台程序，手动输入贪吃蛇局面，程序输出 AI 推荐的方向。具体要求：

1. 输入地图大小 N（3-10）
2. 逐行输入地图（用字符 `# . F O H C` 等）
3. 程序假设蛇头在 H 位置，用 BFS 找食物 F
4. 输出：
   - BFS 距离网格
   - 推荐方向（UP / LEFT / DOWN / RIGHT）
   - 如果食物无法到达，输出 "无法到达食物，建议选择自由空间最大的方向"

**输入示例**：
```
5
#####
#H..#
#.O.#
#..F#
#####
```

**预期输出**（大致）：
```
BFS 距离网格：
 X  X  X  X  X
 X  0  1  X  X
 X  1  X  3  X
 X  2  3  4  X
 X  X  X  X  X

推荐方向：DOWN (距离=2)
备选方向：LEFT (距离=2)
```

<details>
<summary>点击展开参考答案</summary>

```cpp
#include <stdio.h>

#define MAX_N 12

int dr[4] = {-1, 0, 1, 0};
int dc[4] = {0, -1, 0, 1};
const char *dir_name[4] = {"UP", "LEFT", "DOWN", "RIGHT"};

int main()
{
    int N;
    printf("输入地图大小 N: ");
    scanf("%d", &N);
    getchar();  // 吞掉换行

    char map[MAX_N][MAX_N];
    int head_r = -1, head_c = -1;
    int food_r = -1, food_c = -1;

    printf("逐行输入地图:\n");
    for (int r = 0; r < N; r++)
    {
        for (int c = 0; c < N; c++)
        {
            scanf(" %c", &map[r][c]);
            if (map[r][c] == 'H')
            { head_r = r; head_c = c; map[r][c] = '.'; }
            if (map[r][c] == 'F')
            { food_r = r; food_c = c; }
        }
    }

    if (head_r == -1 || food_r == -1)
    {
        printf("错误：地图必须包含 H 和 F。\n");
        return 1;
    }

    // BFS 从食物出发
    int dist[MAX_N][MAX_N];
    for (int r = 0; r < N; r++)
        for (int c = 0; c < N; c++)
            dist[r][c] = -1;

    struct Node { int r, c; } q[200];
    int front = 0, back = 0;
    q[back++] = {food_r, food_c};
    dist[food_r][food_c] = 0;

    while (front < back)
    {
        Node cur = q[front++];
        for (int d = 0; d < 4; d++)
        {
            int nr = cur.r + dr[d];
            int nc = cur.c + dc[d];
            if (nr < 0 || nr >= N || nc < 0 || nc >= N)
                continue;
            if (dist[nr][nc] != -1)
                continue;
            if (map[nr][nc] != '.' && map[nr][nc] != 'F')
                continue;
            dist[nr][nc] = dist[cur.r][cur.c] + 1;
            q[back++] = {nr, nc};
        }
    }

    // 打印距离网格
    printf("\nBFS 距离网格：\n");
    for (int r = 0; r < N; r++)
    {
        for (int c = 0; c < N; c++)
        {
            if (map[r][c] == '#')
                printf(" X ");
            else if (map[r][c] == 'O')
                printf(" X ");
            else if (dist[r][c] == -1)
                printf(" ? ");
            else
                printf("%2d ", dist[r][c]);
        }
        printf("\n");
    }

    // 选方向
    int best_dir = -1, best_dist = 1000000;
    for (int d = 0; d < 4; d++)
    {
        int nr = head_r + dr[d];
        int nc = head_c + dc[d];
        if (nr < 0 || nr >= N || nc < 0 || nc >= N)
            continue;
        if (map[nr][nc] != '.' && map[nr][nc] != 'F')
            continue;
        if (dist[nr][nc] == -1)
            continue;
        if (dist[nr][nc] < best_dist)
        {
            best_dist = dist[nr][nc];
            best_dir = d;
        }
    }

    if (best_dir != -1)
    {
        printf("\n推荐方向：%s (距离=%d)\n", dir_name[best_dir], best_dist);

        // 列出备选
        printf("备选方向：");
        int first = 1;
        for (int d = 0; d < 4; d++)
        {
            if (d == best_dir) continue;
            int nr = head_r + dr[d];
            int nc = head_c + dc[d];
            if (nr < 0 || nr >= N || nc < 0 || nc >= N)
                continue;
            if (map[nr][nc] != '.' && map[nr][nc] != 'F')
                continue;
            if (dist[nr][nc] == -1)
                continue;
            if (!first) printf(", ");
            printf("%s(距离=%d)", dir_name[d], dist[nr][nc]);
            first = 0;
        }
        if (first) printf("无");
        printf("\n");
    }
    else
    {
        printf("\n无法到达食物，建议选择自由空间最大的方向。\n");
    }

    return 0;
}
```

**测试用的几个 case**：

- 输入示例中的地图 → 应输出 DOWN 或 LEFT
- 把 F 放在障碍物 `O` 围成的封闭区域里 → 应输出"无法到达食物"
- 把蛇头放在角落里，周围三面都是墙 → 只有一个合法方向

</details>

---

## 本章小结

这一章我们走完了从"BFS 是什么"到"三级 AI 完整决策树"的全过程。回顾一下你学了什么：

1. **BFS 的原理**：水波纹扩散，逐层遍历，无权图最短路径
2. **队列**：FIFO 数据结构，用数组模拟，front/back 双指针
3. **三层难度设计**：Easy（曼哈顿 + 30% 随机）、Normal（BFS + 15% 干扰）、Hard（BFS + 安全检查）
4. **尾部可达性检查**：模拟移动 → 复制地图 → BFS 判连通 → 安全/危险
5. **自由空间回退**：洪水填充计数 → 选最宽敞方向 → 拖延等待

你还亲手写了控制台 BFS 程序，做了纸笔模拟练习，甚至自己实现了 AI 决策器。

**下一步**：BFS 是入门，但远不是终点。有了 BFS 的底子，你会发现更高级的寻路算法（Dijkstra、A*）不过是 BFS 的"加权变种"——把"每步代价都是 1"换成"每步代价不一样"。这个"把 1 换成权重"的想法，就是算法世界从 BFS 走向 A* 的关键一步。不过那是另一个故事了。

你可以试着做下面这些实验来加深理解：

- 把 Hard AI 的安全检查阈值 105 改成 50，看 AI 行为变化
- 把 Normal AI 的随机概率 15% 改成 0%，看"完美 BFS AI"能不能打赢 Hard AI
- 在控制台 BFS 程序里加入"显示 BFS 搜索过程"——每搜一层打印一次中间状态
- 尝试把 BFS 改成 DFS（把队列改成栈），观察蛇的行为差异

---

> **下一章预告**：第六章会讲对战模式的完整玩法——双蛇碰撞判定、击杀计分、AI 复活机制、PvP 双人操控。到那时候，你会把本章的 AI 模块作为"对战模式的人机对手"拼进完整游戏里。
