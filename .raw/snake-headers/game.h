#pragma once
#ifndef GAME_H
#define GAME_H

// ============================================================================
// game.h — 贪吃蛇游戏核心数据结构与常量定义
//
// 本头文件定义了所有游戏模式共享的：
//   - 地图 / 窗口 / 绘制常量宏
//   - Point 坐标结构体
//   - GameState 完整游戏状态结构体
//   - Direction / GameMode / AIDifficulty / DeathReason 枚举
//   - 所有 game.cpp 中定义的函数声明
//
// 依赖关系：本文件是最底层头文件，不依赖任何其他项目头文件。
// ============================================================================

// ---------------------------------------------------------------------------
// 地图与网格常量
// ---------------------------------------------------------------------------

// 经典模式使用 20x20 地图；对战 / PvP 模式使用 35x35 地图
#define MAP_SIZE            20    // 经典模式地图边长（格数）
#define BATTLE_MAP_SIZE     35    // 对战 / PvP 模式地图边长（格数）
#define MAX_MAP_SIZE        35    // 最大地图边长（用于静态数组维度声明）

// 蛇身最大长度 —— 同时用作 collect_empty_cells 的上限和 AI BFS 队列容量。
// 在 35x35 地图上仅取前 400 个空位已足够食物随机放置；BFS 在两条蛇挤压
// 空间后访问节点数通常不会溢出。
#define MAX_SNAKE_LEN       400

// ---------------------------------------------------------------------------
// 游戏机制常量
// ---------------------------------------------------------------------------

#define GROW_STEP           8     // 每移动 N 步自动增长一节
#define MOVE_INTERVAL_MS    160   // 标准移动间隔（毫秒），scene.cpp 中用于帧计时

// ---------------------------------------------------------------------------
// 绘制 / 布局常量（render.cpp 使用）
// ---------------------------------------------------------------------------

#define CELL_SIZE           25    // 每格边长（像素）
#define BOARD_LEFT          20    // 棋盘区域左边距（像素）
#define BOARD_TOP           20    // 棋盘区域上边距（像素）

// ---------------------------------------------------------------------------
// 窗口尺寸常量（main.cpp / scene.cpp 用于 resize 和 initgraph）
// ---------------------------------------------------------------------------

#define MENU_WINDOW_WIDTH     640   // 主菜单窗口宽度
#define MENU_WINDOW_HEIGHT    600   // 主菜单窗口高度
#define CLASSIC_WINDOW_WIDTH  640   // 经典模式窗口宽度
#define CLASSIC_WINDOW_HEIGHT 600   // 经典模式窗口高度
#define BATTLE_WINDOW_WIDTH   1000  // 对战 / PvP 模式窗口宽度
#define BATTLE_WINDOW_HEIGHT  1000  // 对战 / PvP 模式窗口高度

// ============================================================================
// 枚举定义
// ============================================================================

// 移动方向 —— 蛇头朝向与行进方向
enum Direction {
    UP,      // 上（-row）
    DOWN,    // 下（+row）
    LEFT,    // 左（-col）
    RIGHT    // 右（+col）
};

// 游戏模式 —— 决定地图大小、蛇的数量和控制方式
enum GameMode {
    MODE_CLASSIC,   // 经典单人模式：20x20，一条蛇，需躲避随机障碍物
    MODE_PVP,       // PvP 双人对战：35x35，两条蛇分别由两名玩家操控（WASD / 方向键）
    MODE_BATTLE     // 人机对战：35x35，玩家 vs AI（Easy / Normal / Hard）
};

// AI 难度等级 —— 影响 ai_choose_direction() 的决策策略
enum AIDifficulty {
    AI_EASY,    // 简单：30% 随机移动，否则按曼哈顿距离寻食物
    AI_NORMAL,  // 普通：BFS 寻食物 + 15% 随机干扰，无尾部安全检测
    AI_HARD     // 困难：BFS 寻食物，蛇长 >= 105 时启用尾部可达性安全检查
};

// 死亡原因 —— 用于 game over 面板显示和碰撞检测返回值
enum DeathReason {
    DEATH_NONE,         // 存活中（初始值 / 无死亡）
    DEATH_WALL,         // 撞到地图边界 '#'
    DEATH_OBSTACLE,     // 撞到障碍物 'O'
    DEATH_BODY,         // 撞到自身蛇身 'B' 或 'C'
    DEATH_ENEMY_BODY,   // 撞到敌方蛇身（对战模式）
    DEATH_HEAD_ON,      // 两条蛇头部相撞（对战 / PvP 模式）
    DEATH_FULL_MAP      // 地图已满，无处放置食物
};

// ============================================================================
// 结构体定义
// ============================================================================

// 棋盘坐标 —— 行 / 列从 0 开始，(0,0) 为左上角
struct Point {
    int row;   // 行号
    int col;   // 列号
};

// 游戏状态 —— 一局游戏的全部运行时数据。
// 所有移动 / 渲染 / AI 函数均通过 const / 非 const 指针操作此结构体。
struct GameState {
    // ---- 地图 ----
    // 字符含义：
    //   '#' = 边界墙    '.' = 空地      'F' = 食物
    //   'O' = 障碍物    'H' = 玩家蛇头  'B' = 玩家蛇身
    //   'A' = AI 蛇头   'C' = AI 蛇身
    char map[MAX_MAP_SIZE][MAX_MAP_SIZE];
    int map_size;                       // 当前地图边长（MAP_SIZE 或 BATTLE_MAP_SIZE）

    // ---- 玩家蛇 ----
    Point snake[MAX_SNAKE_LEN];         // 蛇身坐标，snake[0] 为蛇头
    int length;                         // 当前长度（节数）
    int score;                          // 累计分数
    Direction direction;                // 当前移动方向
    bool turn_locked;                   // 本帧是否已转向（防止一帧内多次 180° 反转）
    int food_eaten;                     // 累计吃到食物次数
    int pending_growth;                 // 待应用的生长节数（击杀敌方蛇后累积）

    // ---- AI 蛇 / 2P 蛇（两组模式复用同一组字段） ----
    Point ai_snake[MAX_SNAKE_LEN];      // AI / 2P 蛇身坐标
    int ai_length;                      // AI / 2P 当前长度
    Direction ai_direction;             // AI / 2P 当前方向
    bool ai_is_alive;                   // AI / 2P 是否存活
    int ai_score;                       // AI / 2P 累计分数
    int ai_food_eaten;                  // AI / 2P 累计吃到食物次数
    int ai_pending_growth;              // AI / 2P 待应用生长节数
    DeathReason ai_death_reason;        // AI / 2P 死亡原因
    AIDifficulty ai_difficulty;         // AI 难度等级（仅 MODE_BATTLE 有效）

    // ---- PvP 2P 专用控制字段 ----
    Direction p2_direction;             // 2P 方向（MODE_PVP）
    bool p2_turn_locked;                // 2P 本帧转向锁定（MODE_PVP）

    // ---- 游戏流程控制 ----
    GameMode mode;                      // 当前游戏模式
    bool started;                       // 是否已开始（玩家首次输入后置 true）
    bool paused;                        // 是否暂停（空格切换）
    bool game_over;                     // 游戏是否已结束
    int move_count;                     // 已执行移动步数（用于自动增长计算和速度调节）
    int grow_step;                      // 每 N 步自动增长一节（默认 GROW_STEP）
    DeathReason death_reason;           // 玩家死亡原因
};

// ============================================================================
// 函数声明（定义在 game.cpp）
// ============================================================================

// 初始化经典单人模式 —— 20x20 地图，一条蛇，12 个障碍物，1 个食物
void init_game(GameState *game, GameMode mode);

// 初始化人机对战模式 —— 35x35 地图，玩家 vs AI，30 个障碍物，2 个食物
void init_battle_game(GameState *game, AIDifficulty difficulty);

// 初始化 PvP 双人对战 —— 35x35 地图，两名玩家，30 个障碍物，2 个食物
void init_pvp_game(GameState *game);

// 经典 / 障碍模式的单蛇移动（含吃食物、自动增长、撞墙 / 撞身检测）
void move_snake(GameState *game);

// 人机对战模式的双蛇移动（含碰撞检测与击杀逻辑）
void move_battle_snakes(GameState *game);

// PvP 模式的双蛇移动（含碰撞检测与击杀逻辑）
void move_pvp_snakes(GameState *game);

// 设置玩家蛇方向 —— 防止反向和本帧重复转向
void set_direction(GameState *game, Direction dir);

// 设置 2P 蛇方向 —— PvP 模式专用
void set_p2_direction(GameState *game, Direction dir);

// 在随机空位生成指定数量的障碍物（上限为可放置格数的 25%）
void generate_obstacles(GameState *game, int count);

// 复活 AI 蛇 —— 清除旧身体，在随机空位放置 3 节新蛇（头朝上），补足食物
void respawn_ai(GameState *game);

// 检查玩家能否沿指定方向开始移动（首次移动时调用，防止开局撞墙）
bool can_start_in_direction(GameState *game, Direction dir);

// 重新开始当前游戏 —— 根据 game->mode 调用对应 init_* 函数重置所有状态
void restart_game(GameState *game);

// 调整图形窗口尺寸 —— 关闭当前窗口后以新尺寸重新打开（Battle / PvP 切换用）
void resize_graph_window(int width, int height);

#endif // GAME_H
