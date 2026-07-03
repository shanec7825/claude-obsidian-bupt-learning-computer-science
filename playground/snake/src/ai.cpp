#include "ai.h"
#include <string.h>
#include <stdlib.h>

// Direction arrays matching game Direction enum order: UP, LEFT, DOWN, RIGHT
static const int AI_DR[4] = {-1, 0, 1, 0};
static const int AI_DC[4] = {0, -1, 0, 1};
static const Direction AI_DIR[4] = {UP, LEFT, DOWN, RIGHT};

// BFS distance array
static int dist[MAX_MAP_SIZE][MAX_MAP_SIZE];

// BFS queue
typedef struct node
{
    int r, c;
} Node;
static Node q[MAX_SNAKE_LEN];

// ---------------------------------------------------------------------------
// Shared helpers
// ---------------------------------------------------------------------------

// Check if a cell is passable for the AI (empty or food only)
static bool ai_can_go(const GameState *game, int r, int c)
{
    if (r <= 0 || r >= game->map_size - 1 || c <= 0 || c >= game->map_size - 1)
        return false;
    char ch = game->map[r][c];
    return (ch == '.' || ch == 'F');
}

// BFS from food to compute distance grid
static void ai_bfs_from_food(const GameState *game, int fr, int fc)
{
    for (int r = 0; r < game->map_size; r++)
        for (int c = 0; c < game->map_size; c++)
            dist[r][c] = -1;

    int front = 0, back = 0;
    q[back++] = {fr, fc};
    dist[fr][fc] = 0;

    while (front < back)
    {
        Node cur = q[front++];
        for (int d = 0; d < 4; d++)
        {
            int nr = cur.r + AI_DR[d];
            int nc = cur.c + AI_DC[d];
            if (!ai_can_go(game, nr, nc))
                continue;
            if (dist[nr][nc] != -1)
                continue;
            dist[nr][nc] = dist[cur.r][cur.c] + 1;
            q[back++] = {nr, nc};
        }
    }
}

// BFS to count free cells reachable from a starting point
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
        cnt++;
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
    return cnt;
}

// BFS path finding on a simulated grid
static bool ai_bfs_path(char sim[MAX_MAP_SIZE][MAX_MAP_SIZE], int map_size,
                         int sr, int sc, int tr, int tc, int prev_dir[MAX_MAP_SIZE][MAX_MAP_SIZE])
{
    int vis[MAX_MAP_SIZE][MAX_MAP_SIZE];
    memset(vis, 0, sizeof(vis));

    Point queue[MAX_SNAKE_LEN];
    int front = 0, back = 0;
    queue[back++] = {sr, sc};
    vis[sr][sc] = 1;

    while (front < back)
    {
        Point cur = queue[front++];
        if (cur.row == tr && cur.col == tc)
            return true;

        for (int d = 0; d < 4; d++)
        {
            int nr = cur.row + AI_DR[d];
            int nc = cur.col + AI_DC[d];
            if (nr <= 0 || nr >= map_size - 1 || nc <= 0 || nc >= map_size - 1)
                continue;
            if (vis[nr][nc])
                continue;
            if (nr == tr && nc == tc)
            {
                vis[nr][nc] = 1;
                prev_dir[nr][nc] = d;
                queue[back++] = {nr, nc};
                continue;
            }
            char ch = sim[nr][nc];
            if (ch != '.' && ch != 'F')
                continue;
            vis[nr][nc] = 1;
            prev_dir[nr][nc] = d;
            queue[back++] = {nr, nc};
        }
    }
    return false;
}

// Check if AI can still reach its own tail after moving in direction d
static bool ai_can_reach_tail_after_move(const GameState *game, int d)
{
    const Point *body = game->ai_snake;
    int len = game->ai_length;

    Point hd = body[0];
    Point tail = body[len - 1];

    int nr = hd.row + AI_DR[d];
    int nc = hd.col + AI_DC[d];

    char sim[MAX_MAP_SIZE][MAX_MAP_SIZE];
    for (int r = 0; r < game->map_size; r++)
        for (int c = 0; c < game->map_size; c++)
            sim[r][c] = game->map[r][c];

    bool eat = (sim[nr][nc] == 'F');
    bool auto_grow = ((game->move_count + 1) % game->grow_step) == 0;
    bool will_grow = eat || auto_grow;

    if (!will_grow)
        sim[tail.row][tail.col] = '.';

    sim[hd.row][hd.col] = 'C';
    sim[nr][nc] = 'A';

    int prev_dir[MAX_MAP_SIZE][MAX_MAP_SIZE];
    memset(prev_dir, -1, sizeof(prev_dir));

    Point new_tail;
    if (will_grow)
        new_tail = tail;
    else
        new_tail = body[len - 2];

    return ai_bfs_path(sim, game->map_size, nr, nc, new_tail.row, new_tail.col, prev_dir);
}

// Collect all valid directions from the AI's current head position
static int collect_valid_dirs(const GameState *game, int *dirs_out)
{
    Point hd = game->ai_snake[0];
    int count = 0;
    for (int d = 0; d < 4; d++)
    {
        int nr = hd.row + AI_DR[d];
        int nc = hd.col + AI_DC[d];
        if (ai_can_go(game, nr, nc))
            dirs_out[count++] = d;
    }
    return count;
}

static Direction ai_dir_easy(const GameState *game)
{
    Point hd = game->ai_snake[0];

    // 30% chance: pick a random valid direction
    if (rand() % 100 < 30)
    {
        int valid[4], cnt = collect_valid_dirs(game, valid);
        if (cnt > 0)
            return AI_DIR[valid[rand() % cnt]];
    }

    // Find food and go toward it by Manhattan distance
    int fr = -1, fc = -1;
    for (int r = 0; r < game->map_size; r++)
        for (int c = 0; c < game->map_size; c++)
            if (game->map[r][c] == 'F')
            {
                fr = r;
                fc = c;
                goto food_found_easy;
            }
food_found_easy:

    if (fr != -1)
    {
        int best_dir = -1, best_dist = 1000000;
        for (int d = 0; d < 4; d++)
        {
            int nr = hd.row + AI_DR[d];
            int nc = hd.col + AI_DC[d];
            if (!ai_can_go(game, nr, nc))
                continue;
            int mdist = abs(nr - fr) + abs(nc - fc);
            if (mdist < best_dist)
            {
                best_dist = mdist;
                best_dir = d;
            }
        }
        if (best_dir != -1)
            return AI_DIR[best_dir];
    }

    // Fallback: most free space
    int best_dir = -1, best_space = -1;
    for (int d = 0; d < 4; d++)
    {
        int nr = hd.row + AI_DR[d];
        int nc = hd.col + AI_DC[d];
        if (!ai_can_go(game, nr, nc))
            continue;
        int space = ai_free_count(game, nr, nc);
        if (space > best_space)
        {
            best_space = space;
            best_dir = d;
        }
    }
    if (best_dir != -1)
        return AI_DIR[best_dir];

    // Last resort
    for (int d = 0; d < 4; d++)
    {
        int nr = hd.row + AI_DR[d];
        int nc = hd.col + AI_DC[d];
        if (ai_can_go(game, nr, nc))
            return AI_DIR[d];
    }
    return game->ai_direction;
}

static Direction ai_dir_normal(const GameState *game)
{
    Point hd = game->ai_snake[0];

    int fr = -1, fc = -1;
    for (int r = 0; r < game->map_size; r++)
        for (int c = 0; c < game->map_size; c++)
            if (game->map[r][c] == 'F')
            {
                fr = r;
                fc = c;
                goto food_found_normal;
            }
food_found_normal:

    int best_dir = -1;

    if (fr != -1)
    {
        int best_dist = 1000000;
        ai_bfs_from_food(game, fr, fc);

        for (int d = 0; d < 4; d++)
        {
            int nr = hd.row + AI_DR[d];
            int nc = hd.col + AI_DC[d];
            if (!ai_can_go(game, nr, nc))
                continue;
            if (dist[nr][nc] == -1)
                continue;

            // Normal difficulty: no tail-reachability safety check at any length

            if (dist[nr][nc] < best_dist)
            {
                best_dist = dist[nr][nc];
                best_dir = d;
            }
        }

        if (best_dir != -1)
        {
            // 15% chance: override with a random valid move
            if (rand() % 100 < 15)
            {
                int valid[4], cnt = collect_valid_dirs(game, valid);
                if (cnt > 0)
                    return AI_DIR[valid[rand() % cnt]];
            }
            return AI_DIR[best_dir];
        }
    }

    // Fallback: most free space
    int best_space = -1;
    for (int d = 0; d < 4; d++)
    {
        int nr = hd.row + AI_DR[d];
        int nc = hd.col + AI_DC[d];
        if (!ai_can_go(game, nr, nc))
            continue;
        int space = ai_free_count(game, nr, nc);
        if (space > best_space)
        {
            best_space = space;
            best_dir = d;
        }
    }
    if (best_dir != -1)
        return AI_DIR[best_dir];

    // Last resort
    for (int d = 0; d < 4; d++)
    {
        int nr = hd.row + AI_DR[d];
        int nc = hd.col + AI_DC[d];
        if (ai_can_go(game, nr, nc))
            return AI_DIR[d];
    }
    return game->ai_direction;
}

static Direction ai_dir_hard(const GameState *game)
{
    const Point *body = game->ai_snake;
    Point hd = body[0];

    int fr = -1, fc = -1;
    for (int r = 0; r < game->map_size; r++)
        for (int c = 0; c < game->map_size; c++)
            if (game->map[r][c] == 'F')
            {
                fr = r;
                fc = c;
                goto food_found_hard;
            }
food_found_hard:

    int best_dir = -1;

    if (fr != -1)
    {
        int best_dist = 1000000;
        ai_bfs_from_food(game, fr, fc);

        for (int d = 0; d < 4; d++)
        {
            int nr = hd.row + AI_DR[d];
            int nc = hd.col + AI_DC[d];
            if (!ai_can_go(game, nr, nc))
                continue;
            if (dist[nr][nc] == -1)
                continue;

            // Early game: greedy
            if (game->ai_length < 105)
            {
                if (dist[nr][nc] < best_dist)
                {
                    best_dist = dist[nr][nc];
                    best_dir = d;
                }
            }
            else
            {
                // Late game: safety check
                if (!ai_can_reach_tail_after_move(game, d))
                    continue;
                if (dist[nr][nc] < best_dist)
                {
                    best_dist = dist[nr][nc];
                    best_dir = d;
                }
            }
        }

        if (best_dir != -1)
            return AI_DIR[best_dir];
    }

    // Fallback: most free space
    int best_space = -1;
    for (int d = 0; d < 4; d++)
    {
        int nr = hd.row + AI_DR[d];
        int nc = hd.col + AI_DC[d];
        if (!ai_can_go(game, nr, nc))
            continue;
        int space = ai_free_count(game, nr, nc);
        if (space > best_space)
        {
            best_space = space;
            best_dir = d;
        }
    }
    if (best_dir != -1)
        return AI_DIR[best_dir];

    // Last resort
    for (int d = 0; d < 4; d++)
    {
        int nr = hd.row + AI_DR[d];
        int nc = hd.col + AI_DC[d];
        if (ai_can_go(game, nr, nc))
            return AI_DIR[d];
    }
    return game->ai_direction;
}

// ---------------------------------------------------------------------------
// Dispatcher
// ---------------------------------------------------------------------------
Direction ai_choose_direction(const GameState *game, AIDifficulty difficulty)
{
    switch (difficulty)
    {
    case AI_EASY:
        return ai_dir_easy(game);
    case AI_NORMAL:
        return ai_dir_normal(game);
    case AI_HARD:
    default:
        return ai_dir_hard(game);
    }
}
