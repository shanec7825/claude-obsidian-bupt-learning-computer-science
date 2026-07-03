#include "game.h"
#include "ai.h"
#include "input.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static bool is_same_point(Point a, Point b)
{
    return a.row == b.row && a.col == b.col;
}
static bool is_opposite(Direction a, Direction b)
{
    return (a == UP && b == DOWN) || (a == DOWN && b == UP) ||
           (a == LEFT && b == RIGHT) || (a == RIGHT && b == LEFT);
}

static void clear_map(GameState *game)
{
    for (int r = 0; r < game->map_size; r++)
        for (int c = 0; c < game->map_size; c++)
            game->map[r][c] = (r == 0 || r == game->map_size - 1 ||
                               c == 0 || c == game->map_size - 1)
                                  ? '#'
                                  : '.';
}

static void put_snake_on_map_impl(GameState *game, const Point *body, int len,
                                   char head_char, char body_char)
{
    for (int i = 0; i < len; i++)
    {
        Point p = body[i];
        game->map[p.row][p.col] = (i == 0) ? head_char : body_char;
    }
}

static void put_player_snake_on_map(GameState *game)
{
    put_snake_on_map_impl(game, game->snake, game->length, 'H', 'B');
}

static void put_ai_snake_on_map(GameState *game)
{
    if (game->ai_is_alive)
        put_snake_on_map_impl(game, game->ai_snake, game->ai_length, 'A', 'C');
}

static int collect_empty_cells(GameState *game, Point cells[MAX_SNAKE_LEN])
{
    int count = 0;
    for (int r = 1; r < game->map_size - 1; r++)
        for (int c = 1; c < game->map_size - 1; c++)
            if (game->map[r][c] == '.')
            {
                cells[count].row = r;
                cells[count].col = c;
                count++;
            }
    return count;
}

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

// Generate random obstacles on empty cells.
// count: desired number (capped at 25% of available empty cells).
void generate_obstacles(GameState *game, int count)
{
    // Collect all empty interior cells
    Point cells[MAX_SNAKE_LEN];
    int total = 0;
    for (int r = 1; r < game->map_size - 1; r++)
        for (int c = 1; c < game->map_size - 1; c++)
            if (game->map[r][c] == '.')
            {
                cells[total].row = r;
                cells[total].col = c;
                total++;
            }

    // Cap at 25% of empty cells to keep the map playable
    int max_obstacles = total / 4;
    if (count > max_obstacles)
        count = max_obstacles;
    if (count <= 0)
        return;

    // Partial Fisher-Yates shuffle: randomly select 'count' cells
    for (int i = 0; i < count; i++)
    {
        int j = i + rand() % (total - i);
        game->map[cells[j].row][cells[j].col] = 'O';
        Point tmp = cells[i];
        cells[i] = cells[j];
        cells[j] = tmp;
    }
}

void init_game(GameState *game, GameMode mode)
{
    srand((unsigned)time(NULL));
    game->map_size = MAP_SIZE;
    clear_map(game);

    game->snake[0] = {8, 5};
    game->snake[1] = {8, 4};
    game->snake[2] = {9, 4};
    game->length = 3;
    game->score = 0;
    game->move_count = 0;
    game->grow_step = GROW_STEP;
    game->direction = UP;
    game->death_reason = DEATH_NONE;
    game->started = false;
    game->paused = false;
    game->game_over = false;
    game->mode = mode;
    game->food_eaten = 0;
    game->pending_growth = 0;

    // 加载历史最高分（仅经典模式）
    if (mode == MODE_CLASSIC)
        load_high_score(game);
    else
        game->high_score = 0;

    // Zero AI fields for non-battle modes
    game->ai_length = 0;
    game->ai_direction = UP;
    game->ai_is_alive = false;
    game->ai_score = 0;
    game->ai_food_eaten = 0;
    game->ai_pending_growth = 0;
    game->ai_death_reason = DEATH_NONE;

    put_player_snake_on_map(game);
    generate_obstacles(game, 12);
    game->map[8][6] = 'F';
}

// Initialize battle mode: 35x35 map, two snakes, two food items
void init_battle_game(GameState *game, AIDifficulty difficulty)
{
    srand((unsigned)time(NULL));
    game->map_size = BATTLE_MAP_SIZE;
    game->ai_difficulty = difficulty;
    clear_map(game);
    // No default obstacles in battle mode

    // Player snake on the left side
    int mid = BATTLE_MAP_SIZE / 2;  // 17
    game->snake[0] = {mid, 7};
    game->snake[1] = {mid, 6};
    game->snake[2] = {mid + 1, 6};
    game->length = 3;
    game->score = 0;
    game->move_count = 0;
    game->grow_step = GROW_STEP;
    game->direction = UP;
    game->death_reason = DEATH_NONE;
    game->started = false;
    game->paused = false;
    game->game_over = false;
    game->mode = MODE_BATTLE;
    game->food_eaten = 0;
    game->pending_growth = 0;

    // AI snake on the right side
    game->ai_snake[0] = {mid, BATTLE_MAP_SIZE - 8};
    game->ai_snake[1] = {mid, BATTLE_MAP_SIZE - 7};
    game->ai_snake[2] = {mid + 1, BATTLE_MAP_SIZE - 7};
    game->ai_length = 3;
    game->ai_direction = UP;
    game->ai_is_alive = true;
    game->ai_score = 0;
    game->ai_food_eaten = 0;
    game->ai_pending_growth = 0;
    game->ai_death_reason = DEATH_NONE;

    put_player_snake_on_map(game);
    put_ai_snake_on_map(game);

    generate_obstacles(game, 30);

    // Place two food items
    place_food(game);
    if (!game->game_over)
        place_food(game);
}

static Point next_head(Point head, Direction dir)
{
    Point next = head;
    switch (dir)
    {
    case UP:
        next.row--;
        break;
    case DOWN:
        next.row++;
        break;
    case LEFT:
        next.col--;
        break;
    case RIGHT:
        next.col++;
        break;
    }
    return next;
}

static bool can_enter_cell(GameState *game, Point next, bool will_grow,
                            const Point *body, int length, DeathReason *out_reason)
{
    char cell = game->map[next.row][next.col];
    Point tail = body[length - 1];

    if (cell == '#')
    {
        if (out_reason) *out_reason = DEATH_WALL;
        return false;
    }
    if (cell == 'O')
    {
        if (out_reason) *out_reason = DEATH_OBSTACLE;
        return false;
    }
    // Player body characters
    if (cell == 'B')
    {
        if (!will_grow && is_same_point(next, tail))
            return true;
        if (out_reason) *out_reason = DEATH_BODY;
        return false;
    }
    // AI body characters
    if (cell == 'C')
    {
        if (!will_grow && is_same_point(next, tail))
            return true;
        if (out_reason) *out_reason = DEATH_ENEMY_BODY;
        return false;
    }
    // Opponent head
    if (cell == 'H' || cell == 'A')
    {
        if (out_reason) *out_reason = DEATH_HEAD_ON;
        return false;
    }
    return true;
}

bool can_start_in_direction(GameState *game, Direction dir)
{
    Point next = next_head(game->snake[0], dir);
    DeathReason dummy = DEATH_NONE;
    return can_enter_cell(game, next, false, game->snake, game->length, &dummy);
}

// Original move_snake for classic/obstacle modes
void move_snake(GameState *game)
{
    if (!game->started || game->paused || game->game_over)
        return;

    Point old_head = game->snake[0];
    Point next = next_head(old_head, game->direction);
    bool will_eat = (game->map[next.row][next.col] == 'F');
    bool auto_grow = ((game->move_count + 1) % game->grow_step) == 0;
    bool will_grow = will_eat || auto_grow;

    DeathReason reason = DEATH_NONE;
    if (!can_enter_cell(game, next, will_grow, game->snake, game->length, &reason))
    {
        game->game_over = true;
        game->death_reason = reason;
        return;
    }

    // Remove tail
    if (!will_grow)
    {
        Point tail = game->snake[game->length - 1];
        game->map[tail.row][tail.col] = '.';
    }
    else if (game->length < MAX_SNAKE_LEN)
    {
        game->length++;
    }

    // Old head becomes body
    game->map[old_head.row][old_head.col] = 'B';

    // Shift body
    for (int i = game->length - 1; i > 0; i--)
        game->snake[i] = game->snake[i - 1];
    game->snake[0] = next;
    game->map[next.row][next.col] = 'H';
    game->move_count++;

    if (will_eat)
    {
        game->score += 10;
        game->food_eaten++;
        if (game->mode == MODE_CLASSIC && game->score > game->high_score)
        {
            game->high_score = game->score;
            save_high_score(game);
        }
        place_food(game);
    }
}

// Add pending growth credits (tail won't be removed for this many moves)
static void add_pending_growth(int *pending, int amount)
{
    *pending += amount;
}

// Remove a snake's body cells from the map
static void clear_snake_from_map(GameState *game, const Point *body, int length,
                                  char head_char, char body_char)
{
    for (int i = 0; i < length; i++)
    {
        Point p = body[i];
        char ch = game->map[p.row][p.col];
        if (ch == head_char || ch == body_char)
            game->map[p.row][p.col] = '.';
    }
}

// Apply a single snake move. Returns true if food was eaten.
static bool apply_snake_move(GameState *game, Point *body, int *length,
                              Direction dir, int *score, int *food_eaten,
                              int *pending_growth,
                              char head_char, char body_char)
{
    Point old_head = body[0];
    Point next = next_head(old_head, dir);
    bool will_eat = (game->map[next.row][next.col] == 'F');
    bool auto_grow = ((game->move_count + 1) % game->grow_step) == 0;
    bool has_pending = (*pending_growth > 0);
    bool effective_grow = will_eat || auto_grow || has_pending;

    // Remove tail
    if (!effective_grow)
    {
        Point tail = body[*length - 1];
        game->map[tail.row][tail.col] = '.';
    }
    else if (*length < MAX_SNAKE_LEN)
    {
        (*length)++;
    }

    // Consume one pending growth credit (only when not eating/auto-growing)
    if (has_pending && !will_eat && !auto_grow)
        (*pending_growth)--;

    // Old head becomes body
    game->map[old_head.row][old_head.col] = body_char;

    // Shift body
    for (int i = *length - 1; i > 0; i--)
        body[i] = body[i - 1];
    body[0] = next;
    game->map[next.row][next.col] = head_char;

    if (will_eat)
    {
        *score += 10;
        (*food_eaten)++;
    }

    return will_eat;
}

// Respawn the AI snake at a random empty location
void respawn_ai(GameState *game)
{
    // Clear old AI body from map
    clear_snake_from_map(game, game->ai_snake, game->ai_length, 'A', 'C');

    // Collect empty cells
    Point cells[MAX_SNAKE_LEN];
    int count = collect_empty_cells(game, cells);
    if (count < 4)
    {
        game->ai_is_alive = false;
        game->ai_length = 0;
        game->game_over = true;
        return;
    }

    // Try to find a position with room for a 3-cell snake
    bool placed = false;
    Point start;
    for (int attempt = 0; attempt < 200 && !placed; attempt++)
    {
        int idx = rand() % count;
        start = cells[idx];
        // Check if start and the two cells below are empty
        if (start.row + 2 < game->map_size - 1)
        {
            bool ok = true;
            for (int dr = 0; dr < 3; dr++)
            {
                char ch = game->map[start.row + dr][start.col];
                if (ch != '.')
                {
                    ok = false;
                    break;
                }
            }
            if (ok)
                placed = true;
        }
    }

    if (!placed)
    {
        // Fallback: just place a 1-cell snake
        start = cells[rand() % count];
    }

    // Place new AI snake, heading UP
    int len = placed ? 3 : 1;
    for (int i = 0; i < len; i++)
    {
        game->ai_snake[i] = {start.row + (len - 1 - i), start.col};
    }
    game->ai_length = len;
    game->ai_direction = UP;
    game->ai_is_alive = true;
    game->ai_score = 0;
    game->ai_food_eaten = 0;
    game->ai_pending_growth = 0;
    game->ai_death_reason = DEATH_NONE;

    put_ai_snake_on_map(game);

    // Ensure there are at least 2 food items on the map
    int food_count = 0;
    for (int r = 0; r < game->map_size; r++)
        for (int c = 0; c < game->map_size; c++)
            if (game->map[r][c] == 'F')
                food_count++;
    while (food_count < 2 && !game->game_over)
    {
        place_food(game);
        food_count++;
    }
}

// Move both snakes in battle mode with collision resolution
void move_battle_snakes(GameState *game)
{
    if (!game->started || game->paused || game->game_over)
        return;

    // Compute AI direction
    Direction ai_dir = ai_choose_direction(game, game->ai_difficulty);
    game->ai_direction = ai_dir;

    // Compute next head positions
    Point player_next = next_head(game->snake[0], game->direction);
    Point ai_next = next_head(game->ai_snake[0], ai_dir);

    // Check what's at each next cell
    char player_cell = game->map[player_next.row][player_next.col];
    char ai_cell = game->map[ai_next.row][ai_next.col];

    bool player_will_eat = (player_cell == 'F');
    bool ai_will_eat = (ai_cell == 'F');
    bool auto_grow = ((game->move_count + 1) % game->grow_step) == 0;
    bool player_will_grow = player_will_eat || auto_grow;
    bool ai_will_grow = ai_will_eat || auto_grow;

    bool player_dies = false;
    bool ai_dies = false;
    DeathReason player_reason = DEATH_NONE;
    DeathReason ai_reason = DEATH_NONE;
    bool player_grows_from_kill = false;
    bool ai_grows_from_kill = false;
    int player_grow_amount = 0;
    int ai_grow_amount = 0;

    // --- Collision Resolution ---

    // 1. Head-on collision
    if (is_same_point(player_next, ai_next))
    {
        player_dies = true;
        player_reason = DEATH_HEAD_ON;
        ai_dies = true;
        ai_reason = DEATH_HEAD_ON;
    }
    else
    {
        // 2. Player head hits AI body
        if (player_cell == 'C')
        {
            // Check if AI is not growing and this is AI's tail — then it's safe
            if (!ai_will_grow && is_same_point(player_next, game->ai_snake[game->ai_length - 1]))
            {
                // Player can pass through AI's tail that's about to move
            }
            else
            {
                player_dies = true;
                player_reason = DEATH_ENEMY_BODY;
                ai_grows_from_kill = true;
                ai_grow_amount = game->length;
            }
        }

        // 3. AI head hits player body
        if (ai_cell == 'B')
        {
            if (!player_will_grow && is_same_point(ai_next, game->snake[game->length - 1]))
            {
                // AI can pass through player's tail that's about to move
            }
            else
            {
                ai_dies = true;
                ai_reason = DEATH_ENEMY_BODY;
                player_grows_from_kill = true;
                player_grow_amount = game->ai_length;
            }
        }

        // 4. Player hits wall/obstacle
        if (player_cell == '#' || player_cell == 'O')
        {
            player_dies = true;
            player_reason = (player_cell == '#') ? DEATH_WALL : DEATH_OBSTACLE;
        }

        // 5. AI hits wall/obstacle
        if (ai_cell == '#' || ai_cell == 'O')
        {
            ai_dies = true;
            ai_reason = (ai_cell == '#') ? DEATH_WALL : DEATH_OBSTACLE;
        }

        // 6. Player self-collision
        if (!player_dies && player_cell == 'B')
        {
            if (!player_will_grow && is_same_point(player_next, game->snake[game->length - 1]))
            {
                // safe — own tail
            }
            else
            {
                player_dies = true;
                player_reason = DEATH_BODY;
            }
        }

        // 7. AI self-collision
        if (!ai_dies && ai_cell == 'C')
        {
            if (!ai_will_grow && is_same_point(ai_next, game->ai_snake[game->ai_length - 1]))
            {
                // safe — own tail
            }
            else
            {
                ai_dies = true;
                ai_reason = DEATH_BODY;
            }
        }

        // 8. Player hits AI body tail (already handled in case 2)
    }

    // --- Execute Results ---

    // Handle AI death — player grows
    if (ai_dies)
    {
        clear_snake_from_map(game, game->ai_snake, game->ai_length, 'A', 'C');
        game->ai_is_alive = false;
        game->ai_death_reason = ai_reason;

        if (player_grows_from_kill && player_grow_amount > 0)
        {
            add_pending_growth(&game->pending_growth, player_grow_amount);
        }
    }

    // Handle player death
    if (player_dies)
    {
        game->death_reason = player_reason;
        game->game_over = true;

        if (ai_grows_from_kill && ai_grow_amount > 0)
        {
            add_pending_growth(&game->ai_pending_growth, ai_grow_amount);
        }

        // If AI is still alive, apply its move too (it gets the cell)
        if (!ai_dies && game->ai_is_alive)
        {
            int ate = apply_snake_move(game, game->ai_snake, &game->ai_length,
                                        ai_dir, &game->ai_score, &game->ai_food_eaten,
                                        &game->ai_pending_growth,
                                        'A', 'C');
            if (ate)
            {
                place_food(game);
                // Ensure at least 2 food items
                int fc = 1; // one just placed, check for more
                for (int r = 0; r < game->map_size; r++)
                    for (int c = 0; c < game->map_size; c++)
                        if (game->map[r][c] == 'F')
                            fc++;
                (void)fc; // food was placed, AI may have eaten one
            }
        }

        game->move_count++;
        return;
    }

    // Apply player move
    if (!player_dies)
    {
        bool ate = apply_snake_move(game, game->snake, &game->length,
                                     game->direction, &game->score, &game->food_eaten,
                                     &game->pending_growth,
                                     'H', 'B');
        if (ate)
            place_food(game);
    }

    // Apply AI move
    if (!ai_dies && game->ai_is_alive)
    {
        bool ate = apply_snake_move(game, game->ai_snake, &game->ai_length,
                                     ai_dir, &game->ai_score, &game->ai_food_eaten,
                                     &game->ai_pending_growth,
                                     'A', 'C');
        if (ate)
            place_food(game);
    }

    // Ensure at least 2 food items on the map in battle mode
    if (!game->game_over)
    {
        int food_count = 0;
        for (int r = 0; r < game->map_size; r++)
            for (int c = 0; c < game->map_size; c++)
                if (game->map[r][c] == 'F')
                    food_count++;
        while (food_count < 2 && !game->game_over)
        {
            place_food(game);
            food_count++;
        }
    }

    // Respawn AI if it died
    if (ai_dies)
    {
        respawn_ai(game);
    }

    game->move_count++;
}

void set_direction(GameState *game, Direction dir)
{
    if (!game->started)
    {
        if (can_start_in_direction(game, dir))
        {
            game->direction = dir;
            game->started = true;
        }
        return;
    }

    if (!is_opposite(game->direction, dir))
    {
        game->direction = dir;
        game->started = true;
    }
}

// ---------------------------------------------------------------------------
// PvP mode: two human players
// ---------------------------------------------------------------------------

// Check if P2 can start in a given direction
static bool p2_can_start_in_direction(GameState *game, Direction dir)
{
    Point next = next_head(game->ai_snake[0], dir);
    DeathReason dummy = DEATH_NONE;
    return can_enter_cell(game, next, false, game->ai_snake, game->ai_length, &dummy);
}

void set_p2_direction(GameState *game, Direction dir)
{
    if (!game->started)
    {
        if (p2_can_start_in_direction(game, dir))
        {
            game->p2_direction = dir;
            game->ai_direction = dir;
        }
        return;
    }

    if (!is_opposite(game->ai_direction, dir))
    {
        game->ai_direction = dir;
        game->p2_direction = dir;
    }
}

// Initialize PvP mode: 35x35 map, two human-controlled snakes, two food items
void init_pvp_game(GameState *game)
{
    srand((unsigned)time(NULL));
    game->map_size = BATTLE_MAP_SIZE;
    clear_map(game);

    // Player 1 snake on the left side
    int mid = BATTLE_MAP_SIZE / 2;  // 17
    game->snake[0] = {mid, 7};
    game->snake[1] = {mid, 6};
    game->snake[2] = {mid + 1, 6};
    game->length = 3;
    game->score = 0;
    game->move_count = 0;
    game->grow_step = GROW_STEP;
    game->direction = UP;
    game->death_reason = DEATH_NONE;
    game->started = false;
    game->paused = false;
    game->game_over = false;
    game->mode = MODE_PVP;
    game->food_eaten = 0;
    game->pending_growth = 0;

    // Player 2 snake on the right side
    game->ai_snake[0] = {mid, BATTLE_MAP_SIZE - 8};
    game->ai_snake[1] = {mid, BATTLE_MAP_SIZE - 7};
    game->ai_snake[2] = {mid + 1, BATTLE_MAP_SIZE - 7};
    game->ai_length = 3;
    game->ai_direction = UP;
    game->p2_direction = UP;
    game->ai_is_alive = true;
    game->ai_score = 0;
    game->ai_food_eaten = 0;
    game->ai_pending_growth = 0;
    game->ai_death_reason = DEATH_NONE;

    put_player_snake_on_map(game);
    put_ai_snake_on_map(game);

    generate_obstacles(game, 30);

    // Place two food items
    place_food(game);
    if (!game->game_over)
        place_food(game);
}

// Move both snakes in PvP mode with collision resolution
void move_pvp_snakes(GameState *game)
{
    if (!game->started || game->paused || game->game_over)
        return;

    Direction p1_dir = game->direction;
    Direction p2_dir = game->ai_direction;

    // Compute next head positions
    Point p1_next = next_head(game->snake[0], p1_dir);
    Point p2_next = next_head(game->ai_snake[0], p2_dir);

    // Check what's at each next cell
    char p1_cell = game->map[p1_next.row][p1_next.col];
    char p2_cell = game->map[p2_next.row][p2_next.col];

    bool p1_will_eat = (p1_cell == 'F');
    bool p2_will_eat = (p2_cell == 'F');
    bool auto_grow = ((game->move_count + 1) % game->grow_step) == 0;
    bool p1_will_grow = p1_will_eat || auto_grow;
    bool p2_will_grow = p2_will_eat || auto_grow;

    bool p1_dies = false;
    bool p2_dies = false;
    DeathReason p1_reason = DEATH_NONE;
    DeathReason p2_reason = DEATH_NONE;
    bool p1_grows_from_kill = false;
    bool p2_grows_from_kill = false;
    int p1_grow_amount = 0;
    int p2_grow_amount = 0;

    // --- Collision Resolution ---

    // 1. Head-on collision
    if (is_same_point(p1_next, p2_next))
    {
        p1_dies = true;
        p1_reason = DEATH_HEAD_ON;
        p2_dies = true;
        p2_reason = DEATH_HEAD_ON;
    }
    else
    {
        // 2. P1 head hits P2 body
        if (p1_cell == 'C')
        {
            if (!p2_will_grow && is_same_point(p1_next, game->ai_snake[game->ai_length - 1]))
            {
                // P1 can pass through P2's tail that's about to move
            }
            else
            {
                p1_dies = true;
                p1_reason = DEATH_ENEMY_BODY;
                p2_grows_from_kill = true;
                p2_grow_amount = game->length;
            }
        }

        // 3. P2 head hits P1 body
        if (p2_cell == 'B')
        {
            if (!p1_will_grow && is_same_point(p2_next, game->snake[game->length - 1]))
            {
                // P2 can pass through P1's tail that's about to move
            }
            else
            {
                p2_dies = true;
                p2_reason = DEATH_ENEMY_BODY;
                p1_grows_from_kill = true;
                p1_grow_amount = game->ai_length;
            }
        }

        // 4. P1 hits wall/obstacle
        if (p1_cell == '#' || p1_cell == 'O')
        {
            p1_dies = true;
            p1_reason = (p1_cell == '#') ? DEATH_WALL : DEATH_OBSTACLE;
        }

        // 5. P2 hits wall/obstacle
        if (p2_cell == '#' || p2_cell == 'O')
        {
            p2_dies = true;
            p2_reason = (p2_cell == '#') ? DEATH_WALL : DEATH_OBSTACLE;
        }

        // 6. P1 self-collision
        if (!p1_dies && p1_cell == 'B')
        {
            if (!p1_will_grow && is_same_point(p1_next, game->snake[game->length - 1]))
            {
                // safe — own tail
            }
            else
            {
                p1_dies = true;
                p1_reason = DEATH_BODY;
            }
        }

        // 7. P2 self-collision
        if (!p2_dies && p2_cell == 'C')
        {
            if (!p2_will_grow && is_same_point(p2_next, game->ai_snake[game->ai_length - 1]))
            {
                // safe — own tail
            }
            else
            {
                p2_dies = true;
                p2_reason = DEATH_BODY;
            }
        }
    }

    // --- Execute Results ---

    // Handle P2 death — game over, P1 wins
    if (p2_dies)
    {
        clear_snake_from_map(game, game->ai_snake, game->ai_length, 'A', 'C');
        game->ai_is_alive = false;
        game->ai_death_reason = p2_reason;
        game->game_over = true;

        if (p1_grows_from_kill && p1_grow_amount > 0)
        {
            add_pending_growth(&game->pending_growth, p1_grow_amount);
        }

        // Apply P1's move one last time
        if (!p1_dies)
        {
            apply_snake_move(game, game->snake, &game->length,
                             p1_dir, &game->score, &game->food_eaten,
                             &game->pending_growth,
                             'H', 'B');
        }

        game->move_count++;
        return;
    }

    // Handle P1 death — game over, P2 wins
    if (p1_dies)
    {
        game->death_reason = p1_reason;
        game->game_over = true;

        if (p2_grows_from_kill && p2_grow_amount > 0)
        {
            add_pending_growth(&game->ai_pending_growth, p2_grow_amount);
        }

        // Apply P2's move one last time
        if (!p2_dies && game->ai_is_alive)
        {
            apply_snake_move(game, game->ai_snake, &game->ai_length,
                             p2_dir, &game->ai_score, &game->ai_food_eaten,
                             &game->ai_pending_growth,
                             'A', 'C');
        }

        game->move_count++;
        return;
    }

    // Apply P1 move
    {
        bool ate = apply_snake_move(game, game->snake, &game->length,
                                     p1_dir, &game->score, &game->food_eaten,
                                     &game->pending_growth,
                                     'H', 'B');
        if (ate)
            place_food(game);
    }

    // Apply P2 move
    {
        bool ate = apply_snake_move(game, game->ai_snake, &game->ai_length,
                                     p2_dir, &game->ai_score, &game->ai_food_eaten,
                                     &game->ai_pending_growth,
                                     'A', 'C');
        if (ate)
            place_food(game);
    }

    // Ensure at least 2 food items on the map
    {
        int food_count = 0;
        for (int r = 0; r < game->map_size; r++)
            for (int c = 0; c < game->map_size; c++)
                if (game->map[r][c] == 'F')
                    food_count++;
        while (food_count < 2 && !game->game_over)
        {
            place_food(game);
            food_count++;
        }
    }

    game->move_count++;
}

// ──── High Score Persistence ──────────────────────────────

void load_high_score(GameState *game)
{
    game->high_score = 0;
    FILE *f = fopen("snake_highscore.dat", "r");
    if (f)
    {
        fscanf(f, "%d", &game->high_score);
        fclose(f);
    }
}

void save_high_score(const GameState *game)
{
    FILE *f = fopen("snake_highscore.dat", "w");
    if (f)
    {
        fprintf(f, "%d", game->high_score);
        fclose(f);
    }
}

// Window resize is handled in main.cpp using raylib's SetWindowSize()
