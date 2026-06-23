---
type: concept
title: "第四章：游戏逻辑——食物、碰撞、模式"
created: 2026-06-22
tags:
  - curriculum
  - cpp
  - game-dev
  - game-logic
  - collision
status: developing
related:
  - "[[Snake Game Curriculum]]"
  - "[[snake-game-call-graph]]"
sessions: 5
duration: "8-10 小时"
goal: "经典模式可以完整游玩"
---

# 第四章：游戏逻辑——食物、碰撞、模式

> **目标**：学完本章后，经典模式的贪吃蛇可以从头到尾完整游玩——食物会随机出现、蛇会撞墙死、撞自己死、吃完食物会变长、填满地图会赢。
> **总课时**：5 课时（约 8-10 小时）
> **前置要求**：第一章（C++ 基础语法）、第二章（数组与结构体）、第三章（地图与渲染）

这一章是整个游戏的心脏。前三章你搭好了骨架——地图能画了、蛇能显示了、键盘能读了。但从"能看"到"能玩"，中间差的就是这一章要讲的东西：**食物怎么出现、蛇怎么判断撞了、身体怎么变长、一整局游戏怎么从头跑到尾**。

让我们从一个问题开始：你玩过的贪吃蛇，蛇每吃到一个豆子，下一个豆子是不是立刻出现在一个随机的新位置？这个"随机出现在空地"的功能，就是我们要写的第一个函数：`place_food()`。

---

## 4.1 食物生成（1课时）

> **课时**：1 课时（约 1.5-2 小时）
> **目标**：理解并独立写出 `place_food()` 的完整逻辑

### 4.1.1 食物是什么？

在 Snake 的地图系统里，食物就是用字符 `'F'` 表示的一个格子。它和空地 `'.'`、墙 `'#'`、蛇身 `'B'` 一样，都是 `map[row][col]` 这个二维字符数组里的一个元素。

```
地图上的表示：
  '#' = 墙（边界）
  '.' = 空地（蛇可以走的地方）
  'F' = 食物（蛇头碰到就吃掉）
  'B' = 蛇身
  'H' = 蛇头
```

食物的核心规则只有三条：
1. **每次只有一个食物**（经典模式）。蛇吃掉后，新食物出现在另一个随机空地。
2. **食物不能出现在墙上、障碍物上、蛇身上**——只能出现在 `'.'` 格子上。
3. **如果地图上没有空地了**（蛇把地图填满了）→ 玩家赢了，游戏结束。

这三条规则翻译成代码，就是 `place_food()` 函数要做的全部事情。

### 4.1.2 place_food() —— 逐行讲解

下面是 `place_food()` 的完整实现。别看它只有十几行，每一行都有讲究。我们一行一行拆开看。

```cpp
// 在随机空地上放置一个食物 'F'
// 如果地图已满（没有空地），则判定玩家获胜，游戏结束
static void place_food(GameState *game)
{
    // 步骤1：收集所有空地的坐标
    Point cells[MAX_SNAKE_LEN];
    int count = collect_empty_cells(game, cells);

    // 步骤3：没有空位 → 玩家赢了！
    if (count <= 0)
    {
        game->game_over = true;
        game->death_reason = DEATH_FULL_MAP;
        return;
    }

    // 步骤2：随机选一个空地，放上食物
    int idx = rand() % count;
    game->map[cells[idx].row][cells[idx].col] = 'F';
}
```

#### 第 1 行：`Point cells[MAX_SNAKE_LEN];`

声明了一个 `Point` 类型的数组，大小是 `MAX_SNAKE_LEN`（在 `game.h` 里定义为 400）。这个数组用来存所有空地的坐标。

为什么是 400？因为 20x20 经典模式地图的内部空地是 18x18 = 324 格，400 绰绰有余。这是一个**工程折中**：用固定大小数组换简单性，避免动态分配。

> **注意**：在 35x35 的对战地图上，内部空地是 33x33 = 1089 格，超出了 400 的上限。实际项目中，`collect_empty_cells()` 内部加了 `if (count >= MAX_SNAKE_LEN) break;` 这个边界保护来防止数组越界。如果你将来要给对战模式扩展这个功能，记得要么加大数组，要么加上边界检查——这是典型的"简单方案在小数据量下够用，但需要知道它的边界"。经典模式下这个限制完全不影响使用。

#### 第 2 行：`int count = collect_empty_cells(game, cells);`

调用 `collect_empty_cells()`，把地图上所有 `'.'` 格子的坐标收集到 `cells` 数组里，返回收集到的数量。`count` 就是"现在有多少个空地"。

#### 第 4-9 行：空地数量检查

```cpp
if (count <= 0)
{
    game->game_over = true;
    game->death_reason = DEATH_FULL_MAP;
    return;
}
```

如果 `count <= 0`，说明地图上一个 `'.'` 都没了。这意味着蛇的身体和障碍物把整个地图占满了——**玩家赢了**。这是一种特殊的"Game Over"，但不是失败，而是胜利。`DEATH_FULL_MAP` 这个枚举值（定义在 `game.h` 里）的字面意思是"死因：地图已满"，实际上表示"你填满了整个地图，你赢了"。

注意这里用的是 `<=` 而不是 `==`。作为一个防御性编程的习惯，用 `<=` 比 `==` 安全——万一什么奇怪的 bug 导致 count 变成负数，程序也不会崩。

#### 第 11 行：`int idx = rand() % count;`

这是整个函数里最精妙的一行。`rand()` 返回一个 0 到 `RAND_MAX`（通常 32767）之间的伪随机整数，`% count` 取余数，结果在 `0` 到 `count-1` 之间。这个余数就是我们要的食物索引。

比如：`count = 42`（42 个空地），`rand()` 返回 12345，`12345 % 42 = 39`。于是 `cells[39]` 被选中，成为新食物的位置。

**为什么用一维数组存二维坐标？** 这是故意的。如果我们用一个二维布尔数组来标记空地，随机选位置时就需要"先随机行，再随机列，然后检查是不是空地，不是就重试"——这很麻烦，而且当地图快满时空地很少，重试可能很多次。用一维数组一次性收集所有空地，然后 `rand() % count` 一把选出来，O(n) 收集，O(1) 挑选，干净利落。

#### 第 12 行：`game->map[cells[idx].row][cells[idx].col] = 'F';`

把选中格子的地图字符改成 `'F'`。`cells[idx]` 是一个 `Point` 结构体（`game.h` 里定义：`struct Point { int row; int col; };`），用 `.row` 和 `.col` 取出行列坐标，直接索引地图二维数组。

### 4.1.3 collect_empty_cells() —— 收集空地的帮手

`place_food()` 依赖 `collect_empty_cells()`，我们来看它的实现：

```cpp
// 收集地图上所有 '.' 格子的坐标到 cells 数组，返回收集数量
static int collect_empty_cells(GameState *game, Point cells[])
{
    int count = 0;
    int ms = game->map_size;
    for (int r = 0; r < ms; r++)
    {
        for (int c = 0; c < ms; c++)
        {
            if (game->map[r][c] == '.')
            {
                cells[count].row = r;
                cells[count].col = c;
                count++;
                // 边界保护：防止数组越界（经典模式不需要，但对战模式需要用）
                if (count >= MAX_SNAKE_LEN) return count;
            }
        }
    }
    return count;
}
```

逻辑极其直观：
1. `count = 0` — 计数器归零
2. 双重 for 循环遍历地图每一格
3. 如果格子是 `'.'`，把坐标写入 `cells[count]`，然后 `count++`
4. 循环结束后返回 count

这个函数可以说是 Snake 里**最简单的遍历-收集模式**。你会在后面的 AI 寻路、障碍物生成里反复看到类似的"遍历地图、收集符合条件格子"的代码。

### 4.1.4 常见错误

**错误 1：忘了检查 count <= 0**

```cpp
// WRONG ❌ —— 如果地图满了，cells 全空，idx 可能越界
static void place_food(GameState *game)
{
    Point cells[MAX_SNAKE_LEN];
    int count = collect_empty_cells(game, cells);
    int idx = rand() % count;  // 如果 count = 0，这里除零错误！
    game->map[cells[idx].row][cells[idx].col] = 'F';
}

// CORRECT ✅ —— 先检查再使用
static void place_food(GameState *game)
{
    Point cells[MAX_SNAKE_LEN];
    int count = collect_empty_cells(game, cells);
    if (count <= 0)
    {
        game->game_over = true;
        game->death_reason = DEATH_FULL_MAP;
        return;
    }
    int idx = rand() % count;
    game->map[cells[idx].row][cells[idx].col] = 'F';
}
```

C++ 里 `rand() % 0` 是**未定义行为**——程序可能崩溃、可能返回奇怪的值、可能看起来一切正常直到某天突然炸。永远在 `%` 之前检查除数不为零。

**错误 2：忘记设置 death_reason**

```cpp
// WRONG ❌ —— 游戏结束了，但渲染层不知道是"赢了"还是"死了"
if (count <= 0)
{
    game->game_over = true;
    // 忘记设置 death_reason，渲染层可能显示错误的信息
    return;
}

// CORRECT ✅
if (count <= 0)
{
    game->game_over = true;
    game->death_reason = DEATH_FULL_MAP;  // 告诉渲染层：这是胜利，不是撞死
    return;
}
```

`game_over` 只表示"游戏结束了"，但**为什么结束**由 `death_reason` 决定。`DEATH_FULL_MAP` 会让渲染层显示"YOU WIN!"而不是"GAME OVER"。

**错误 3：没有引入随机种子**

```cpp
// WRONG ❌ —— 每次运行，rand() 序列完全一样
int main()
{
    // 缺少 srand(time(NULL));
    GameState game;
    init_game(&game, MODE_CLASSIC);
    // 每次运行，食物出现的位置完全一样！
}

// CORRECT ✅ —— 在程序启动时设置一次种子
int main()
{
    srand((unsigned int)time(NULL));  // 用当前时间当种子
    GameState game;
    init_game(&game, MODE_CLASSIC);
    // 每次运行食物位置不同
}
```

### 4.1.5 练一练

**练习：写一个在地图上随机生成食物的程序**

要求：
1. 声明一个 `char map[10][10]`，先用 `'#'` 填满边界、`'.'` 填内部
2. 写一个 `collect_empty_cells` 函数和 `place_food` 函数
3. 调用 `place_food` 后，把整个地图打印出来
4. 食物位置用 `'F'` 显示

**预期输出**（每次运行食物位置不同）：

```
# # # # # # # # # #
# . . . . . . . . #
# . . . . . . . . #
# . . . F . . . . #    ← 'F' 出现在随机位置
# . . . . . . . . #
# . . . . . . . . #
# . . . . . . . . #
# . . . . . . . . #
# . . . . . . . . #
# # # # # # # # # #
```

### 参考答案

<details>
<summary>点击展开练习</summary>

```cpp
#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;

struct Point {
    int row;
    int col;
};

// 收集所有 '.' 格子的坐标
int collect_empty_cells(char map[10][10], Point cells[])
{
    int count = 0;
    for (int r = 1; r < 9; r++)  // 跳过边界
    {
        for (int c = 1; c < 9; c++)
        {
            if (map[r][c] == '.')
            {
                cells[count].row = r;
                cells[count].col = c;
                count++;
            }
        }
    }
    return count;
}

// 在随机空地放食物
void place_food(char map[10][10])
{
    Point cells[100];
    int count = collect_empty_cells(map, cells);
    if (count <= 0)
    {
        cout << "Map full! You win!" << endl;
        return;
    }
    int idx = rand() % count;
    map[cells[idx].row][cells[idx].col] = 'F';
}

// 打印地图
void print_map(char map[10][10])
{
    for (int r = 0; r < 10; r++)
    {
        for (int c = 0; c < 10; c++)
        {
            cout << map[r][c] << ' ';
        }
        cout << endl;
    }
}

int main()
{
    srand((unsigned int)time(NULL));

    // 初始化地图：边界 '#'，内部 '.'
    char map[10][10];
    for (int r = 0; r < 10; r++)
    {
        for (int c = 0; c < 10; c++)
        {
            if (r == 0 || r == 9 || c == 0 || c == 9)
                map[r][c] = '#';
            else
                map[r][c] = '.';
        }
    }

    place_food(map);
    print_map(map);

    return 0;
}
```
</details>

---

## 4.2 碰撞检测（上）—— can_enter_cell()

> **课时**：1 课时（约 1.5-2 小时）
> **目标**：理解碰撞检测的优先级顺序，能写出 `can_enter_cell()` 函数

### 4.2.1 为什么碰撞检测是"整个游戏最重要的一行判断"？

蛇每走一步，都要回答一个问题：**下一步蛇头要去的那个格子，能不能进去？** 这个问题看起来简单——"看看那个格子是空地就行了吧？"——但实际比你想的复杂得多。

请你想象这几种情况：
- 蛇头下一步是墙 `'#'` → 死，撞墙
- 蛇头下一步是障碍物 `'O'` → 死，撞障碍物
- 蛇头下一步是自己的尾巴 `'B'`（但尾巴马上要移开了） → **不死！安全！**
- 蛇头下一步是自己的脖子 `'B'`（尾巴移不开） → 死，撞自己
- 蛇头下一步是对方蛇头 `'A'` → 死，对撞

看到了吗？同样是 `'B'`，尾巴和脖子的判断结果完全不同。这就是碰撞检测的微妙之处。而且这些检查之间有**优先级**——你得先检查最危险的（墙），再检查次危险的，最后才是"安全通过"。

`can_enter_cell()` 就是来做这件事的。它接收蛇头要去的位置，看一眼那个格子里有什么，然后返回一个 `DeathReason` 枚举值——要么是某种死亡原因，要么是 `DEATH_NONE`（安全，请进）。

### 4.2.2 can_enter_cell() —— 逐行讲解

让我们直接看代码：

```cpp
// 检查蛇头能否进入 (row, col) 这个格子
// 返回值：DEATH_NONE 表示安全可进入；其他值表示死亡原因
// 注意：尾部安全检查不在这里做——由调用方（move_snake 等）负责
static DeathReason can_enter_cell(GameState *game, int row, int col)
{
    char ch = game->map[row][col];

    // 优先级 1：墙
    if (ch == '#')
        return DEATH_WALL;

    // 优先级 2：障碍物
    if (ch == 'O')
        return DEATH_OBSTACLE;

    // 优先级 3：自己的身体
    if (ch == 'B')
        return DEATH_BODY;

    // 优先级 4：对方身体
    if (ch == 'C')
        return DEATH_ENEMY_BODY;

    // 优先级 5：对方蛇头（头部对撞）
    if (ch == 'H' || ch == 'A')
        return DEATH_HEAD_ON;

    // 安全：空地 '.' 或食物 'F'
    return DEATH_NONE;
}
```

#### 逐行讲解

**第 3 行 `char ch = game->map[row][col];`**：把目标格子里的字符取出来。这行看似简单，但有一个好处——后面六个 if 判断都直接比较 `ch`，不需要反复写 `game->map[row][col]`。

**优先级 1——墙 `'#'`**：墙不可穿越，这是最绝对的死亡条件。检查它排在第一，因为如果撞了墙，后面什么都不用看了。

**优先级 2——障碍物 `'O'`**：经典模式下地图上有随机障碍物。和墙一样，撞了就死。

**优先级 3——自己的身体 `'B'`**：这是最常见的死亡原因。蛇长大了，转弯空间越来越小，自己把自己绕死了。注意：这个检查**不区分尾巴和脖子**——那是调用方的责任。`can_enter_cell()` 只管告诉你"那个格子是蛇身"，至于那是尾巴还是脖子，由 `move_snake()` 自己判断（我们下一节讲）。

**优先级 4——对方身体 `'C'`**：这是对战模式专用的。人机对战时，`'C'` 是 AI 蛇的身体。只有在对战/PvP 模式下地图里才有 `'C'`，经典模式下永远不会触发这个分支。

**优先级 5——对方蛇头 `'H'` 或 `'A'`**：头部对撞。两条蛇头碰头，一般两个都死（具体看游戏规则）。

**最后一行的 `return DEATH_NONE;`**：如果前面所有 if 都没触发，说明这个格子要么是空地 `'.'`，要么是食物 `'F'`——都是安全的。

#### 为什么是这个顺序？

你可能想问："能不能换顺序？比如先检查头对撞，再检查墙？"

答案是：**不能**。原因有二。

第一，**墙和障碍物是绝对的**。如果蛇头已经撞了墙，你不需要再告诉玩家"你还撞了对方蛇的身体"。死亡原因要返回最重要的那个——墙 > 障碍物 > 自己身体 > 对方身体 > 头部对撞。这是一个递减的严重程度。

第二，**逻辑包含关系**。在地图上，蛇身字符 `'B'` 可能恰好覆盖在墙 `'#'` 旁边。如果你先检查 `'B'` 再检查 `'#'`，蛇头接近墙时可能先被判定为"撞到自己身体"——但蛇身体理论上不应该出现在墙的位置上。先检查墙可以避免这种混乱。

### 4.2.3 常见错误

**错误 1：检查顺序随意**

```cpp
// WRONG ❌ —— 顺序混乱，墙壁检查排在后面
static DeathReason can_enter_cell(GameState *game, int row, int col)
{
    char ch = game->map[row][col];

    if (ch == 'B')           // 先查自己身体
        return DEATH_BODY;
    if (ch == 'H' || ch == 'A')  // 再查头对撞
        return DEATH_HEAD_ON;
    if (ch == '#')           // 最后才查墙？！
        return DEATH_WALL;
    // ...
}

// CORRECT ✅ —— 墙 → 障碍物 → 自己 → 对方 → 头对撞
static DeathReason can_enter_cell(GameState *game, int row, int col)
{
    char ch = game->map[row][col];
    if (ch == '#')     return DEATH_WALL;
    if (ch == 'O')     return DEATH_OBSTACLE;
    if (ch == 'B')     return DEATH_BODY;
    if (ch == 'C')     return DEATH_ENEMY_BODY;
    if (ch == 'H' || ch == 'A') return DEATH_HEAD_ON;
    return DEATH_NONE;
}
```

**错误 2：`'H'` 和 `'A'` 的含义搞混**

在 Snake 里，字符标记的含义是**从地图渲染角度定义的**：
- `'H'` = 玩家蛇头（Human head）
- `'A'` = AI 蛇头（AI head）
- `'B'` = 玩家蛇身（Body）
- `'C'` = AI 蛇身（AI body，C 是字母表里排在 B 后面）

这些字符定义在 `put_snake_on_map_impl()` 的参数里被传入——调用方决定用什么字符画哪条蛇。`can_enter_cell()` 只需要知道：**除了空地 `'.'` 和食物 `'F'` 以外，踩到任何其他字符都意味着碰撞**。

### 4.2.4 练一练

**练习：实现 can_enter_cell() 并测试**

写一个测试程序：
1. 声明一个 5x5 的地图，放几个不同字符
2. 实现 `can_enter_cell()` 函数
3. 对每个非空格的格子调用 `can_enter_cell()`，打印返回值

**预期输出**（根据你的地图布局，坐标从 0 开始；测试代码只打印非 `'.'` 格子）：

```
Cell (0,0) = '#' → DEATH_WALL (1)
Cell (0,1) = 'O' → DEATH_OBSTACLE (2)
Cell (0,2) = 'B' → DEATH_BODY (3)
Cell (0,4) = 'F' → DEATH_NONE (0)
Cell (1,0) = 'C' → DEATH_ENEMY_BODY (4)
Cell (1,1) = 'H' → DEATH_HEAD_ON (5)
```

### 参考答案

<details>
<summary>点击展开练习</summary>

```cpp
#include <iostream>
using namespace std;

// DeathReason 枚举（和 game.h 里一致）
enum DeathReason {
    DEATH_NONE = 0,
    DEATH_WALL = 1,
    DEATH_OBSTACLE = 2,
    DEATH_BODY = 3,
    DEATH_ENEMY_BODY = 4,
    DEATH_HEAD_ON = 5,
    DEATH_FULL_MAP = 6
};

// 碰撞检测
DeathReason can_enter_cell(char map[5][5], int row, int col)
{
    char ch = map[row][col];

    if (ch == '#')
        return DEATH_WALL;
    if (ch == 'O')
        return DEATH_OBSTACLE;
    if (ch == 'B')
        return DEATH_BODY;
    if (ch == 'C')
        return DEATH_ENEMY_BODY;
    if (ch == 'H' || ch == 'A')
        return DEATH_HEAD_ON;

    return DEATH_NONE;  // '.' 或 'F'
}

int main()
{
    char map[5][5] = {
        {'#', 'O', 'B', '.', 'F'},
        {'C', 'H', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.'}
    };

    const char *names[] = {
        "DEATH_NONE", "DEATH_WALL", "DEATH_OBSTACLE",
        "DEATH_BODY", "DEATH_ENEMY_BODY", "DEATH_HEAD_ON"
    };

    // 测试前两行
    for (int r = 0; r < 2; r++)
    {
        for (int c = 0; c < 5; c++)
        {
            if (map[r][c] != '.')
            {
                DeathReason dr = can_enter_cell(map, r, c);
                cout << "Cell (" << r << "," << c << ") = '"
                     << map[r][c] << "' → " << names[dr]
                     << " (" << dr << ")" << endl;
            }
        }
    }

    return 0;
}
```
</details>

---

## 4.3 碰撞检测（下）—— 尾部安全与 next_head()

> **课时**：1 课时（约 1.5-2 小时）
> **目标**：理解尾部安全的设计原理，能用 `next_head()` 计算蛇头下一位置

### 4.3.1 "尾巴问题"——碰撞检测最精致的设计

上一节我们讲的 `can_enter_cell()` 有一个"漏洞"：它把所有 `'B'` 都视为死亡。但实际上，蛇的**尾巴**是安全的——因为如果蛇这步不会变长，尾巴会移开，下一个位置就是空地。

让我们画一张图来理解。

```
蛇正在向右移动 →
不会变长（没吃食物，也不到自动增长回合）

  Step N                     Step N+1
  ╔═════╗                   ╔═════╗
  ║ H1 2║                   ║ ·H1 2║
  ║ B B3║    ───→           ║ B B3║
  ║ B B4║                   ║ B B4║
  ╚═════╝                   ╚═════╝

  蛇头 H 在 (0,1)
  尾巴 4 在 (2,2)
  下一步蛇头要去 (0,2)，那个格子现在是 'B'（蛇身！）
  但是——尾巴 4 会移开，(2,2) 变成 '.'，
  蛇身整体向前挪——所以 (0,2) 实际上是安全的！
```

如果蛇**不会变长**，尾巴移开后，整个蛇的长度不变，头往前一格，身体跟着挪，尾巴消失。所以下一步蛇头的位置虽然现在看起来是蛇身，但它其实是**尾巴将要离开的位置**——只要那个位置**恰好是尾巴**，它就是安全的。

但如果那个 `'B'` 不是尾巴（比如是蛇的第 2 节、第 3 节），那就不安全了——因为身体中间的部分不会移开，蛇头撞上去就是自杀。

这个设计叫**尾部安全**（Tail Safety），是贪吃蛇游戏的经典处理方式。

### 4.3.2 核心判断：will_grow

那么怎么判断"蛇会不会变长"？答案在 `will_grow` 这个变量：

```cpp
// 蛇会不会在这一步变长？
bool will_eat = (game->map[next.row][next.col] == 'F');  // 吃到食物就会长
bool auto_grow = ((game->move_count + 1) % game->grow_step) == 0;  // 自动增长
bool will_grow = will_eat || auto_grow;  // 二者满足其一就长
```

- **`will_eat`**：蛇头下一步的位置是不是食物 `'F'`？如果是，就吃到了，身体会变长。
- **`auto_grow`**：是不是到了自动增长的回合？（这个我们在 4.4 节细讲）
- **`will_grow`**：只要吃到食物或者自动增长满足其一，蛇就会变长。

**关键逻辑**：如果 `will_grow == true`，蛇会变长，尾巴**不会**移开——所以撞任何 `'B'` 都死。如果 `will_grow == false`，蛇不变长，尾巴会移开——撞到的 `'B'` 如果是尾巴，安全；否则还是死。

### 4.3.3 尾部安全检测的代码

现在我们把这个逻辑翻译成代码：

```cpp
// 在 move_snake() 中，计算出 next（下一步蛇头位置）后：

DeathReason reason = can_enter_cell(game, next.row, next.col);

// 如果 can_enter_cell 返回 DEATH_BODY，需要进一步判断：是不是尾巴？
if (reason == DEATH_BODY)
{
    // 注意：走到这里说明 next 格子上是 'B'（蛇身），所以 will_eat 一定为 false。
    // 但为了代码的完整性和可读性，这里仍然显示了完整的 will_grow 计算。
    // auto_grow 才是这个分支里真正影响结果的变量。
    bool will_eat = (game->map[next.row][next.col] == 'F');  // 此处恒为 false
    bool auto_grow = ((game->move_count + 1) % game->grow_step) == 0;
    bool will_grow = will_eat || auto_grow;
    // 等价于：bool will_grow = auto_grow;
    //         （因为 will_eat 在此分支中恒为 false）


    if (!will_grow)
    {
        // 蛇不会变长 → 尾巴会移开 → 检查 next 是不是尾巴
        Point tail = game->snake[game->length - 1];
        if (is_same_point(next, tail))
        {
            reason = DEATH_NONE;  // 撞的是尾巴，安全！
        }
    }
    // 如果 will_grow == true，reason 保持 DEATH_BODY → 真的撞死了
}

if (reason != DEATH_NONE)
{
    // 游戏结束
    game->game_over = true;
    game->death_reason = reason;
    return;
}
```

这段代码的执行流程是：
1. 先调 `can_enter_cell()` 得到初步判断
2. 如果返回的是 `DEATH_BODY`（撞到自己身体），**不要急着判定死亡**
3. 检查蛇会不会变长。如果不会 → 尾巴会移开 → 检查 `next` 是不是恰好是尾巴位置
4. 用 `is_same_point(next, tail)` 比较两个 Point——如果一样，`reason` 改回 `DEATH_NONE`
5. 如果 `reason` 还是非 NONE，才是真的死亡

### 4.3.4 is_same_point() —— 判断两个点是否重合

这是一个很短的辅助函数：

```cpp
static bool is_same_point(Point a, Point b)
{
    return a.row == b.row && a.col == b.col;
}
```

两个点"相同"当且仅当它们的行号相同且列号相同。`&&` 是"与"运算符——两边都成立才算 true。

在尾部安全的逻辑里，我们用 `is_same_point(next, tail)` 来判断"蛇头下一步要去的格子是不是恰好就是尾巴现在的位置"。如果是，且蛇不变长，则安全通过。

### 4.3.5 next_head() —— 根据方向和当前位置算下一步

说了这么多"下一步"，下一步到底在哪？这就需要一个函数来算：

```cpp
// 给定当前蛇头位置和方向，返回蛇头下一步的坐标
static Point next_head(Point head, Direction dir)
{
    Point next = head;
    switch (dir)
    {
        case UP:    next.row--; break;
        case DOWN:  next.row++; break;
        case LEFT:  next.col--; break;
        case RIGHT: next.col++; break;
    }
    return next;
}
```

逻辑非常直观：
- **向上（UP）**：行号减 1（row--），列号不变
- **向下（DOWN）**：行号加 1（row++），列号不变
- **向左（LEFT）**：行号不变，列号减 1（col--）
- **向右（RIGHT）**：行号不变，列号加 1（col++）

注意：这里**不做边界检查**。`next_head()` 只负责"算位置"，不管那个位置是不是合法。合不合法由调用方用 `can_enter_cell()` 去判断。这是一个好的设计原则：**一个函数只做一件事**。

### 4.3.6 碰撞场景全景图

让我们把各种碰撞场景画出来。蛇头用 `H` 表示，前进方向用箭头标出。

```
场景1：撞墙                             场景2：撞障碍物
  ╔═════╗                                ╔═════╗
  ║ H→# ║  ← 蛇头向右，墙挡住              ║ H→O ║  ← 蛇头向右，障碍物挡住
  ║  .  ║     DEATH_WALL                  ║  .  ║     DEATH_OBSTACLE
  ╚═════╝                                ╚═════╝

场景3：撞自己身体（非尾巴）                 场景4：尾巴安全
  ╔═════╗                                ╔═════╗
  ║ H B ║  ← 蛇头向右，身体在前面             ║ H→B ║  ← B 是尾巴，且不会变长
  ║ B B ║     DEATH_BODY                  ║ B ══╝     安全通过！DEATH_NONE
  ╚═════╝                                ╚═════╝

场景5：头部对撞（对战模式）                  场景6：撞对方身体（对战模式）
  ╔═════╗                                ╔═════╗
  ║ ·H→ ←A  ║  两个蛇头面对面               ║ H→C ║  蛇头撞到对方身体
  ║  ·  ·  ║  DEATH_HEAD_ON               ║  .  ║  DEATH_ENEMY_BODY
  ╚═════╝                                ╚═════╝
```

### 4.3.7 常见错误

**错误 1：尾巴安全放在哪里检查？**

这是一个设计选择问题，不是对错问题。本章的教学版 `can_enter_cell()` 只接受 `(row, col)` 两个参数，**不**做尾部安全检查——它返回 `DEATH_BODY`，由 `move_snake()` 调用方去判断是不是尾巴。但实际项目中的 `can_enter_cell()` 签名更长，多传了 `will_grow` 和 `body` 参数，尾部安全检查就写在它里面。

两种设计各有道理：
- **分离式（本章教学版）**：`can_enter_cell` 只管"这个格子是什么"，尾部安全是 `move_snake` 的局部逻辑——职责清晰，初学者好理解。
- **集中式（实际项目版）**：`can_enter_cell` 一口气返回最终判断——减少调用方的心智负担，但函数签名更复杂。

完整的实际签名长这样：

```cpp
// 实际项目中的完整签名（参考 game.cpp 源码）
static bool can_enter_cell(GameState *game, Point next, bool will_grow,
                            const Point *body, int length, DeathReason *out_reason);
//                                                      ^^^^^^^^^^^^^^^^^^^^^^^^
//              多了 will_grow 和 body 参数 → 可以在内部做尾部安全检查
```

**关键**：不管你用哪种设计，别忘了尾部安全这个逻辑就行——至于把它放在哪，看你的函数怎么划分职责。初学者建议先用本章的"分离式"理解清楚再化简。

**错误 2：用 == 直接比较两个 Point 结构体**

```cpp
// WRONG ❌ —— C 语言不能直接用 == 比较结构体
Point next = {1, 2};
Point tail = {1, 2};
if (next == tail)  // 编译错误！结构体不能直接 ==
{ /* ... */ }

// CORRECT ✅ —— 必须写一个比较函数
static bool is_same_point(Point a, Point b)
{
    return a.row == b.row && a.col == b.col;
}
if (is_same_point(next, tail))
{ /* ... */ }
```

> **注意**：在 C++ 里可以通过重载 `operator==` 来让 `Point == Point` 编译通过。但 Snake 这个项目的代码风格是 C 风格的（用 `static` 函数而非运算符重载），所以用显式的比较函数更一致。

**错误 3：next_head() 里忘记先复制 head**

```cpp
// WRONG ❌ —— 直接修改了参数（虽然参数是值传递，不影响调用方，但不够清晰）
static Point next_head(Point head, Direction dir)
{
    switch (dir)  // 直接改 head，没有声明 next
    {
        case UP:    head.row--; break;
        // ...
    }
    return head;
}

// CORRECT ✅ —— 声明一个局部变量，明确意图是"计算新位置"
static Point next_head(Point head, Direction dir)
{
    Point next = head;  // 先复制一份
    switch (dir)
    {
        case UP:    next.row--; break;
        case DOWN:  next.row++; break;
        case LEFT:  next.col--; break;
        case RIGHT: next.col++; break;
    }
    return next;
}
```

虽然两种写法在 C 的值传递下结果完全一样（`head` 是副本，改它不影响调用方），但第二种写法意图更清晰——你在"计算一个**新**位置"，不是在"修改旧位置"。

### 4.3.8 练一练

**练习 1：实现 next_head() 并验证**

写一个程序，声明一个 `Point head = {5, 5}`，对四个方向分别调用 `next_head()`，打印结果。

**预期输出**：

```
head = (5, 5)
UP    → (4, 5)
DOWN  → (6, 5)
LEFT  → (5, 4)
RIGHT → (5, 6)
```

**练习 2：模拟尾部安全判断**

声明一个蛇身数组 `snake[4] = {{2,3}, {2,2}, {2,1}, {3,1}}`（长度为 4 的蛇，向右移动）。模拟蛇头向右走一步到 `(2,4)`。判断：如果 will_grow = false，`(2,4)` 安全吗？（提示：检查 `(2,4)` 是不是尾巴位置。）

**预期答案**：`(2,4)` 不是尾巴 `(3,1)`，所以如果 `(2,4)` 上有 `'B'`（身体），那就是真的撞自己了，不安全。

### 参考答案

<details>
<summary>点击展开练习 1</summary>

```cpp
#include <iostream>
using namespace std;

struct Point { int row; int col; };

enum Direction { UP, DOWN, LEFT, RIGHT };

Point next_head(Point head, Direction dir)
{
    Point next = head;
    switch (dir)
    {
        case UP:    next.row--; break;
        case DOWN:  next.row++; break;
        case LEFT:  next.col--; break;
        case RIGHT: next.col++; break;
    }
    return next;
}

int main()
{
    Point head = {5, 5};
    cout << "head = (" << head.row << ", " << head.col << ")" << endl;
    cout << "UP    → (" << next_head(head, UP).row << ", "
         << next_head(head, UP).col << ")" << endl;
    cout << "DOWN  → (" << next_head(head, DOWN).row << ", "
         << next_head(head, DOWN).col << ")" << endl;
    cout << "LEFT  → (" << next_head(head, LEFT).row << ", "
         << next_head(head, LEFT).col << ")" << endl;
    cout << "RIGHT → (" << next_head(head, RIGHT).row << ", "
         << next_head(head, RIGHT).col << ")" << endl;
    return 0;
}
```
</details>

<details>
<summary>点击展开练习 2</summary>

```cpp
#include <iostream>
using namespace std;

struct Point { int row; int col; };

bool is_same_point(Point a, Point b)
{
    return a.row == b.row && a.col == b.col;
}

int main()
{
    Point snake[4] = { {2,3}, {2,2}, {2,1}, {3,1} };  // 头到尾巴
    int length = 4;

    Point next = {2, 4};  // 蛇头向右走一步
    Point tail = snake[length - 1];  // (3,1)

    cout << "Next head: (" << next.row << ", " << next.col << ")" << endl;
    cout << "Tail: (" << tail.row << ", " << tail.col << ")" << endl;

    if (is_same_point(next, tail))
        cout << "SAFE: next IS the tail, tail will move away" << endl;
    else
        cout << "DANGER: next is NOT the tail → collision!" << endl;

    return 0;
}
```

输出：
```
Next head: (2, 4)
Tail: (3, 1)
DANGER: next is NOT the tail → collision!
```

`(2,4)` 和 `(3,1)` 不是同一个点，所以如果 `(2,4)` 上有蛇身 `'B'`，那就是真的撞到了，蛇会死。
</details>

---

## 4.4 自动增长机制（1课时）

> **课时**：1 课时（约 1.5-2 小时）
> **目标**：理解自动增长的设计目的和实现，能调节参数感受游戏节奏

### 4.4.1 为什么需要"自动增长"？

如果你玩过原版贪吃蛇，你会发现一个现象：**即使你没吃到豆子，蛇也会自己变长**。这不是 bug，是一个精心设计的机制。

想想看：如果蛇只有吃食物才变长，那这游戏也太简单了——你只要躲着食物走，蛇永远不变长，你可以在空地上绕圈绕到天荒地老。但有了自动增长，你的蛇**每走 8 步就长一节**。这意味着时间是你的敌人——越拖延，蛇越长，空间越小，难度越大。你必须主动去吃食物来得分，而不是被动躲避。

这就是自动增长的策略意义：**逼玩家主动行动**。

### 4.4.2 计算公式

自动增长的判断只有一行，但每一部分都值得拆开看：

```cpp
bool auto_grow = ((game->move_count + 1) % game->grow_step) == 0;
```

逐层拆解：

- **`game->move_count`**：蛇已经移动了多少步（从游戏开始计）。每走一步 +1。
- **`+ 1`**：因为我们要判断的是"下一步"是否触发增长。比如 `GROW_STEP = 8`，我们希望第 7 步、第 15 步、第 23 步时触发（0-indexed）。`7 + 1 = 8`，`8 % 8 = 0`，精确命中。
- **`% game->grow_step`**：取余数。`game->grow_step` 默认是 8（`GROW_STEP` 宏）。所以这一步的意思是："下一步是不是第 8 的倍数步？"
- **`== 0`**：余数为 0 表示整除，即到了自动增长的回合。

**具体例子**（`grow_step = 8`）：

| move_count | move_count + 1 | (move_count+1) % 8 | auto_grow? |
|------------|----------------|---------------------|------------|
| 0 | 1 | 1 | false |
| 6 | 7 | 7 | false |
| 7 | 8 | 0 | **true** ← 第 8 步触发 |
| 8 | 9 | 1 | false |
| 14 | 15 | 7 | false |
| 15 | 16 | 0 | **true** ← 第 16 步触发 |

看到了吗？`auto_grow` 每 8 步触发一次。游戏跑了 80 步，蛇就自动长了 10 节。

### 4.4.3 自动增长如何生效？

自动增长不是单独触发的——它和食物逻辑一起作用：

```cpp
// 在 move_snake() 函数中，计算出 next 后：

// 判断是否会吃食物
bool will_eat = (game->map[next.row][next.col] == 'F');

// 判断是否会自动增长
bool auto_grow = ((game->move_count + 1) % game->grow_step) == 0;

// 取或：二者满足其一，蛇就不删尾巴（等于长了一节）
bool will_grow = will_eat || auto_grow;
```

蛇身体的移动逻辑是：
1. 算出新头部位置 `next`
2. 身体全体前移一格：`snake[i] = snake[i-1]`（从尾巴到脖子）
3. 新头部 `snake[0] = next`
4. **关键**：如果 `will_grow == false` → 尾巴那一节被"移除"了（长度不变）；如果 `will_grow == true` → 尾巴保留（长度 +1）

所以"生长"的本质是：**不删尾巴**。吃食物时不删尾巴、自动增长回合不删尾巴——两种情况下身体都变长一节。

### 4.4.4 pending_growth —— 击杀奖励

在 Snake 里，还有一个与增长相关的字段：`pending_growth`。

```cpp
game->pending_growth;    // 玩家的待应用增长数
game->ai_pending_growth; // AI 的待应用增长数
```

这是什么？在对战模式中，如果你把对方蛇杀了，你获得额外的增长点数。比如你击杀 AI 蛇后 `pending_growth += 5`。在你接下来的 5 步里，每一步蛇都会多长一节（消耗 `pending_growth`），直到耗尽为止。

这是一种**击杀奖励**——杀敌不仅得分，还让你的蛇暴长一段，增加压迫力。

```cpp
// 简化示意：应用 pending_growth
if (game->pending_growth > 0)
{
    will_grow = true;        // 这一帧也长
    game->pending_growth--;  // 消耗一点
}
```

### 4.4.5 GROW_STEP 调参实验

`GROW_STEP` 的默认值是 8。如果我们改它，游戏体验会发生什么变化？

| GROW_STEP | 增长频率 | 游戏体验 |
|-----------|---------|---------|
| 3 | 每 3 步长一节 | 极快节奏，蛇迅速变长，空间压力巨大，高手模式 |
| 5 | 每 5 步长一节 | 快节奏，需要频繁计算路线 |
| 8 | 每 8 步长一节 | **标准节奏**，有足够时间规划，难度适中 |
| 12 | 每 12 步长一节 | 慢节奏，有充裕时间绕圈 |
| 20 | 每 20 步长一节 | 非常慢，食物成为主要增长来源，偏休闲 |

这个参数就是游戏的**难度旋钮**。在课程项目里它是常量，但你完全可以把它改成变量——比如随着分数提高逐步减小 `grow_step`，让游戏越往后越快。

### 4.4.6 常见错误

**错误 1：忘记 +1**

```cpp
// WRONG ❌ —— 第一步就触发增长！
bool auto_grow = (game->move_count % game->grow_step) == 0;
// move_count = 0 时，0 % 8 = 0 → true！游戏刚开始蛇就长了

// CORRECT ✅
bool auto_grow = ((game->move_count + 1) % game->grow_step) == 0;
// move_count = 0 时，(0+1) % 8 = 1 → false，正确
```

**错误 2：will_grow 只算了吃食物，忘了自动增长**

```cpp
// WRONG ❌ —— 自动增长被吞了
bool will_eat = (game->map[next.row][next.col] == 'F');
bool will_grow = will_eat;  // 只算了吃食物
// 结果：蛇永远不长，除非吃食物。游戏毫无挑战性。

// CORRECT ✅
bool will_eat = (game->map[next.row][next.col] == 'F');
bool auto_grow = ((game->move_count + 1) % game->grow_step) == 0;
bool will_grow = will_eat || auto_grow;  // 两者都考虑
```

### 4.4.7 练一练

**练习：改变 GROW_STEP 感受节奏**

在你的 Snake 项目里（如果你已经有了完整代码），把 `GROW_STEP` 从 8 改成：
1. 3 —— 玩一局，感受节奏
2. 20 —— 玩一局，感受节奏

记录两种情况下：
- 你在 30 步内的蛇长度
- 你感觉的难度变化
- 你的策略有什么不同

> **提示**：如果你还没有完整代码，可以先手工模拟：拿一张纸画 20x20 网格，标记蛇的初始位置，每走一步记录蛇的长度。GROW_STEP=3 时每 3 步加一节，GROW_STEP=20 时每 20 步加一节。看着纸上的蛇，你能直观感受到节奏差异。

### 参考答案

<details>
<summary>点击展开分析</summary>

**GROW_STEP = 3 时的体验**：
- 30 步后，自动增长触发 10 次 → 蛇长了 10 节（初始 3 节 → 大约 13 节，不含吃食物）
- 节奏极快，你几乎没时间思考——每 3 步就多一节，蛇迅速占满空间
- 策略：必须主动规划路线，留出足够空间。不能绕圈，因为绕圈过程中身体就暴长了
- 你可能在不到 100 步就死了——因为空间太小

**GROW_STEP = 20 时的体验**：
- 30 步后，自动增长触发 1 次（第 19 步）→ 蛇只长了 1 节
- 节奏悠闲，蛇增长几乎全靠吃食物
- 策略：可以悠闲绕圈，游戏变成"纯吃食物"模式，压力很小
- 你可能能玩好几分钟不死——地图够大，蛇够短

**结论**：`GROW_STEP = 8` 是一个很好的平衡点。它给初学者足够的时间适应，同时又有持续的增长压力，让游戏不会无聊。
</details>

---

## 4.5 完整经典模式串联（1课时）

> **课时**：1 课时（约 2-3 小时）
> **目标**：把所有代码模块串联起来，理解 init_game() 的完整流程，能写出经典模式的 main 框架

### 4.5.1 前面学了什么

在进入"串联"之前，让我们回顾一下整个游戏系统已经覆盖的知识：

| 章节 | 内容 | 你学到的 |
|------|------|---------|
| 第〇章 | 环境配置 | VS 2022 + EasyX 安装 |
| 第一章 | C++ 基础 | 变量、if/for/while、函数、数组、结构体、指针 |
| 第二章 | 地图与蛇身 | 二维数组表示地图、Point 结构体、枚举 |
| 第三章 | 渲染 | EasyX 绘图、draw_board、draw_panel |
| 第四章（本章） | 游戏逻辑 | place_food、can_enter_cell、尾部安全、自动增长 |

现在，我们要把这一切**串成一个可以玩的游戏**。这个串联函数就是 `init_game()`——它负责创建一局新游戏的全部初始状态。

### 4.5.2 init_game() —— 逐行讲解

下面是经典模式 `init_game()` 的完整实现（简化版，只含核心逻辑）：

```cpp
void init_game(GameState *game, GameMode mode)
{
    // ===== 第一步：设置地图参数 =====
    game->mode = mode;
    game->map_size = MAP_SIZE;  // 经典模式 20x20

    // ===== 第二步：清空+画边界 =====
    for (int r = 0; r < game->map_size; r++)
    {
        for (int c = 0; c < game->map_size; c++)
        {
            if (r == 0 || r == game->map_size - 1 ||
                c == 0 || c == game->map_size - 1)
                game->map[r][c] = '#';  // 边界墙
            else
                game->map[r][c] = '.';  // 内部空地
        }
    }

    // ===== 第三步：初始化玩家蛇 =====
    game->length = 3;
    game->snake[0] = {8, 5};   // 蛇头 —— 第 8 行第 5 列
    game->snake[1] = {8, 4};   // 身1 —— 第 8 行第 4 列
    game->snake[2] = {9, 4};   // 身2（尾巴）—— 第 9 行第 4 列
    game->direction = UP;      // 初始方向向上（与游戏源码一致）

    // 把蛇画到地图上
    for (int i = 0; i < game->length; i++)
    {
        Point p = game->snake[i];
        game->map[p.row][p.col] = (i == 0) ? 'H' : 'B';
    }

    // ===== 第四步：初始化状态变量 =====
    game->score = 0;
    game->move_count = 0;
    game->food_eaten = 0;
    game->pending_growth = 0;
    game->grow_step = GROW_STEP;  // 默认 8
    game->started = false;
    game->paused = false;
    game->game_over = false;
    game->turn_locked = false;
    game->death_reason = DEATH_NONE;

    // ===== 第五步：放障碍物 =====
    generate_obstacles(game, 12);  // 经典模式放 12 个障碍物

    // ===== 第六步：放第一个食物 =====
    game->map[8][6] = 'F';  // 故意放在蛇头附近（蛇头在 (8,5)，食物在 (8,6)）
}
```

#### 逐行讲解

**第一步：设置地图参数**。`map_size = 20` 决定了后面的循环范围。

**第二步：清空 + 画边界**。双重 for 循环，遇到边界（第 0 行、第 19 行、第 0 列、第 19 列）设为 `'#'`，内部设为 `'.'`。这一步之后，地图是一个 20x20 的"中空矩阵"，外面一圈墙，里面全是空地。

**第三步：初始化蛇身**。这是最有意思的部分。初始蛇的位置是：

```
  列 4  列 5  列 6
  ╔═════════════╗
  ║ B1  H   F  ║  行 8：身1 → 头 → 食物
  ║ B2(尾)     ║  行 9：尾巴
  ╚═════════════╝
```

- `snake[0] = {8, 5}`：蛇头在 (8, 5)
- `snake[1] = {8, 4}`：第一节身体在 (8, 4)，头左边
- `snake[2] = {9, 4}`：尾巴在 (9, 4)，是一个拐角

```
  列 4  列 5  列 6
  ╔═════════════╗
  ║ B1   H   F  ║  行 8：身1 → 头(H) → 食物
  ║ B2(尾)      ║  行 9：尾巴
  ╚═════════════╝
  方向：UP（向上）
  玩家按 W / ↑ → 蛇头从 (8,5) 走向 (7,5)
```

**为什么是这个形状？** 蛇头在 (8,5)，初始方向是 `UP`（向上）。身体第一节在头左边 (8,4)，尾巴拐到下面 (9,4)——这是一个"L"形。玩家开局看到一条向上弯的蛇，食物 `F` 在 (8,6)（头右边一格），所以玩家有两个选择：直行（按 W 往上走）或右转吃食物（按 D 后按 W 爬上去）——这个设计给了初学者一个清晰的"动作-反馈"循环。

注意这三节的身形是通过 `i == 0 ? 'H' : 'B'` 来区分头和身体的：索引 0 是头 `'H'`，其他都是身体 `'B'`。

**第四步：初始化状态**。把 game 结构体里的计数字段全部归零。这些字段的初始值都很有讲究：
- `move_count = 0`：还没走一步，第 7 步才会触发 auto_grow（因为 `(0+1)%8 != 0`）
- `started = false`：游戏处于"等待玩家按第一个键"状态
- `turn_locked = false`：允许接收方向输入

**第五步：放障碍物**。`generate_obstacles(game, 12)` 在空地上随机放 12 个 `'O'`。注意：障碍物生成前蛇已经在地图上了（`'H'` 和 `'B'` 已写入 map），所以障碍物不会覆盖蛇的位置。`generate_obstacles` 内部有保护逻辑——最多只占可放置格子的 25%。

**第六步：放第一个食物**。`game->map[8][6] = 'F'`。注意这里用的是**硬编码坐标** `(8, 6)`，而不是调 `place_food()` 去随机放。为什么？因为这是游戏的第一帧，玩家还没开始动。把食物放在蛇头正前方一格（蛇头在 (8,5)，方向右，食物在 (8,6)），玩家按一下右键就能吃到——这是一个**新手引导设计**：第一个食物几乎白送，让玩家立刻体验"吃到了！"的爽感。

### 4.5.3 整局游戏的执行流程

下面是一张文字版流程图，帮你理解从游戏启动到结束发生了什么：

```
游戏启动
  │
  ▼
init_game(&game, MODE_CLASSIC)
  ├── 地图 20x20，边界 '#'
  ├── 蛇初始位置 (8,5)(8,4)(9,4)，方向 RIGHT
  ├── 12 个随机障碍物 'O'
  └── 食物在 (8,6)
  │
  ▼
══════ 游戏主循环 ══════
  │                      ▲
  ▼                      │
等待键盘输入             │
  ├── W/A/S/D → 改变方向  │
  └── 其他输入（空格暂停） │
  │                      │
  ▼                      │
定时器触发（每 160ms）    │
  │                      │
  ▼                      │
move_snake(&game)         │
  ├── next_head() 算下一步 │
  ├── can_enter_cell() 碰撞检查
  │     ├── 撞墙/障碍物/自己 → game_over
  │     └── 尾部安全检查
  ├── 判断 will_eat / auto_grow
  │     ├── 吃到食物 → 加分 + place_food()
  │     └── 自动增长
  ├── 身体前移（删不删尾巴取决于 will_grow）
  └── move_count++
  │                      │
  ▼                      │
draw_game(&game)          │
  ├── draw_board() 画棋盘 │
  ├── draw_panel() 画信息 │
  └── 如果 game_over → 画结束画面
  │                      │
  ▼                      │
如果 game_over == false ──┘
  │
  ▼
如果 game_over == true
  │
  ▼
显示 "GAME OVER" 或 "YOU WIN"
等待玩家按 R 重新开始 或 ESC 退出
```

### 4.5.4 generate_obstacles() 是怎么防止放太多的？

在 `init_game()` 里调了 `generate_obstacles(game, 12)`。这个函数你不需要完全理解，但要知道它的安全机制：

```cpp
void generate_obstacles(GameState *game, int count)
{
    int ms = game->map_size;
    int max_obstacles = (ms - 2) * (ms - 2) / 4;  // 可放置格子数的 25%

    if (count > max_obstacles)
        count = max_obstacles;  // 裁剪到上限，防止放太多

    int placed = 0;
    while (placed < count)
    {
        int r = rand() % (ms - 2) + 1;  // 避开边界
        int c = rand() % (ms - 2) + 1;
        if (game->map[r][c] == '.')  // 只放在空地上
        {
            game->map[r][c] = 'O';
            placed++;
        }
    }
}
```

关键点：`max_obstacles` 是硬上限——20x20 地图内部有 18x18 = 324 格，除以 4 = 81。也就是说即使你传了 1000，也最多只放 81 个。这个机制确保**地图不会被障碍物塞满**，留足够的空间给蛇移动。

### 4.5.5 游戏状态机

经典模式下的游戏有以下几个状态，由 `GameState` 的几个 bool 字段管理：

```
                        ┌──────────┐
          init_game() → │  等待输入  │ started=false
                        └─────┬────┘
                              │ 玩家按第一个键
                              ▼
                        ┌──────────┐
               ┌─────── │  游戏中   │ game_over=false
               │ 暂停    └─────┬────┘
               │              │ 撞墙/撞自己/地图满
          按空格切换            ▼
               │        ┌──────────┐
               └─────── │ Game Over │ game_over=true
                        └─────┬────┘
                              │ 按 R
                              ▼
                        restart_game() ──→ 回到 init_game()
                              │ 按 ESC
                              ▼
                          返回主菜单
```

### 4.5.6 常见错误

**错误 1：init_game 里忘记设置 started = false**

```cpp
// WRONG ❌ —— 开局蛇就自己跑了
game->started = true;  // 忘记改成 false

// CORRECT ✅
game->started = false;  // 等待玩家按第一个键
```

`started = false` 时，`move_snake()` 的第一段代码会提前 return，不移动蛇。只有玩家按了方向键，`started` 才设为 `true`，蛇才开始走。这是一个"预备"状态的实现。

**错误 2：init_game 里调 place_food() 而非硬编码第一个食物的坐标**

```cpp
// 可以，但不推荐 —— 食物可能出现在地图另一头，新手引导失效
place_food(game);

// 更好 ✅ —— 第一个食物故意放在蛇头旁边，给新手立即可达的目标
game->map[8][6] = 'F';
```

两种写法都能让游戏正常运行。`place_food()` 会随机放食物，功能上完全正确。但把第一个食物硬编码在蛇头旁边 `(8,6)` 是一个**用户体验优化**——新手按一两下方向键就能吃到，立刻建立"能动、能吃、能得分"的正反馈。这是一个设计选择，不是对错问题。

**错误 3：忘记初始化 pending_growth**

```cpp
// WRONG ❌ —— 垃圾值
// 什么都没写，pending_growth 可能是 328947

// CORRECT ✅
game->pending_growth = 0;
```

`GameState` 结构体在栈上分配时，它的字段不会被自动清零！如果你不显式初始化每个字段，它们里面的值是"随机的"——上一帧栈内存留下的垃圾。这就是为什么 `init_game()` 里有一大串 `= 0` 和 `= false` 的赋值。

### 4.5.7 练一练

**练习：写出经典模式的 main 函数框架**

不要求能跑（因为你还没有完整代码），但要求逻辑结构正确。写出以下内容：

1. `init_game()` 调用
2. 游戏主循环 `while (!game.game_over)`
3. 在循环内处理键盘输入（W/A/S/D 改变方向，空格暂停）
4. 在循环内调用 `move_snake()`
5. 在循环内调用 `draw_game()`
6. 游戏结束后显示 "Game Over" 并等待 R 键重开

用**伪代码 + 注释**写出框架即可，不需要每个函数的完整实现。

**框架结构参考**：

```
int main()
{
    // 1. 初始化随机种子和图形窗口
    // 2. 声明 GameState 并调用 init_game()
    // 3. 游戏主循环
    // 4. 退出时的清理
}
```

### 参考答案

<details>
<summary>点击展开练习</summary>

```cpp
#include "game.h"
#include "render.h"
#include "input.h"
#include <graphics.h>  // EasyX
#include <ctime>
#include <windows.h>

int main()
{
    // ===== 1. 初始化 =====
    srand((unsigned int)time(NULL));          // 随机种子
    initgraph(CLASSIC_WINDOW_WIDTH, CLASSIC_WINDOW_HEIGHT);  // 打开图形窗口
    disable_ime();                            // 禁用输入法
    BeginBatchDraw();                         // 开启双缓冲

    // ===== 2. 创建并初始化游戏 =====
    GameState game;
    init_game(&game, MODE_CLASSIC);

    // ===== 3. 游戏主循环 =====
    DWORD last_move = GetTickCount();  // 上次移动的时间戳

    while (!game.game_over)
    {
        // ---- 3a. 处理键盘输入 ----
        if (key_pressed('W') || key_pressed(VK_UP))
            set_direction(&game, UP);
        if (key_pressed('S') || key_pressed(VK_DOWN))
            set_direction(&game, DOWN);
        if (key_pressed('A') || key_pressed(VK_LEFT))
            set_direction(&game, LEFT);
        if (key_pressed('D') || key_pressed(VK_RIGHT))
            set_direction(&game, RIGHT);

        if (key_pressed(VK_SPACE))
            game.paused = !game.paused;  // 切换暂停

        if (key_pressed(VK_ESCAPE))
            break;  // 退出游戏

        // ---- 3b. 定时移动 ----
        if (!game.paused)
        {
            DWORD now = GetTickCount();
            int interval = key_held(VK_SHIFT) ? MOVE_INTERVAL_MS / 3
                                              : MOVE_INTERVAL_MS;
            if (now - last_move >= (DWORD)interval)
            {
                move_snake(&game);
                last_move = now;
            }
        }

        // ---- 3c. 渲染 ----
        draw_game(&game);
        FlushBatchDraw();
        Sleep(16);  // 约 60 FPS
    }

    // ===== 4. 游戏结束后的处理 =====
    // 停留几秒让玩家看结束画面
    // 等待玩家按 R 重开 或 ESC 退出
    // （实际项目中会在这里循环等待输入）

    // ===== 5. 清理 =====
    EndBatchDraw();
    closegraph();
    return 0;
}
```

> **说明**：这个框架里的 `move_snake()`、`set_direction()`、`draw_game()` 等函数来自 `game.h` 和 `render.h` 的声明。如果你使用的是完整的 Snake 项目代码，把各 `.cpp` 文件编译链接即可运行。如果你目前只有部分代码，这个框架可以作为后续填充的目标——当你写完所有模块后，把函数实现填进去即可。
</details>

---

## 本章回顾

这一章我们完成了经典贪吃蛇最核心的游戏逻辑：

| 节 | 主题 | 核心函数 | 关键概念 |
|----|------|---------|---------|
| 4.1 | 食物生成 | `place_food()`、`collect_empty_cells()` | 一维数组收集二维空地，`rand() % count` 随机选 |
| 4.2 | 碰撞检测（上） | `can_enter_cell()` | 五级优先级：墙 > 障碍物 > 自己 > 对方 > 头对撞 |
| 4.3 | 碰撞检测（下） | `next_head()`、`is_same_point()` | 尾部安全：`will_grow` 决定尾巴是否移开 |
| 4.4 | 自动增长 | `auto_grow` 公式 | `(move_count+1) % GROW_STEP == 0`，`pending_growth` 击杀奖励 |
| 4.5 | 模式串联 | `init_game()`、游戏主循环 | 蛇初始 "L" 形、硬编码第一个食物、状态机流转 |

**学完本章后，你应该能回答：**
- 食物是怎么出现在随机位置的？为什么用一维数组而不是二维？
- 蛇撞到什么会死？碰撞的优先级为什么是墙最优先？
- 为什么撞自己的尾巴不一定会死？什么情况下尾巴是安全的？
- 自动增长是怎么计算的？`GROW_STEP` 改成不同值游戏体验怎么变？
- `init_game()` 里每一步在做什么？为什么第一个食物不随机放？

**下一章预告**：第五章——AI 对手。你会学到 BFS 算法如何让 AI 蛇"学会"找食物，以及三种难度等级（Easy/Normal/Hard）背后的策略差异。
