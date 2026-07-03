#pragma once
#include <raylib.h>
#include <time.h>

#define MAP_SIZE 35
#define MAX_MAP_SIZE 40
#define BATTLE_MAP_SIZE 35
#define MAX_SNAKE_LEN (MAX_MAP_SIZE * MAX_MAP_SIZE)
#define CELL_SIZE 24
#define BOARD_LEFT 32
#define BOARD_TOP 48
// Window sizes for different modes
#define MENU_WINDOW_WIDTH   700
#define MENU_WINDOW_HEIGHT  620
#define CLASSIC_WINDOW_WIDTH  1140
#define CLASSIC_WINDOW_HEIGHT 920
#define BATTLE_WINDOW_WIDTH  1180
#define BATTLE_WINDOW_HEIGHT 920

// Legacy aliases (used by battle / PvP modes)
#define WINDOW_WIDTH  BATTLE_WINDOW_WIDTH
#define WINDOW_HEIGHT BATTLE_WINDOW_HEIGHT

#define MOVE_INTERVAL_MS 160
#define GROW_STEP 32

// Dropped tail blocks (P1=P1 food/P2 obstacle, P2=P2 food/P1 obstacle)
#define MAP_CHAR_P1_DROP 'D'
#define MAP_CHAR_P2_DROP 'E'

enum Direction
{
    UP = 'W',
    DOWN = 'S',
    LEFT = 'A',
    RIGHT = 'D'
};
enum DeathReason
{
    DEATH_NONE = 0,
    DEATH_WALL,
    DEATH_OBSTACLE,
    DEATH_BODY,
    DEATH_ENEMY_BODY,
    DEATH_HEAD_ON,
    DEATH_FULL_MAP
};
enum GameMode
{
    MODE_CLASSIC,
    MODE_PVP,
    MODE_BATTLE
};
enum AIDifficulty
{
    AI_EASY,
    AI_NORMAL,
    AI_HARD
};

typedef struct point
{
    int row, col;
} Point;

typedef struct gamestate
{
    char map[MAX_MAP_SIZE][MAX_MAP_SIZE];
    int map_size;

    // --- Player (human) snake ---
    Point snake[MAX_SNAKE_LEN];
    int length;
    int score;
    int move_count;
    int grow_step;
    Direction direction;
    DeathReason death_reason;
    bool started;
    bool paused;
    bool game_over;
    GameMode mode;
    int food_eaten;
    int pending_growth;
    int high_score;  // 经典模式历史最高分

    // --- Second player / AI snake (PvP / battle mode) ---
    Point ai_snake[MAX_SNAKE_LEN];
    int ai_length;
    Direction ai_direction;
    bool ai_is_alive;
    int ai_score;
    int ai_food_eaten;
    int ai_pending_growth;
    AIDifficulty ai_difficulty;
    DeathReason ai_death_reason;

    // --- P2 controls (PvP mode only) ---
    Direction p2_direction;

    // --- Drop-tail mechanic (PvP / Battle) ---
    bool p1_drop_requested;
    bool p2_drop_requested;
} GameState;

void init_game(GameState *game, GameMode mode);
void set_direction(GameState *game, Direction dir);
void move_snake(GameState *game);

// PvP mode functions
void init_pvp_game(GameState *game);
void move_pvp_snakes(GameState *game, bool move_p1, bool move_p2);
void set_p2_direction(GameState *game, Direction dir);

// Battle mode functions
void init_battle_game(GameState *game, AIDifficulty difficulty);
void move_battle_snakes(GameState *game, bool move_p1, bool move_p2);
void respawn_ai(GameState *game);

// Obstacle generation
void generate_obstacles(GameState *game, int count);

// High score persistence (classic mode)
void load_high_score(GameState *game);
void save_high_score(const GameState *game);
