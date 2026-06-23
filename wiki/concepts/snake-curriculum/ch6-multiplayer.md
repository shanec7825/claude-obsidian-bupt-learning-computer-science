---
type: concept
title: "第六章：多玩家与战斗系统"
created: 2026-06-22
tags: [curriculum, cpp, game-dev, multiplayer, pvp, scene-management]
status: developing
related:
  - "[[Snake Game Curriculum]]"
  - "[[snake-game-call-graph]]"
sessions: 4
duration: "6-8 小时"
goal: "PvP 和 Battle 模式全部完成，理解场景管理的面向对象设计"
---

# 第六章：多玩家与战斗系统

> **目标**：学完本章后，你的 Snake 项目从单人游戏升级为三模式完整游戏——经典模式、PvP 双人对战、人机 Battle。你还会理解面向对象设计中"多态"的威力。
> **总课时**：4 课时（约 6-8 小时）
> **前置要求**：第四章（游戏逻辑）、第五章（AI 对手）

这是整个课程的最后一章，也是技术密度最高的一章。前面五章你搭好了基础设施——地图能画了、蛇能跑了、AI 会思考了。但至今为止，你的游戏每次只处理**一条蛇**。这一章我们要处理**两条蛇同时存在**的世界。

两条蛇同时在地图上移动，它们会互相碰撞，一方的死会让另一方变长。这还不够——我们还要设计一套**场景管理系统**，让玩家可以从菜单进入不同模式，玩完一局后返回菜单再开下一局。这就是面向对象中"多态"的真正用武之地。

让我们从核心问题开始：两条蛇的状态怎么存？

---

## 6.1 两条蛇的状态管理（1课时）

> **课时**：1 课时（约 1.5-2 小时）
> **目标**：理解 GameState 结构体如何在一套字段里同时管理两条蛇的状态，能画出 PvP 模式初始化的 ASCII 地图

### 6.1.1 回顾 GameState 的完整布局

在第四章，你见过的 `GameState` 大概长这样：

```cpp
struct GameState {
    char map[MAX_MAP_SIZE][MAX_MAP_SIZE];
    int map_size;
    Point snake[MAX_SNAKE_LEN];
    int length;
    int score;
    Direction direction;
    // ... 一些 bool 字段
};
```

这是经典的"单人版"视图。但实际上，从第五章开始 `GameState` 就已经悄悄变大了——它多了一套"AI 蛇"的字段。现在让我们看完整版：

```cpp
// game.h —— 完整 GameState 结构体
struct GameState {
    // ---- 地图 ----
    char map[MAX_MAP_SIZE][MAX_MAP_SIZE];
    int map_size;

    // ---- 玩家蛇（蛇1）----
    Point snake[MAX_SNAKE_LEN];    // 蛇身坐标，snake[0] 为头
    int length;                    // 当前长度
    int score;                     // 分数
    Direction direction;           // 方向
    bool turn_locked;              // 本帧是否已转向
    int food_eaten;                // 累计吃食物次数
    int pending_growth;            // 待应用生长节数

    // ---- AI 蛇 / 2P 蛇（蛇2）----
    Point ai_snake[MAX_SNAKE_LEN]; // 蛇2 的身体坐标
    int ai_length;                 // 蛇2 长度
    Direction ai_direction;        // 蛇2 方向
    bool ai_is_alive;              // 蛇2 是否存活
    int ai_score;                  // 蛇2 分数
    int ai_food_eaten;             // 蛇2 吃食物次数
    int ai_pending_growth;         // 蛇2 待应用生长
    DeathReason ai_death_reason;   // 蛇2 死亡原因
    AIDifficulty ai_difficulty;    // AI 难度（Battle 用）

    // ---- PvP 专用 ----
    Direction p2_direction;        // 2P 方向
    bool p2_turn_locked;           // 2P 转向锁定

    // ---- 游戏控制 ----
    GameMode mode;
    bool started;
    bool paused;
    bool game_over;
    int move_count;
    int grow_step;
    DeathReason death_reason;
};
```

这个结构体大约 30 个字段。别怕——大部分你已经在第四章和第五章里见过了。现在要理解的是：**蛇2 的字段为什么放在同一个 struct 里，而不是单独做一个 `Snake` 结构体**？

### 6.1.2 为什么两条蛇放在同一个 struct 里？

这个问题问得很好。很多初学者的第一反应是："蛇就是蛇，我定义一个 `Snake` 结构体，然后声明两个不就行了？"

```cpp
// 很多人的第一反应 —— 看起来更"面向对象"
struct Snake {
    Point body[MAX_SNAKE_LEN];
    int length;
    int score;
    Direction dir;
    // ...
};

Snake player1;
Snake player2;
```

这个设计在某些场景下很合理，但 Snake 这个项目选择了一条不同的路。原因有两条，一深一浅：

**浅层原因：方便函数传参。** 所有游戏逻辑函数（`move_pvp_snakes()`、`move_battle_snakes()`、`draw_game()`）都接收一个 `GameState *` 指针。一个指针就能访问整个游戏的所有数据——地图、两条蛇、控制状态。如果你拆成了三个独立对象，每个函数都要传三个指针：

```cpp
// 拆分方案 —— 每个函数签名都变长了
void move_pvp_snakes(Map *map, Snake *p1, Snake *p2, GameControl *ctrl);
//                        四个参数 vs 现在的一个参数
void move_pvp_snakes(GameState *game);  // 干净
```

**深层原因：碰撞检测需要"同时看到两条蛇"。** 这是真正的设计驱动力。在 6.2 节你会看到，两条蛇的碰撞检查不是各自独立进行的——它们必须**一起算**。蛇1 的头可能撞到蛇2 的身体、蛇2 的头可能撞到蛇1 的身体、两个头可能正好撞在一起（同归于尽）。这些判断需要同时读取两条蛇的位置，以及地图上的每一格是什么。

把两条蛇放在同一个 GameState 里，`map[][]` 数组天然就是它们的"共享现实"——蛇1 的头是 `'H'`，蛇1 的身体是 `'B'`，蛇2 的头是 `'A'`，蛇2 的身体是 `'C'`。碰撞检测只要查 `map[next.row][next.col]` 是什么字符，就知道这一格属于谁。

你可以把这个设计理解为：**GameState 不是"一条蛇的状态"，而是"这一局游戏的全部状态"。** 它是对"棋盘上正在发生的一切"的完整快照。

### 6.1.3 两条蛇的字段对照表

注意命名上的一个历史遗留：蛇2 的字段前缀是 `ai_`。这在 Battle 模式下很自然（"AI 蛇"），但在 PvP 模式下它实际存的是"2P 的蛇"。这些字段名是一个工程现实——项目从单人模式逐步演进到双人模式，字段名没有跟着重命名。

| 蛇1（玩家 / P1） | 蛇2（AI / P2） | 含义 |
|---|---|---|
| `snake[]` | `ai_snake[]` | 蛇身坐标数组 |
| `length` | `ai_length` | 当前长度（节数） |
| `direction` | `ai_direction` | 当前移动方向 |
| `score` | `ai_score` | 累计分数 |
| `food_eaten` | `ai_food_eaten` | 吃到的食物数 |
| `pending_growth` | `ai_pending_growth` | 待应用生长 |
| `turn_locked` | `p2_turn_locked` | 本帧是否已转向 |
| 无（死了 = game_over） | `ai_is_alive` | 是否存活 |
| `death_reason` | `ai_death_reason` | 死亡原因 |

注意一个不对称之处：蛇1 没有 `is_alive` 字段。为什么？因为蛇1 是玩家——玩家死了，游戏就结束了（`game_over = true`）。所以 `game_over` 本身就在说"蛇1 死了"。而蛇2 可能死了但玩家还活着（Battle 模式下 AI 会复活），所以它需要一个独立的 `ai_is_alive` 字段。

### 6.1.4 set_p2_direction() —— 为什么也有 turn_locked？

在第四章你学过 `set_direction()` 的 `turn_locked` 机制：每帧只允许转一次向，防止玩家在一帧内多次改变方向导致的 bug（比如快速按"上→左"会让蛇原地掉头）。

PvP 模式下，2P 的转向函数 `set_p2_direction()` 有独立的锁定字段 `p2_turn_locked`：

```cpp
// game.cpp —— P2 方向设置
void set_p2_direction(GameState *game, Direction dir)
{
    if (game->p2_turn_locked)      // 本帧已经转过向了，忽略
        return;

    if (!game->started)
    {
        // 还没开始：检查起始方向是否安全
        if (p2_can_start_in_direction(game, dir))
        {
            game->p2_direction = dir;
            game->ai_direction = dir;    // P2 的方向也存到 ai_direction
            game->p2_turn_locked = true;
        }
        return;
    }

    // 不能反向
    if (!is_opposite(game->ai_direction, dir))
    {
        game->ai_direction = dir;
        game->p2_direction = dir;
        game->p2_turn_locked = true;
    }
}
```

关键的细节在 `game->p2_direction = dir` 和 `game->ai_direction = dir` **同时赋值**。因为 PvP 模式下，`ai_direction` 字段实际上存的是 P2 的方向。移动函数 `move_pvp_snakes()` 读取 `game->ai_direction` 来知道 P2 要往哪走。而 `p2_direction` 只用于初始方向的存根（在 `started == false` 状态下暂存）。

为什么 `p2_turn_locked` 不和蛇1 共用同一个 `turn_locked`？因为它们是两个独立的人，每帧每人都可以转一次向。如果共用，P1 转了以后 P2 就不能转了——这不公平。

在 `move_pvp_snakes()` 的末尾，两个锁都被解开：

```cpp
game->turn_locked = false;
game->p2_turn_locked = false;
```

### 6.1.5 PvP 模式初始化全景

```cpp
// game.cpp —— init_pvp_game() 核心部分
void init_pvp_game(GameState *game)
{
    game->map_size = BATTLE_MAP_SIZE;  // 35x35
    clear_map(game);

    int mid = BATTLE_MAP_SIZE / 2;  // 17

    // P1 蛇在左边
    game->snake[0] = {mid, 7};       // 蛇头
    game->snake[1] = {mid, 6};       // 身1
    game->snake[2] = {mid + 1, 6};   // 身2（尾巴）
    game->length = 3;
    game->direction = UP;

    // P2 蛇在右边
    game->ai_snake[0] = {mid, BATTLE_MAP_SIZE - 8};      // 蛇头 = (17, 27)
    game->ai_snake[1] = {mid, BATTLE_MAP_SIZE - 7};      // 身1  = (17, 28)
    game->ai_snake[2] = {mid + 1, BATTLE_MAP_SIZE - 7};  // 身2  = (18, 28)
    game->ai_length = 3;
    game->ai_direction = UP;
    game->ai_is_alive = true;

    // 把两条蛇写入地图
    put_player_snake_on_map(game);   // 'H' + 'B'
    put_ai_snake_on_map(game);       // 'A' + 'C'

    // 30 个随机障碍物 + 2 个食物
    generate_obstacles(game, 30);
    place_food(game);
    place_food(game);
}
```

P1 在左侧（列 6-7），P2 在右侧（列 27-28），两条蛇的初始形态完全对称——都是 L 形，头朝上。这保证了公平性：两个玩家开局的视角和空间是对称的。

### 6.1.6 常见错误

**错误 1：PvP 初始化时忘记设置 ai_is_alive = true**

```cpp
// WRONG ❌ —— P2 的蛇永远不存在
init_pvp_game(game);
// 忘记写 game->ai_is_alive = true;
// 结果：move_pvp_snakes() 看到 ai_is_alive == false，P2 永远不动

// CORRECT ✅
game->ai_is_alive = true;
```

**错误 2：认为 p2_direction 和 ai_direction 是两个独立的方向**

```cpp
// WRONG ❌ —— 只改了 p2_direction，ai_direction 没跟上
game->p2_direction = RIGHT;
// move_pvp_snakes() 读的是 ai_direction，所以 P2 实际上没转！

// CORRECT ✅ —— 两个字段同步更新
game->p2_direction = RIGHT;
game->ai_direction = RIGHT;
```

### 6.1.7 练一练

**练习：画出 PvP 模式的 GameState 初始化后地图的 ASCII 图**

条件：
- `BATTLE_MAP_SIZE = 35`，坐标从 0 到 34
- `mid = 35 / 2 = 17`（整数除法）
- P1 蛇头 `(17, 7)`，身1 `(17, 6)`，身2 `(18, 6)`，方向 UP
- P2 蛇头 `(17, 27)`，身1 `(17, 28)`，身2 `(18, 28)`，方向 UP
- 边界 `#`，内部 `.`，障碍物暂时忽略（用 `.` 代替），食物位置随机忽略

画一个简化的 35x35 地图，只画出有东西的几行（15-19 行，4-28 列），蛇头朝上，标注坐标。

**预期输出参考**：你的图应该包含 `H` `B` `B`（P1 绿色蛇）在左侧、`A` `C` `C`（P2 蓝色蛇）在右侧，两条蛇镜像对称。

### 参考答案

<details>
<summary>点击展开 ASCII 地图</summary>

```
     列 4  5  6  7  8    ...    25 26 27 28 29
行 15  #  #  #  #  #            #  #  #  #  #
行 16  #  .  .  .  .            .  .  .  .  #
行 17  #  .  B  H  .    ...     .  A  C  .  #    ← 中间行
行 18  #  .  B  .  .            .  .  C  .  #    ← 尾巴行
行 19  #  .  .  .  .            .  .  .  .  #
       #  #  #  #  #            #  #  #  #  #

简化视图（只保留关键区域，15 ≤ row ≤ 19，4 ≤ col ≤ 29）：

       col: 4  5  6  7  8  9  ... 25 26 27 28 29
row 15      #  #  #  #  #  #       #  #  #  #  #
row 16      #  .  .  .  .  .  ...  .  .  .  .  #     ← 空地
row 17      #  .  B  H  .  .  ...  .  A  C  .  #     ← 蛇头行（mid=17）
row 18      #  .  B  .  .  .  ...  .  .  C  .  #     ← 尾巴在各自身体下方
row 19      #  .  .  .  .  .  ...  .  .  .  .  #
row 20      #  #  #  #  #  #       #  #  #  #  #

P1 蛇（绿色）：H=(17,7), B=(17,6), B=(18,6)  → "L"形，头朝上
P2 蛇（蓝色）：A=(17,27), C=(17,28), C=(18,28) → 镜像"L"形，头朝上

两条蛇镜像对称，相距 20 列。
```

</details>

---

## 6.2 多蛇碰撞解决（2课时）——本章核心

> **课时**：2 课时（约 3-4 小时）
> **目标**：完全理解 `move_pvp_snakes()` 的碰撞检测顺序和执行逻辑，能纸上模拟任意碰撞场景

这是整个 Snake 课程里最难的一节。不是因为代码多——`move_pvp_snakes()` 只有约 200 行——而是因为它把好几层逻辑叠在了一起：同时计算两个头的新位置、按优先级检查 6 种碰撞、根据结果决定谁死谁活、谁执行移动、谁获得击杀奖励。

我建议你把这节分成两段来学：上半节搞懂碰撞检测的顺序，下半节搞懂执行结果的逻辑。中间可以休息一下，或者先纸上画一画再接着看。

### 6.2.1 为什么不能"各自判断"？

在经典模式里，单人蛇的碰撞检测很简单：算出下一步 `next`，检查那一格是不是墙/障碍物/自己身体。逻辑链条是**线性的**。

但两条蛇的世界不一样。考虑这个场景：

```
P1 蛇头向右走 → H 要走到 (17, 28)
P2 蛇头向左走 ← A 要走到 (17, 7)

如果各自独立判断：
  P1 检查 (17, 28) → 那里是 P2 的身体 'C' → P1 认为"我撞到敌人了" → P1 死
  P2 检查 (17, 7)  → 那里是 P1 的身体 'B' → P2 认为"我撞到敌人了" → P2 死
结果：双方都死。这合理吗？
```

不合理！P1 的头要去的位置正好在 P2 的尾巴旁边（这个例子不太精确，但你理解意思就行）。两条蛇的移动必须**同时计算、一起判断**。你不能让 P1 先判断完再让 P2 判断——因为 P1 判断时 P2 还没动，地图状态是旧的。你必须同时算出两个头的新位置，然后在一个统一的碰撞解析器里决定生死。

这就是 `move_pvp_snakes()` 要解决的核心问题。

### 6.2.2 move_pvp_snakes() 完整讲解（上）—— 碰撞检测的精确顺序

下面我们对照实际源码，一步一步走完碰撞检测的 7 个步骤。先看上半部分——从"算出两个新头"到"所有 bool 标记就绪"：

```cpp
// game.cpp —— move_pvp_snakes() 上半节：碰撞检测
void move_pvp_snakes(GameState *game)
{
    if (!game->started || game->paused || game->game_over)
        return;

    // === 第一步：同时计算两个头的新位置 ===
    Direction p1_dir = game->direction;
    Direction p2_dir = game->ai_direction;

    Point p1_next = next_head(game->snake[0], p1_dir);
    Point p2_next = next_head(game->ai_snake[0], p2_dir);

    // === 第二步：查地图，看每个新位置是什么 ===
    char p1_cell = game->map[p1_next.row][p1_next.col];
    char p2_cell = game->map[p2_next.row][p2_next.col];

    // === 第三步：判断是否吃食物 / 自动增长 ===
    bool p1_will_eat = (p1_cell == 'F');
    bool p2_will_eat = (p2_cell == 'F');
    bool auto_grow = ((game->move_count + 1) % game->grow_step) == 0;
    bool p1_will_grow = p1_will_eat || auto_grow;
    bool p2_will_grow = p2_will_eat || auto_grow;

    // 死亡标记
    bool p1_dies = false, p2_dies = false;
    DeathReason p1_reason = DEATH_NONE, p2_reason = DEATH_NONE;
    bool p1_grows_from_kill = false, p2_grows_from_kill = false;
    int p1_grow_amount = 0, p2_grow_amount = 0;

    // === 第四步：碰撞检测（顺序极其重要！） ===
```

**第一步**值得细说。`next_head()` 是一个纯函数——它只负责"给定当前位置和方向，算出新位置"。它不做任何碰撞检查。这是"关注点分离"的好例子：位置计算和安全性判断是两个独立的职责。

**第二步**直接查 `map[][]`——这是把两条蛇放同一个 struct 的好处。地图是共享的现实，你不需要去遍历 P2 的身体数组来知道 `(17,28)` 是不是 P2 的身体——直接读 `map[17][28]`，如果是 `'C'`，那就是 P2 的身体。

**第三步**判断增长。`will_grow` 决定尾巴是否移开——这直接影响"尾部安全"的判断。我们在第四章学过：如果蛇在增长，尾巴不动，所以那个位置不安全。如果蛇没在增长，尾巴会移走，所以尾巴可以过去。

---

现在到了最关键的部分——**碰撞检测的精确顺序**：

```cpp
    // 1. 头碰头（is_same_point）→ 同归于尽
    if (is_same_point(p1_next, p2_next))
    {
        p1_dies = true;
        p1_reason = DEATH_HEAD_ON;
        p2_dies = true;
        p2_reason = DEATH_HEAD_ON;
    }
    else
    {
        // 2. P1 头碰 P2 身体
        if (p1_cell == 'C')
        {
            if (!p2_will_grow && is_same_point(p1_next,
                               game->ai_snake[game->ai_length - 1]))
            {
                // 安全 —— P2 的尾巴正要移开
            }
            else
            {
                p1_dies = true;
                p1_reason = DEATH_ENEMY_BODY;
                p2_grows_from_kill = true;
                p2_grow_amount = game->length;
            }
        }

        // 3. P2 头碰 P1 身体
        if (p2_cell == 'B')
        {
            if (!p1_will_grow && is_same_point(p2_next,
                               game->snake[game->length - 1]))
            {
                // 安全 —— P1 的尾巴正要移开
            }
            else
            {
                p2_dies = true;
                p2_reason = DEATH_ENEMY_BODY;
                p1_grows_from_kill = true;
                p1_grow_amount = game->ai_length;
            }
        }

        // 4. P1 撞墙/障碍物
        if (p1_cell == '#' || p1_cell == 'O')
        {
            p1_dies = true;
            p1_reason = (p1_cell == '#') ? DEATH_WALL : DEATH_OBSTACLE;
        }

        // 5. P2 撞墙/障碍物
        if (p2_cell == '#' || p2_cell == 'O')
        {
            p2_dies = true;
            p2_reason = (p2_cell == '#') ? DEATH_WALL : DEATH_OBSTACLE;
        }

        // 6. P1 自撞
        if (!p1_dies && p1_cell == 'B')
        {
            if (!p1_will_grow && is_same_point(p1_next,
                               game->snake[game->length - 1]))
            {
                // 安全 —— 自己的尾巴
            }
            else
            {
                p1_dies = true;
                p1_reason = DEATH_BODY;
            }
        }

        // 7. P2 自撞
        if (!p2_dies && p2_cell == 'C')
        {
            if (!p2_will_grow && is_same_point(p2_next,
                               game->ai_snake[game->ai_length - 1]))
            {
                // 安全 —— 自己的尾巴
            }
            else
            {
                p2_dies = true;
                p2_reason = DEATH_BODY;
            }
        }
    }
```

### 6.2.3 为什么这个顺序不能乱？

这个顺序是经过精心设计的。让我们逆着推理——如果顺序错了会发生什么：

**为什么步骤 2/3（攻击对方身体）在步骤 6/7（自撞）之前？**

考虑这个场景：P1 的蛇头冲向 P2 的身体，而那个位置恰好也在 P1 自己身体的路径上。应该判什么？判"攻击成功"还是"自杀"？

正确行为是：**先判攻击**。如果 P1 撞到了 P2 的身体，那就不是"自撞"——那是 P1 的攻击行为，只是攻击失败了（撞到对方身体上死了）。按照顺序 2-3-6-7，P1 会先被步骤 2 捕获（`p1_cell == 'C'`），标记 `p1_dies = true`。到了步骤 6 时，前面的 `!p1_dies` 条件阻止了重复判断。

如果反过来（先判自撞再判攻击），就会出现逻辑混乱——P1 自撞的 `'B'` 可能和攻击目标的 `'C'` 是同一格的不同解释。

**为什么步骤 1（头碰头）要放在最前面？**

头碰头是一种"势均力敌"的碰撞——两个头同时到达同一个格子。它不等同于"P1 撞到 P2 身体"或"P2 撞到 P1 身体"。为什么？因为如果头碰头被当作"P1 撞到 P2 身体"处理，P1 会死而 P2 活——这不公平，P2 也朝着同一格冲过去了。正确行为是**同归于尽**。

`is_same_point()` 判断在 `else` 分支之外——如果两个头目标点相同，直接跳过步骤 2-7，双方都标记死亡。

**为什么步骤 4/5（撞墙/障碍物）在 6/7 之前？**

撞墙是一个立即终结条件，不需要再检查是不是自撞——你都死了，自不自撞没意义。先判墙可以提前结束判断链。

### 6.2.4 move_pvp_snakes() 完整讲解（下）—— 执行结果

碰撞检测完了，现在我们有了一组 bool 标记：`p1_dies`、`p2_dies`，以及它们各自是否获得击杀奖励。

下面是执行部分：

```cpp
    // ===== 第八步：执行结果 =====

    // 情况 A：P2 死了
    if (p2_dies)
    {
        // 清除 P2 从地图上
        clear_snake_from_map(game, game->ai_snake, game->ai_length, 'A', 'C');
        game->ai_is_alive = false;
        game->ai_death_reason = p2_reason;
        game->game_over = true;        // PvP 中任何一方死 = 游戏结束

        // P1 获得击杀奖励
        if (p1_grows_from_kill && p1_grow_amount > 0)
            add_pending_growth(&game->pending_growth, p1_grow_amount);

        // P1 还可以走最后一帧
        if (!p1_dies)
        {
            apply_snake_move(game, game->snake, &game->length,
                             p1_dir, &game->score, &game->food_eaten,
                             &game->pending_growth, 'H', 'B');
        }

        game->move_count++;
        game->turn_locked = false;
        game->p2_turn_locked = false;
        return;
    }

    // 情况 B：P1 死了
    if (p1_dies)
    {
        game->death_reason = p1_reason;
        game->game_over = true;

        // P2 获得击杀奖励
        if (p2_grows_from_kill && p2_grow_amount > 0)
            add_pending_growth(&game->ai_pending_growth, p2_grow_amount);

        // P2 还可以走最后一帧
        if (!p2_dies && game->ai_is_alive)
        {
            apply_snake_move(game, game->ai_snake, &game->ai_length,
                             p2_dir, &game->ai_score, &game->ai_food_eaten,
                             &game->ai_pending_growth, 'A', 'C');
        }

        game->move_count++;
        game->turn_locked = false;
        game->p2_turn_locked = false;
        return;
    }

    // 情况 C：都没死 —— 正常执行两方的移动
    // P1 移动
    {
        bool ate = apply_snake_move(game, game->snake, &game->length,
                                     p1_dir, &game->score, &game->food_eaten,
                                     &game->pending_growth, 'H', 'B');
        if (ate) place_food(game);
    }

    // P2 移动
    {
        bool ate = apply_snake_move(game, game->ai_snake, &game->ai_length,
                                     p2_dir, &game->ai_score, &game->ai_food_eaten,
                                     &game->ai_pending_growth, 'A', 'C');
        if (ate) place_food(game);
    }

    // 确保 2 个食物始终存在
    {
        int food_count = 0;
        for (int r = 0; r < game->map_size; r++)
            for (int c = 0; c < game->map_size; c++)
                if (game->map[r][c] == 'F')
                    food_count++;
        while (food_count < 2 && !game->game_over)
            place_food(game);
    }

    game->move_count++;
    game->turn_locked = false;
    game->p2_turn_locked = false;
}
```

有几个关键细节值得单独讲：

**"最后一帧移动"机制**：当 P2 死了而 P1 活着时，P1 还要再执行一次 `apply_snake_move()`。为什么？因为 P1 的蛇头已经算出了 `p1_next`，在碰撞检测里只是"检查"了那个位置，但蛇的身体还没有真正前移。如果不执行这一帧，P1 的蛇会"卡"在上一个位置——在视觉上，头已经"进入"了 P2 的身体那一格，但实际上蛇身没动。这看起来像 bug。给赢家再走一帧，画面才连贯。

**`apply_snake_move()` 的抽象价值**：注意到 P1 移动和 P2 移动调用的是**同一个函数**吗？

```cpp
// game.cpp —— apply_snake_move 签名
static bool apply_snake_move(GameState *game, Point *body, int *length,
                              Direction dir, int *score, int *food_eaten,
                              int *pending_growth,
                              char head_char, char body_char);
```

这个函数的参数列表暴露了它"通用"的设计意图：
- `body` 指针：不关心是 `game->snake` 还是 `game->ai_snake`
- `length` 指针：不关心是 `&game->length` 还是 `&game->ai_length`
- `head_char` / `body_char`：不关心是 `'H'/'B'` 还是 `'A'/'C'`

这就是**参数化**的威力——同样的逻辑（前移身体、处理尾部、写入 map），通过不同参数用于两条不同的蛇。如果你把蛇的移动逻辑写了两遍（一份给 P1、一份给 P2），代码量翻倍不说，改了 P1 的逻辑忘了改 P2 的就会出 bug。

### 6.2.5 pending_growth —— 击杀奖励的逐步增长

`add_pending_growth()` 做的事情很简单：

```cpp
// game.cpp
static void add_pending_growth(int *pending, int amount)
{
    *pending += amount;
}
```

它就是在 `pending_growth` 上累加一个数。真正消费它的地方在 `apply_snake_move()` 内部：

```cpp
// apply_snake_move() 内部
bool has_pending = (*pending_growth > 0);
bool effective_grow = will_eat || auto_grow || has_pending;

// 尾部逻辑
if (!effective_grow) {
    Point tail = body[*length - 1];
    game->map[tail.row][tail.col] = '.';  // 移除尾巴
} else if (*length < MAX_SNAKE_LEN) {
    (*length)++;  // 保留尾巴 = 长了
}

// 消耗 pending_growth
if (has_pending && !will_eat && !auto_grow)
    (*pending_growth)--;
```

关键：如果这一帧同时吃到食物（`will_eat == true`）或触发了自动增长（`auto_grow == true`），`pending_growth` **不消耗**。只有当这一帧本来不长、但因为 `pending_growth > 0` 才被迫增长时，才消耗一点。这是一个巧妙的保护机制——你不会因为击杀奖励而"浪费"一次自动增长机会。

举个例子：你击杀了 AI 蛇（身长 120 节），获得 `pending_growth = 120`。接下来 120 步里，每走一步你的蛇长一节（除非那一帧你同时吃了食物或触发了 auto_grow）。效果就是你的蛇在接下来的几分钟里暴长，把地图占得满满的。这就是"吃鸡"快感在 Snake 里的实现。

### 6.2.6 move_battle_snakes() vs move_pvp_snakes() —— 三处关键区别

两个函数的碰撞检测部分**几乎一模一样**（换了一下变量名），但执行结果部分有三处重要不同：

| 方面 | move_battle_snakes() | move_pvp_snakes() |
|---|---|---|
| AI 死了 | `respawn_ai()` 复活 AI | 不复活，游戏结束 |
| 玩家死了 | `game_over = true`，游戏结束 | `game_over = true`，游戏结束 |
| P2 死了 | `respawn_ai()` 复活 AI | `game_over = true`，P1 胜 |

核心区别只有一处：Battle 模式中 AI 死了会复活（`respawn_ai()`），而 PvP 中任何一方死 = 游戏结束。

`respawn_ai()` 做这些事情：
1. 清除 AI 旧身体（`clear_snake_from_map`）
2. 找一个有 3 格连续空地的位置
3. 放置新的 3 节 AI 蛇（头朝上）
4. 重置 AI 的分数和状态
5. 确保地图上有至少 2 个食物

这就是为什么 Battle 模式的结束条件只有"玩家死"——AI 死多少回都会重生，直到你失误为止。

### 6.2.7 常见错误

**错误 1：碰撞顺序颠倒**

```cpp
// WRONG ❌ —— 先判自撞再判攻击
if (p1_cell == 'B') { p1_dies = true; }      // 步骤A：自撞
if (p1_cell == 'C') { p1_dies = true; }      // 步骤B：攻击
// 如果 p1_cell == 'C' 但地图那一格既是 'C' 又有其他含义...
// 等等，map 一格只能有一个字符，所以这里不会同时触发。
// 但问题是逻辑上的优先级：攻击失败应该优先于自杀判断。

// CORRECT ✅ —— 严格按优先级
// 1. 头碰头
// 2. P1头碰P2身（攻击）
// 3. P2头碰P1身（攻击）
// 4. 撞墙
// 5. 撞障碍物
// 6. P1自撞
// 7. P2自撞
```

**错误 2：忘记确保 2 个食物**

```cpp
// WRONG ❌ —— 35x35 的地图只有 1 个食物，两条蛇抢 1 个食物太难了
apply_snake_move(...);
apply_snake_move(...);
// 没有补食物！

// CORRECT ✅
apply_snake_move(...);
apply_snake_move(...);
// 确保 2 个食物
int food_count = ...;  // 数地图上的 'F'
while (food_count < 2) { place_food(game); food_count++; }
```

**错误 3：用 P1 的 will_grow 判断 P2 尾巴安全**

```cpp
// WRONG ❌ —— 用错了变量！
if (!p1_will_grow && is_same_point(p2_next, game->ai_snake[...]))
//     ^^^^^^^^^^^^                              ^^^^^^^^^^^^^^^^
//     这是 P1 的增长状态                        这是 P2 的尾巴
//     完全不搭！

// CORRECT ✅ —— 判断 P2 的尾巴安不安全，应该看 P2 会不会增长
if (!p2_will_grow && is_same_point(p2_next,
                   game->ai_snake[game->ai_length - 1]))
```

### 6.2.8 练一练

**练习 1：纸上模拟"头碰头"碰撞**

假设 35x35 PvP 地图上有两条蛇：

- P1 蛇头 `(17, 15)`，方向 `RIGHT`，身体从 `(17,15)` 向左延伸，长度 3
- P2 蛇头 `(17, 18)`，方向 `LEFT`，身体从 `(17,18)` 向右延伸，长度 3

在当前帧，双方都不吃食物（下一格都没有 `'F'`），双方都不触发自动增长（move_count = 1，`(1+1)%8 != 0`）。

写出碰撞检测每一步的判断结果。

**练习 2：纸上模拟"P1 撞到 P2 身体"**

- P1 蛇头 `(17, 15)`，方向 `DOWN`，长度 5
- P2 蛇头 `(16, 17)`，方向 `UP`，身体在 `(17,17)` `(18,17)`，长度 3

P1 的下一步是 `(18, 15)`（空地 `.`），安全。
P2 的下一步是 `(15, 17)`（空地 `.`），安全。  
但等等——P1 的身体在 `(16,15)` `(17,15)` `(17,16)` `(17,17)`。P2 的头要去 `(15,17)`，没问题。
好吧，换一个更有意义的场景：P2 的下一步是 `(17, 16)`，那里是 P1 的身体 `'B'`。写出碰撞检测结果。

### 参考答案

<details>
<summary>点击展开练习 1：头碰头</summary>

```
初始状态：
  P1: head=(17,15), dir=RIGHT → p1_next=(17,16)
  P2: head=(17,18), dir=LEFT  → p2_next=(17,17)
  （双方头部相差 3 格，各自走一步后相差 1 格）

不对，让我重新设置场景让它们头碰头：
  P1: head=(17,15), dir=RIGHT → p1_next=(17,16)
  P2: head=(17,17), dir=LEFT  → p2_next=(17,16)
  
现在 p1_next == p2_next == (17,16)！

碰撞检测过程：

步骤1: is_same_point(p1_next, p2_next) → is_same_point((17,16), (17,16)) → TRUE
       → p1_dies = true, p1_reason = DEATH_HEAD_ON
       → p2_dies = true, p2_reason = DEATH_HEAD_ON
       → 跳过 else 分支（步骤 2-7 不执行）

执行结果：
  情况 A: p2_dies == true
    → clear_snake_from_map(P2 身体)
    → game->ai_is_alive = false
    → game_over = true
    → p1_award: add_pending_growth(P1, 3)  ← P2 身长为 3
    → apply_snake_move(P1) ← P1 走最后一帧
    → return

  但等等！p1_dies 也是 true。看代码：
  if (p2_dies) { ... if (!p1_dies) apply_snake_move(P1); }
  p1_dies == true，所以 P1 的最后一帧移动被跳过了。

  然后到情况 B: if (p1_dies) { ... }
  但情况 A 已经 return 了！所以情况 B 不会执行。

最终结果：双方都死，游戏结束。P1 的 move 没执行（头碰头，没人能走最后一帧）。
p1_reason = DEATH_HEAD_ON，p2_reason = DEATH_HEAD_ON。
```

</details>

<details>
<summary>点击展开练习 2：P1 撞 P2 身体</summary>

```
修改场景让它更有意义：

P1: head=(17,15), dir=RIGHT  → p1_next=(17,16)
P2: head=(16,17), dir=DOWN   → p2_next=(17,17)  ← 但 (17,16) 是什么？

假设当前地图：
  行17: ... B B H . C C ...    (P1 身体 + 头在 15, P2 身体在 17-18)
  
实际上让我重新设置：
P1: head=(17,15), dir=RIGHT  → p1_next=(17,16)
P2: body: head=(17,17), body at (17,18), tail at (18,18), dir=RIGHT → p2_next=(17,19)

map[17][16] = '.'（空地），所以 p1 安全
map[17][19] = '.'（空地），所以 p2 安全
→ 没什么碰撞

让我设置一个更有趣的：
P1: head=(17,15), dir=RIGHT  → p1_next=(17,16)
P2: body: head=(16,17), body at (17,17), tail at (18,17), dir=UP → p2_next=(15,17)

假设 map[17][16] = 'C'（P2 的身体在那一格）
map[15][17] = '.'（空地）

碰撞检测：
步骤1: is_same_point((17,16), (15,17)) → FALSE → 进入 else
步骤2: p1_cell = 'C' → P1 头碰 P2 身体
       !p2_will_grow? P2 方向 UP, 尾巴在 (18,17), 下一步 (15,17)
       is_same_point((17,16), (18,17))? NO
       → p1_dies = true, p1_reason = DEATH_ENEMY_BODY
       → p2_grows_from_kill = true, p2_grow_amount = P1 身长
步骤3: p2_cell = '.' → 不触发
步骤4: p1_cell = 'C' → 不是墙 → 不触发
步骤5: p2_cell = '.' → 不是墙 → 不触发
步骤6: !p1_dies? p1_dies == true → 跳过
步骤7: !p2_dies? p2_dies == false, p2_cell = '.' → 不触发

执行结果：
情况 B: p1_dies == true
  → game_over = true
  → P2 获得击杀奖励：add_pending_growth(P2, P1 身长)
  → P2 走最后一帧：apply_snake_move(P2)
  → return

最终：P1 死（撞到 P2 身体），P2 活，P2 获得击杀奖励。
```

</details>

---

## 6.3 场景管理：面向对象的威力（1课时）

> **课时**：1 课时（约 1.5-2 小时）
> **目标**：理解 Scene 抽象基类的设计、多态的实现原理、场景切换的完整流程

在第一章你学过 `struct` 和指针。那时候你可能在想："这有什么用？"现在，它们有了真正的用武之地——构建可扩展的场景管理系统。

### 6.3.1 问题的提出：怎么管理多个界面？

思考一下 Snake 游戏有哪些"界面"：

1. **主菜单**：显示"1-经典模式 2-PvP 3-Battle"，等玩家按键
2. **难度选择**：Battle 模式下选择 Easy/Normal/Hard
3. **游戏进行中**：蛇在地图上跑，右侧面板显示分数
4. **Game Over 画面**：显示分数，等玩家按 R 重开或 ESC 退出

这些界面每个都有自己的逻辑：
- 菜单关心按键 `1` `2` `3`
- 游戏中关心 WASD + 方向键 + 定时移动
- 每个界面的渲染也不同

如果不用任何抽象，`main()` 会长这样：

```cpp
// 没有抽象的 main() —— 想象一下有多乱
int main()
{
    int state = 0;  // 0=菜单, 1=游戏, 2=难度选择
    GameState game;

    while (true) {
        if (state == 0) {
            // 画菜单
            // 处理菜单按键
            // 如果选了模式 → state = 1
        } else if (state == 1) {
            // 画游戏
            // 处理游戏按键
            // 如果按 ESC → state = 0
        } else if (state == 2) {
            // 画难度选择
            // ...
        }
    }
}
```

问题显而易见：`main()` 变成了一个巨大的 `if-else` 链，每种模式的逻辑混在一起。加一个新界面就得改 `main()`。这是典型的"高耦合"代码——改动一处，冒风险多处。

### 6.3.2 Scene 抽象基类设计

面向对象给了一个优雅的解决方案：**定义一个"场景"的抽象接口，每个具体场景各自实现**。

```cpp
// scene.h —— 场景抽象基类
class Scene {
public:
    // 纯虚函数：每个子类必须实现
    virtual void update() = 0;         // 处理输入和逻辑
    virtual void render() = 0;         // 绘制画面
    virtual SceneType next_scene() const = 0;  // 返回目标场景

    // 虚析构函数：确保通过基类指针 delete 子类时正确释放
    virtual ~Scene() {}
};
```

三个纯虚函数（`= 0`）定义了"什么是一个场景"：

- **`update()`**：每帧调一次，处理按键输入、更新内部状态。比如 MenuScene 的 update 检查 `key_pressed('1')`，PlayScene 的 update 检查 WASD 和定时器。
- **`render()`**：每帧调一次，画当前场景的内容。比如 MenuScene 的 render 调用 `draw_menu()`，PlayScene 的 render 调用 `draw_game()`。
- **`next_scene()`**：返回一个 `SceneType` 枚举值，告诉 main 循环"我下一步想去哪个场景"。比如 MenuScene 在玩家选了模式后返回 `SCENE_PLAY`，PlayScene 在玩家按 ESC 后返回 `SCENE_MENU`。

任何实现了这三个函数的类，都可以被 main 循环以**完全相同的方式**驱动——main 不需要知道当前是菜单还是游戏，它只需要调 `update()` → `render()` → `next_scene()`。

### 6.3.3 多态实战

```cpp
// 创建一个菜单场景
Scene *current = new MenuScene();

// current 的类型是 Scene*（基类指针）
// 编译器不知道它指向的是 MenuScene 还是 PlayScene
// 但 C++ 的虚函数表（vtable）会在运行时自动找到正确的函数！

current->update();   // 实际调用 MenuScene::update()
current->render();   // 实际调用 MenuScene::render()
```

这就是**多态**（polymorphism）的核心：**基类指针调用虚函数时，实际执行的是子类的版本**。决定权不在编译期，而在运行期——C++ 通过每个对象的"虚函数表指针"（vptr）在运行时查到正确的函数地址。

换句话说：

```
你写了一行 current->update();

编译器不知道 current 指向什么——它可能是 MenuScene，可能是 PlayScene，
可能是将来你写的任何一个新场景。

但程序跑起来后，C++ 自己去查这个对象内部的"函数表"，找到正确的 update()，
执行它。你不需要写 if (type == MENU) ... else if (type == PLAY) ...。
```

这就是面向对象最根本的承诺：**对扩展开放（加新场景不改 main），对修改封闭（main 的代码不用变）。**

### 6.3.4 场景切换的完整流程

现在看 `main.cpp` 的 while 循环：

```cpp
// main.cpp —— 场景主循环
Scene *current = new MenuScene();          // 从菜单开始

while (true)
{
    current->update();                     // 处理当前场景的输入
    current->render();                     // 画当前场景
    FlushBatchDraw();                      // 刷新屏幕
    Sleep(16);                             // ≈60 FPS

    SceneType next = current->next_scene(); // 查看是否需要切换

    if (next == SCENE_EXIT)
        break;                             // 退出程序

    if (next != SCENE_MENU
        && dynamic_cast<MenuScene *>(current) != nullptr)
    {
        // 从菜单进入游戏
        MenuScene *menu = static_cast<MenuScene *>(current);
        GameMode mode = menu->selected_mode;  // 读取用户选的模式
        delete current;                       // 销毁旧场景

        // 根据模式调整窗口大小
        if (mode == MODE_CLASSIC)
            resize_graph_window(CLASSIC_WINDOW_WIDTH, CLASSIC_WINDOW_HEIGHT);
        else
            resize_graph_window(BATTLE_WINDOW_WIDTH, BATTLE_WINDOW_HEIGHT);

        // 构造新场景
        current = new PlayScene(mode, menu->selected_difficulty());
    }
    else if (next == SCENE_MENU
             && dynamic_cast<PlayScene *>(current) != nullptr)
    {
        // 从游戏返回菜单
        delete current;
        resize_graph_window(MENU_WINDOW_WIDTH, MENU_WINDOW_HEIGHT);
        current = new MenuScene();
    }
}
```

这个循环的关键步骤是：

1. **`dynamic_cast` 检查**：`dynamic_cast<MenuScene *>(current)` 在运行时检查 `current` 是否真的是 `MenuScene`（或其子类）。如果是，返回有效指针；如果不是，返回 `nullptr`。这比手动维护一个 `type` 枚举字段更安全——类型信息由 C++ 运行时系统管理，不会出现手动同步不一致的 bug。

2. **`delete` 旧场景**：每次切换场景都要先销毁旧的。`delete` 会调用 `~Scene()`（虚析构函数确保调用正确的子类析构函数），释放 PlayScene 内部的 `GameState` 占用的栈内存。

3. **`new` 新场景**：`new PlayScene(mode, difficulty)` 不仅分配内存，还会在构造函数里调用 `init_game()` / `init_pvp_game()` / `init_battle_game()`——一场新游戏从构造那一刻就准备好了。

4. **`resize_graph_window()`**：经典模式用 640x600 窗口，对战模式用 1000x1000 窗口。EasyX 不支持运行时改窗口大小，所以必须先关再开。

### 6.3.5 resize_graph_window 为什么需要关闭重开？

```cpp
// game.cpp
void resize_graph_window(int width, int height)
{
    EndBatchDraw();       // 停止批量绘制
    closegraph();         // 关闭当前图形窗口
    initgraph(width, height);  // 以新尺寸重新打开
    disable_ime();        // 重新禁用输入法
    reset_key_state();    // 重置按键状态（防止窗口切换时的"幽灵按键"）
    BeginBatchDraw();     // 重新开始批量绘制
}
```

EasyX 的设计里，窗口大小在 `initgraph()` 时确定，之后不能动态改变。所以改大小 = 关掉旧的 + 开一个新的。`reset_key_state()` 是必需的——Windows 在窗口创建/销毁时可能产生虚假的按键事件，不重置会导致"新窗口一打开蛇就自己转了方向"这样的灵异 bug。

### 6.3.6 常见错误

**错误 1：忘记虚析构函数**

```cpp
// WRONG ❌ —— 没有 virtual 析构函数
class Scene {
public:
    virtual void update() = 0;
    // 析构函数不是 virtual！
    ~Scene() {}  // 非虚析构函数
};

// 后果：
Scene *p = new PlayScene(...);
delete p;  // 只调用了 ~Scene()，没有调用 ~PlayScene()
           // PlayScene 内部的 GameState 可能没被正确清理
           // 内存泄漏！

// CORRECT ✅
class Scene {
public:
    virtual void update() = 0;
    virtual ~Scene() {}  // 虚析构函数！
};
```

**错误 2：把游戏逻辑放在 main() 里**

```cpp
// WRONG ❌ —— main() 知道太多
if (mode == MODE_PVP) {
    move_pvp_snakes(&game);     // main 直接调用游戏逻辑函数
} else if (mode == MODE_BATTLE) {
    move_battle_snakes(&game);
}

// CORRECT ✅ —— main 只做场景调度，游戏逻辑在 PlayScene::update() 里
current->update();  // PlayScene 内部自己判断 mode，调用正确的 move 函数
```

main 应该是一个**场景调度器**，不是游戏逻辑的容器。它的职责是"当前在哪个场景？要不要切换？"，而不是"这个模式应该调哪个 move 函数？"

### 6.3.7 练一练

**练习：实现一个简单的"两个场景切换"程序（控制台版，不用 EasyX）**

要求：
1. 定义一个 `Scene` 抽象基类，有 `update()` 和 `render()` 纯虚函数
2. 实现 `MenuScene`：打印 "=== MENU ===" 和 "1: Start Game, ESC: Quit"
3. 实现 `GameScene`：打印 "=== GAME ===" 和一个模拟分数，按 ESC 返回菜单
4. 在 `main()` 中用基类指针管理场景切换
5. 用 `_getch()` 读按键（包含 `<conio.h>`）

**预期行为**：
```
=== MENU ===
1: Start Game
ESC: Quit

按 1 → 切换到 GameScene:
=== GAME ===
Score: 0
ESC: Return to Menu

按 ESC → 返回 MenuScene
```

### 参考答案

<details>
<summary>点击展开参考实现</summary>

```cpp
#include <iostream>
#include <conio.h>

// 抽象基类
class Scene {
public:
    virtual void update(char input) = 0;
    virtual void render() = 0;
    virtual bool wants_exit() const = 0;
    virtual ~Scene() {}
};

// 菜单场景
class MenuScene : public Scene {
    bool _exit = false;
public:
    void update(char input) override {
        // MenuScene 本身不处理切换——切换由 main 根据返回值决定
    }
    void render() override {
        std::cout << "\n=== MENU ===" << std::endl;
        std::cout << "1: Start Game" << std::endl;
        std::cout << "ESC: Quit" << std::endl;
    }
    bool wants_exit() const override { return _exit; }
};

// 游戏场景
class GameScene : public Scene {
    int _score = 0;
    bool _exit = false;
public:
    void update(char input) override {
        _score++;
    }
    void render() override {
        std::cout << "\n=== GAME ===" << std::endl;
        std::cout << "Score: " << _score << std::endl;
        std::cout << "ESC: Return to Menu" << std::endl;
    }
    bool wants_exit() const override { return _exit; }
};

int main()
{
    Scene *current = new MenuScene();
    bool running = true;

    while (running)
    {
        current->render();

        if (_kbhit())
        {
            char ch = _getch();
            current->update(ch);

            // 检查是否需要切换场景
            MenuScene *menu = dynamic_cast<MenuScene *>(current);
            GameScene *game = dynamic_cast<GameScene *>(current);

            if (menu && ch == '1')
            {
                delete current;
                current = new GameScene();
            }
            else if (menu && ch == 27)  // ESC
            {
                running = false;
            }
            else if (game && ch == 27)  // ESC in game
            {
                delete current;
                current = new MenuScene();
            }
        }
    }

    delete current;
    return 0;
}
```

> **说明**：这个练习简化了场景切换——用 `dynamic_cast` 在 main 里判断当前场景类型，而不是通过 `next_scene()` 返回值。但核心思想相同：基类指针 + 多态 + 运行时类型检查。真正的 Snake 项目用 `next_scene()` 把切换决策封装在场景内部，代码更干净。
</details>

---

## 6.4 三种模式全景串联（1课时）

> **课时**：1 课时（约 2 小时）
> **目标**：完整理解三种游戏模式的场景切换路径，能从 main() 入口追踪到 Game Over 的全流程

### 6.4.1 场景状态机

从 `main()` 进来，所有场景切换的路径可以用一张 ASCII 状态机图概括：

```
              ┌──────────┐
 程序启动 ──→ │ MenuScene │ ←──────────────────────────────┐
              └────┬─────┘                                  │
                   │                                        │
         ┌─────────┼──────────┐                             │
         │ 按键 1  │ 按键 2   │ 按键 3                       │
         ▼         ▼          ▼                             │
   MODE_CLASSIC  MODE_PVP  MODE_BATTLE                      │
         │         │          │                             │
         │    ┌────┘          └──→ 选难度 (1/2/3)           │
         │    │                     │                       │
         ▼    ▼                     ▼                       │
   ┌──────────────────────────────────┐                     │
   │          PlayScene               │                     │
   │  ┌─────────┐ ┌──────┐ ┌───────┐ │                     │
   │  │ Classic  │ │ PvP  │ │Battle │ │                     │
   │  │ move_    │ │move_ │ │move_  │ │                     │
   │  │ snake()  │ │pvp_  │ │battle_│ │                     │
   │  │          │ │snakes│ │snakes │ │                     │
   │  └────┬─────┘ └──┬───┘ └───┬───┘ │                     │
   │       │          │         │     │                     │
   │       ▼          ▼         ▼     │                     │
   │   game_over  game_over  game_over│                     │
   │       │          │         │     │                     │
   │       └──────────┼─────────┘     │                     │
   │                  │               │                     │
   │             按 ESC               │                     │
   └──────────────────┬───────────────┘                     │
                      │                                     │
                      ▼                                     │
              SCENE_MENU ───────────────────────────────────┘
                      │
                 按 ESC
                      │
                      ▼
              SCENE_EXIT → delete current → closegraph()
```

关键点：
- **MenuScene** 是"十字路口"——所有模式从这里出发，最后也回到这里
- **PlayScene** 内部根据 `mode` 字段决定调用哪个 move 函数，外面的 main 循环不需要知道具体模式
- **ESC 键** 在 PlayScene 中返回 `SCENE_MENU`；在 MenuScene 中返回 `SCENE_EXIT`

### 6.4.2 三种初始化函数的对比表

| 方面 | init_game() | init_pvp_game() | init_battle_game() |
|---|---|---|---|
| 地图大小 | 20x20 (MAP_SIZE) | 35x35 (BATTLE_MAP_SIZE) | 35x35 |
| 蛇的数量 | 1 条（玩家） | 2 条（P1 + P2） | 2 条（玩家 + AI） |
| 蛇1 的初始位置 | (8,5)(8,4)(9,4) L形，头朝上 | (17,7)(17,6)(18,6) L形，左侧 | 同 PvP |
| 蛇2 的初始位置 | 不存在 | (17,27)(17,28)(18,28) L形，右侧 | 同 PvP |
| ai_is_alive | false（没有蛇2） | true（蛇2 是 P2） | true（蛇2 是 AI） |
| 障碍物数量 | 12 个 | 30 个 | 30 个 |
| 食物数量 | 1 个（硬编码在 (8,6)） | 2 个（随机） | 2 个（随机） |
| 窗口大小 | 640x600 | 1000x1000 | 1000x1000 |
| 蛇2 的控制者 | 无 | 人类（方向键） | AI（ai_choose_direction） |
| 蛇2 死亡后的行为 | 无 | 游戏结束，P1 胜 | AI 复活（respawn_ai） |
| 游戏结束条件 | 玩家死 | 任何一方死 | 玩家死 |

### 6.4.3 完整数据流：从菜单到 Game Over

在脑海里完整走一遍 PvP 模式的流程：

```
1. main() 启动
   └→ initgraph(640, 600)  // 菜单窗口
   └→ current = new MenuScene()

2. 玩家在菜单按 '2'（PvP 模式）
   └→ MenuScene::update() 检查 key_pressed('2')
   └→ selected_mode = MODE_PVP
   └→ _next = SCENE_PLAY

3. main 循环检测到 next_scene() == SCENE_PLAY
   └→ dynamic_cast 确认 current 是 MenuScene
   └→ 读取 selected_mode = MODE_PVP
   └→ delete MenuScene（释放菜单的内存）
   └→ resize_graph_window(1000, 1000)  // 切换到 35x35 窗口
   └→ current = new PlayScene(MODE_PVP, AI_EASY)
       └→ 构造函数内部调用 init_pvp_game(&_game)
           └→ 地图 35x35，边界 '#'
           └→ P1 蛇 (17,7)(17,6)(18,6)，方向 UP
           └→ P2 蛇 (17,27)(17,28)(18,28)，方向 UP
           └→ 30 个障碍物，2 个食物
           └→ ai_is_alive = true（P2 存活）

4. 游戏主循环（PlayScene::update() 每帧执行）
   └→ 读键盘：
        P1 按 W → set_direction(&_game, UP)
        P2 按方向键左 → set_p2_direction(&_game, LEFT)
   └→ 定时器触发（每 ~160ms）：
        move_pvp_snakes(&_game)
          ├→ next_head(P1) = (16, 7)
          ├→ next_head(P2) = (17, 26)
          ├→ 碰撞检测：地图上两个目标格都是 '.' → 都安全
          ├→ apply_snake_move(P1) → 蛇前移
          ├→ apply_snake_move(P2) → 蛇前移
          └→ 补食物（确保 ≥ 2 个）
   └→ draw_game(&_game)
       ├→ draw_board()：35x35 格，蛇头 H/A，蛇身 B/C
       └→ draw_panel()：P1 分数、P2 分数、控制器提示

5. 某一帧：P2 的头撞到了 P1 的身体
   └→ p2_cell = 'B' → p2_dies = true
   └→ clear_snake_from_map(P2)
   └→ ai_is_alive = false
   └→ game_over = true
   └→ P1 获得击杀奖励：add_pending_growth(P1, P2 身长)
   └→ 画面显示 "P2 LOSES"

6. 玩家按 ESC
   └→ PlayScene::update() 检测 ESC → _next = SCENE_MENU
   └→ main 循环检测到 next_scene() == SCENE_MENU
   └→ delete PlayScene
   └→ resize_graph_window(640, 600)
   └→ current = new MenuScene()
   └→ 回到步骤 2
```

### 6.4.4 最终验证

如果你跟着课程一步一步写完了代码，现在你应该能：

1. 编译整个项目（所有 .cpp 文件 + EasyX 链接）
2. 启动 → 看到主菜单
3. 按 1 → 进入经典模式，WASD 控制，蛇会吃食物、自动增长、撞墙死
4. 按 ESC → 回到菜单
5. 按 2 → 进入 PvP 模式，两个人在同一键盘上对战（P1 = WASD，P2 = 方向键）
6. 按 3 → 选择难度 → 进入 Battle 模式，和 AI 对战，AI 会寻路、会复活
7. 按住 Shift → 蛇加速移动
8. 在任何模式下按 R → 重新开始同一模式

如果你的项目还差一些功能，别着急——对照本章的代码示例和 `game.cpp` 的完整源码，一个函数一个函数地补全。

### 6.4.5 常见错误

**错误 1：场景切换后按键状态残留**

```cpp
// 现象：从游戏回到菜单后，菜单自动响应了上一次的按键
// 原因：reset_key_state() 没有在窗口切换时调用

// CORRECT ✅ —— resize_graph_window 内部必须调用 reset_key_state()
void resize_graph_window(int width, int height)
{
    EndBatchDraw();
    closegraph();
    initgraph(width, height);
    disable_ime();
    reset_key_state();    // ← 这一行不能少！
    BeginBatchDraw();
}
```

**错误 2：PlayScene 构造时 mode 和 difficulty 没传对**

```cpp
// WRONG ❌ —— Battle 模式传了 MODE_CLASSIC
MenuScene *menu = static_cast<MenuScene *>(current);
current = new PlayScene(MODE_CLASSIC, menu->selected_difficulty());
// 玩家选的是 Battle，但构造的是 Classic！

// CORRECT ✅
GameMode mode = menu->selected_mode;  // 从菜单读取
current = new PlayScene(mode, menu->selected_difficulty());
```

### 6.4.6 练一练

**练习：完整画一遍蛇从菜单到 Game Over 的数据流**

选一个模式（经典/PvP/Battle），画出以下流程的每个步骤：
1. main() 启动
2. 菜单上按了什么键
3. PlayScene 构造函数做了什么
4. 游戏循环中的关键数据变化（蛇头位置、分数、食物数量）
5. 什么事件触发了 game_over
6. game_over 后画面显示了什么
7. 玩家按了什么键回到菜单

推荐用"时间线"格式，每一行写当时发生的事件和关键数据。

### 参考答案

<details>
<summary>点击展开：Battle 模式完整数据流</summary>

```
T=0   main() 启动
        → initgraph(640, 600)  菜单窗口
        → current = new MenuScene()

T=1   菜单渲染：draw_menu() 显示三个选项
       玩家按 '3'（Battle 模式）
        → MenuScene::update() 设置 _showing_difficulty = true
        → 菜单显示难度选择：draw_difficulty_menu()

T=2   玩家按 '2'（Normal 难度）
        → _selected_difficulty = AI_NORMAL
        → selected_mode = MODE_BATTLE
        → _next = SCENE_PLAY

T=3   main 循环检测切换
        → delete current (MenuScene)
        → resize_graph_window(1000, 1000)  ← 从 640×600 变 1000×1000
        → current = new PlayScene(MODE_BATTLE, AI_NORMAL)
          → 构造函数: init_battle_game(&_game, AI_NORMAL)
            → map_size = 35, 地图清空 + 边界 '#'
            → P1蛇: snake = [(17,7), (17,6), (18,6)], dir=UP, length=3
            → AI蛇: ai_snake = [(17,27), (17,28), (18,28)], dir=UP, length=3
            → ai_is_alive = true, ai_difficulty = AI_NORMAL
            → generate_obstacles(30个)
            → place_food() × 2

T=4~  游戏循环开始（每帧约 16ms）
        T=4  draw_game(&_game):
               draw_board(): 35×35 格，H=玩家头, A=AI头, F=食物
               draw_panel(): Player Score=0, AI Score=0, AI Difficulty=Normal

        T=100 玩家按 W → set_direction(UP) → started=true
        T=120 定时器触发（160ms 一次）
               move_battle_snakes(&_game):
                 p1_next = (16, 7): map='.' → 安全
                 ai_dir = ai_choose_direction(Normal) → AI_NORMAL BFS 寻路
                 假设 AI 也向上 → ai_next = (16, 27): map='.' → 安全
                 双方执行移动, move_count=1

        T=300 经过几帧，move_count=15
               玩家吃了一个食物: score=10, food_eaten=1
               AI 也吃了一个食物: ai_score=10, ai_food_eaten=1

        T=500 move_count=50, 自动增长了 6 次 (50/8≈6)
               玩家蛇长度: 3 + 6(auto) + 1(food) = 10
               AI蛇长度:   3 + 6(auto) + 2(food) = 11

        T=1200 move_count=200
               地图越来越挤，两条蛇都超过 20 节
               玩家向右，AI 向下，玩家蛇头 (5, 18)
               AI蛇头在 (20, 15)，AI 身体在 (19,15) (18,15) ...

        T=1250 关键帧：
               p1_next = (5, 19): map='C' → 玩家头撞到 AI 身体！
               碰撞检测步骤2: p1_cell='C', !ai_will_grow, 不是尾巴
               → p1_dies=true, p1_reason=DEATH_ENEMY_BODY
               → ai_grows_from_kill=true, ai_grow_amount=25 (玩家身长)

               执行结果: p1_dies → game_over=true
               AI 获得击杀奖励: ai_pending_growth += 25
               AI 走最后一帧: apply_snake_move(AI)
               (玩家不走了——它死了)

T=1251 画面渲染：
          draw_board(): 玩家蛇消失（game_over 时不清除，但蛇不再移动）
          draw_panel(): Status: You Lose
          draw_game_over(): 显示 "YOU LOSE" + Score: XX

T=1300 玩家按 ESC
          → PlayScene::update() → _next = SCENE_MENU
          → main 检测切换 → delete PlayScene
          → resize_graph_window(640, 600)
          → current = new MenuScene()

T=1301 菜单再次渲染，等待下一次选择
```

</details>

---

## 课程完结感言

好了——这是第六章的最后一页，也是整个 Snake 课程的最后一页。

回头看看你从第零章那个红色小圆（EasyX 的 `solidcircle(100, 100, 20)`）走到这里的路：

| 章节 | 你学会了什么 |
|------|------------|
| 第〇章 | 安装 VS 2022 + EasyX，画了第一个红色小圆 |
| 第一章 | C++ 基础语法：变量、if/for/while、函数、数组、结构体、指针。从"main 是什么"到"能读源码" |
| 第二章 | 地图与蛇身：二维字符数组表示棋盘，Point 坐标结构体，枚举管理方向和模式 |
| 第三章 | EasyX 渲染：双缓冲绘图、单元格绘制、右侧信息面板、Game Over 画面 |
| 第四章 | 游戏逻辑：食物随机生成、碰撞检测（六种死法）、尾部安全、自动增长、init_game 串联 |
| 第五章 | AI 对手：BFS 寻路算法、Easy/Normal/Hard 三种决策策略、尾部安全可达性检查 |
| 第六章（本章） | 多玩家与战斗系统：双蛇同时移动、碰撞优先级、pending_growth 击杀奖励、Scene 多态场景管理 |

你从一个不知道 `int main()` 是什么的初学者，变成了一个能写出**含主菜单、三种游戏模式、AI 对手、双人对战**的完整游戏的程序员。

那个第零章的红色小圆 `solidcircle(100, 100, 20)`，现在变成了 1000x1000 像素窗口里游动的绿色蛇头、蓝色 AI 蛇、红色食物、灰色边界——每一帧都由你写的代码驱动。

这就是编程带给人的力量：从一个想法开始，一行一行搭建，最后拥有一个**真正能跑的、可以给朋友玩的**程序。

课程到这里就结束了，但你的学习才刚刚开始。这个 Snake 项目是一个绝佳的实验场——你可以给它加新功能（加速道具？传送门？排行榜？），重构它的代码（把 GameState 拆成多个小结构体？用真正的 ECS 架构？），或者把它当作跳板去学更复杂的游戏（俄罗斯方块？横版过关？）。

记住第六章的核心信息——**多态让扩展变得简单**。如果你想给 Snake 加一个新模式（比如"限时模式"或"镜像模式"），你只需要：
1. 在 MenuScene 里加一个菜单项
2. 在 PlayScene 里加一个 `if (mode == MODE_NEW) { ... }`
3. 在 game.cpp 里写移动逻辑

main.cpp 不用改。这就是面向对象设计的承诺。

祝你编程愉快，写出你想写的一切。

---

## 本章回顾

| 节 | 主题 | 核心内容 | 关键概念 |
|----|------|---------|---------|
| 6.1 | 状态管理 | GameState 完整布局、两蛇字段对照 | 共享结构体的设计理由、字段复用 |
| 6.2 | 碰撞解决 | move_pvp_snakes() 7步碰撞检测 + 执行 | 碰撞顺序、pending_growth、apply 抽象 |
| 6.3 | 场景管理 | Scene 抽象基类、多态、场景切换 | 虚函数表、dynamic_cast、关注点分离 |
| 6.4 | 全景串联 | 状态机图、三模式对比、完整数据流 | 系统思维、端到端追踪能力 |

**学完本章后，你应该能回答：**
- 为什么两条蛇放在同一个 GameState 里而不是两个独立对象？
- move_pvp_snakes() 的碰撞检测顺序为什么是 1→2→3→4→5→6→7，不能颠倒？
- pending_growth 机制如何实现"击杀敌人后逐步变长"的效果？
- Scene 抽象基类的三个纯虚函数各自是什么职责？
- main() 的 while 循环如何做到"不关心当前是什么场景"？
- 从程序启动到 Battle 模式 Game Over 的完整数据流是怎样的？
