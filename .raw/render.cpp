#include "render.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

// ==============================================================
// Color palette (refined)
// ==============================================================
const Color COL_WALL        = { 35,  38,  48,  255};
const Color COL_OBSTACLE    = {128, 134, 148, 255};
const Color COL_HEAD         = { 36, 168,  94,  255};
const Color COL_BODY         = { 73, 204, 118, 255};
const Color COL_BODY_TAIL    = {110, 215, 145, 255};
const Color COL_HEAD_ALT     = { 52, 118, 230, 255};  // P2 / AI head
const Color COL_BODY_ALT     = { 95, 160, 245, 255};  // P2 / AI body
const Color COL_BODY_ALT_TAIL= {140, 185, 250, 255};
const Color COL_FOOD         = {238,  66,  58,  255};
const Color COL_FOOD_INNER   = {255, 110, 100, 255};
const Color COL_EMPTY_A      = {232, 237, 244, 255};  // Checkerboard A
const Color COL_EMPTY_B      = {242, 246, 250, 255};  // Checkerboard B
const Color COL_GRID_LINE    = {210, 218, 228, 255};
const Color COL_BG            = {225, 232, 240, 255};
const Color COL_TEXT          = { 28,  32,  40,  255};
const Color COL_TEXT_DIM      = { 75,  82,  95,  255};
const Color COL_TEXT_TITLE    = { 20,  25,  35,  255};
const Color COL_ACCENT_GREEN  = { 24, 140,  82,  255};
const Color COL_ACCENT_BLUE   = { 42, 108, 218, 255};
const Color COL_ACCENT_RED    = {200,  52,  45,  255};
const Color COL_ACCENT_ORANGE = {220, 130,  40,  255};

// ──── Resources ────────────────────────────────────────────
static bool resources_loaded = false;
Font g_game_font;

void init_render_resources()
{
    if (resources_loaded) return;

    // 加载支持中文的字体（Windows 系统黑体 / 微软雅黑）
    // 优先使用 simhei.ttf (TTF 格式兼容性最好)，回退 msyh.ttc
    const char *font_paths[] = {
        "C:/Windows/Fonts/simhei.ttf",
        "C:/Windows/Fonts/msyh.ttc",
    };

    // ASCII + 游戏中实际用到的汉字 + 全角标点（从源码提取）
    const char *chinese_chars =
        "一二三中了亡人体你使停典出分列到制加动区单即双吃向咬回因困图"
        "地基墙增失头如始字存完家宽对就己已并度建开式微态意戏成战手择"
        "持按撞支数整文新方无普暂未本机束构果格模步死活测游满点物状玩"
        "用的知码碍碰种移空等简系级经结绘统绪绿胜自色获菜蓝蛇表认败贪"
        "身软载输返退选通速重量键长间限隔障难雅食黑默"
        "最高"
        "：（）→─，！";

    int codepoints[256];
    int cp_count = 0;
    // ASCII 可打印字符
    for (int i = 32; i < 127; i++)
        codepoints[cp_count++] = i;
    // 游戏用到的中文字符（UTF-8 → 码点）
    for (const char *p = chinese_chars; *p; )
    {
        unsigned int cp = 0;
        unsigned char c = (unsigned char)*p;
        if (c < 0x80) { cp = c; p++; }
        else if (c < 0xE0) { cp = ((c & 0x1F) << 6) | (p[1] & 0x3F); p += 2; }
        else { cp = ((c & 0x0F) << 12) | ((p[1] & 0x3F) << 6) | (p[2] & 0x3F); p += 3; }
        if (cp_count < 256)
            codepoints[cp_count++] = (int)cp;
    }

    g_game_font.glyphCount = 0;
    for (int i = 0; i < 2; i++)
    {
        if (FileExists(font_paths[i]))
        {
            g_game_font = LoadFontEx(font_paths[i], 48, codepoints, cp_count);
            if (g_game_font.glyphCount > 0)
                break;
        }
    }

    // 全部加载失败则回退默认字体
    if (g_game_font.glyphCount == 0)
    {
        g_game_font = GetFontDefault();
    }

    resources_loaded = true;
}

void unload_render_resources()
{
    if (!resources_loaded) return;
    if (g_game_font.glyphCount > 0 && g_game_font.texture.id != GetFontDefault().texture.id)
        UnloadFont(g_game_font);
    resources_loaded = false;
}

// ──── Helpers ──────────────────────────────────────────────

static int panel_left(const GameState *game)
{
    return BOARD_LEFT + game->map_size * CELL_SIZE + 20;
}

static Color cell_render_color(const GameState *game, int r, int c, char cell)
{
    switch (cell)
    {
    case '#': return COL_WALL;
    case 'O': return COL_OBSTACLE;
    case 'H': return COL_HEAD;
    case 'B': return COL_BODY;
    case 'A': return COL_HEAD_ALT;
    case 'C': return COL_BODY_ALT;
    case 'F': return COL_FOOD;
    default:  // checkerboard empty cells
        return ((r + c) % 2 == 0) ? COL_EMPTY_A : COL_EMPTY_B;
    }
}

// Get the index of a cell in a snake body array. Returns -1 if not found.
static int snake_body_index(const GameState *game, int r, int c, bool is_player)
{
    const Point *body = is_player ? game->snake : game->ai_snake;
    int len = is_player ? game->length : game->ai_length;
    for (int i = 0; i < len; i++)
        if (body[i].row == r && body[i].col == c)
            return i;
    return -1;
}

// Draw a snake segment with rounded corners. Index 0 = head.
static void draw_snake_segment(int row, int column, Color base_color, int index, int total_len, bool is_head)
{
    int left = BOARD_LEFT + column * CELL_SIZE;
    int top  = BOARD_TOP  + row * CELL_SIZE;
    float size = (float)CELL_SIZE;

    // Body segments shrink slightly toward the tail
    float shrink = is_head ? 0.0f : 2.0f + 2.0f * (float)index / (float)total_len;
    float x = left + shrink / 2.0f;
    float y = top  + shrink / 2.0f;
    float w = size - shrink;
    float h = size - shrink;

    // Darken color toward the tail for depth
    float darken = is_head ? 0.0f : 0.25f * (float)index / (float)total_len;
    Color seg_color = {
        (unsigned char)((float)base_color.r * (1.0f - darken)),
        (unsigned char)((float)base_color.g * (1.0f - darken)),
        (unsigned char)((float)base_color.b * (1.0f - darken)),
        255
    };

    DrawRectangleRounded({x, y, w, h}, 0.28f, 8, seg_color);

    // Head: draw eyes
    if (is_head)
    {
        int e1x = (int)(x + size * 0.35f);
        int e2x = (int)(x + size * 0.65f);
        int ey  = (int)(y + size * 0.35f);
        DrawCircle(e1x, ey, 3.0f, WHITE);
        DrawCircle(e1x, ey, 1.6f, BLACK);
        DrawCircle(e2x, ey, 3.0f, WHITE);
        DrawCircle(e2x, ey, 1.6f, BLACK);
    }
}

static void draw_cell(const GameState *game, int r, int c, char cell)
{
    int left = BOARD_LEFT + c * CELL_SIZE;
    int top  = BOARD_TOP  + r * CELL_SIZE;

    // Snake body segments: draw specially
    if (cell == 'H' || cell == 'B')
    {
        bool is_head = (cell == 'H');
        int idx = snake_body_index(game, r, c, true);
        if (idx >= 0)
        {
            Color base = is_head ? COL_HEAD : COL_BODY;
            draw_snake_segment(r, c, base, idx, game->length, is_head);
            return;
        }
    }
    if (cell == 'A' || cell == 'C')
    {
        bool is_head = (cell == 'A');
        int idx = snake_body_index(game, r, c, false);
        if (idx >= 0)
        {
            Color base = is_head ? COL_HEAD_ALT : COL_BODY_ALT;
            draw_snake_segment(r, c, base, idx, game->ai_length, is_head);
            return;
        }
    }

    // Regular cell rendering
    Color fill = cell_render_color(game, r, c, cell);
    DrawRectangle(left, top, CELL_SIZE, CELL_SIZE, fill);

    // Food: animated pulsing circle
    if (cell == 'F')
    {
        int cx = left + CELL_SIZE / 2;
        int cy = top  + CELL_SIZE / 2;
        float base_r = (float)CELL_SIZE / 2.0f - 4.0f;

        // Pulse animation
        float pulse = 1.0f + 0.12f * sinf((float)GetTime() * 6.0f);
        float r = base_r * pulse;

        // Outer glow
        DrawCircle(cx, cy, r + 2.0f, ColorAlpha(COL_FOOD, 0.35f));
        // Main circle
        DrawCircle(cx, cy, r, COL_FOOD);
        // Inner highlight
        DrawCircle(cx - 2, cy - 2, r * 0.4f, COL_FOOD_INNER);
    }

    // Subtle grid line
    DrawRectangleLines(left, top, CELL_SIZE, CELL_SIZE, COL_GRID_LINE);
}

static const char *death_reason_text(DeathReason r)
{
    switch (r)
    {
    case DEATH_WALL:       return "死因：撞墙";
    case DEATH_OBSTACLE:   return "死因：撞到障碍物";
    case DEATH_BODY:       return "死因：咬到自己身体";
    case DEATH_ENEMY_BODY: return "死因：撞到对手身体";
    case DEATH_HEAD_ON:    return "死因：头对头碰撞";
    case DEATH_FULL_MAP:   return "死因：地图已满";
    default:               return "死因：未知";
    }
}

// ──── Font-aware text wrappers ────────────────────────────

// 使用中文字体绘制文本
static void DrawTextF(const char *text, int x, int y, int fontSize, Color color)
{
    Vector2 pos = { (float)x, (float)y };
    DrawTextEx(g_game_font, text, pos, (float)fontSize, 0, color);
}

// 使用中文字体测量文本宽度
static int MeasureTextF(const char *text, int fontSize)
{
    return (int)MeasureTextEx(g_game_font, text, (float)fontSize, 0).x;
}

static void draw_stat_line(int x, int y, int font_size,
                           Color label_col, const char *label,
                           Color val_col,   const char *value)
{
    char line[128];
    snprintf(line, sizeof(line), "%s %s", label, value);
    DrawTextF(line, x, y, font_size, label_col);
    (void)val_col;
}

// ──── Board ────────────────────────────────────────────────

void draw_board(const GameState *game)
{
    int ms = game->map_size;
    for (int r = 0; r < ms; r++)
        for (int c = 0; c < ms; c++)
            draw_cell(game, r, c, game->map[r][c]);

    // Outer border with slight shadow
    Rectangle border = {
        (float)BOARD_LEFT,
        (float)BOARD_TOP,
        (float)(ms * CELL_SIZE),
        (float)(ms * CELL_SIZE)
    };
    DrawRectangleLinesEx({border.x + 1, border.y + 1,
                          border.width + 1, border.height + 1},
                         1.0f, ColorAlpha(BLACK, 0.15f));
    DrawRectangleLinesEx(border, 2.5f, {40, 48, 56, 255});
}

// ──── Panel ─────────────────────────────────────────────────

void draw_panel(const GameState *game)
{
    char val[64];
    int px = panel_left(game);

    if (game->mode == MODE_PVP)
    {
        DrawTextF("双人对战", px, 48, 30, COL_TEXT_TITLE);

        snprintf(val, sizeof(val), "-- 玩家1 (绿色) --");
        DrawTextF(val, px, 90, 20, COL_ACCENT_GREEN);

        snprintf(val, sizeof(val), "%d", game->score);
        draw_stat_line(px, 120, 20, COL_TEXT, "分数：", COL_TEXT, val);
        snprintf(val, sizeof(val), "%d", game->length);
        draw_stat_line(px, 150, 20, COL_TEXT, "长度：", COL_TEXT, val);
        snprintf(val, sizeof(val), "%d", game->food_eaten);
        draw_stat_line(px, 180, 20, COL_TEXT, "食物：", COL_TEXT, val);

        snprintf(val, sizeof(val), "-- 玩家2 (蓝色) --");
        DrawTextF(val, px, 230, 20, COL_ACCENT_BLUE);

        snprintf(val, sizeof(val), "%d", game->ai_score);
        draw_stat_line(px, 260, 20, COL_TEXT, "分数：", COL_TEXT, val);
        snprintf(val, sizeof(val), "%d", game->ai_length);
        draw_stat_line(px, 290, 20, COL_TEXT, "长度：", COL_TEXT, val);
        snprintf(val, sizeof(val), "%d", game->ai_food_eaten);
        draw_stat_line(px, 320, 20, COL_TEXT, "食物：", COL_TEXT, val);

        DrawTextF(game->ai_is_alive ? "P2 状态：存活" : "P2 状态：死亡",
                 px, 350, 20, game->ai_is_alive ? COL_ACCENT_GREEN : COL_ACCENT_RED);

        snprintf(val, sizeof(val), "%d", game->move_count);
        draw_stat_line(px, 390, 20, COL_TEXT, "步数：", COL_TEXT, val);
        snprintf(val, sizeof(val), "%d", game->grow_step);
        draw_stat_line(px, 420, 20, COL_TEXT, "成长间隔：", COL_TEXT, val);

        if (!game->started)
            DrawTextF("状态：就绪", px, 460, 20, COL_ACCENT_BLUE);
        else if (game->game_over)
        {
            DrawTextF(game->ai_is_alive ? "状态：P2 获胜！" : "状态：P1 获胜！",
                     px, 460, 20, COL_ACCENT_RED);
            DrawTextF(death_reason_text(
                game->ai_is_alive ? game->death_reason : game->ai_death_reason),
                px, 490, 20, COL_ACCENT_RED);
        }
        else if (game->paused)
            DrawTextF("状态：暂停", px, 460, 20, COL_ACCENT_ORANGE);
        else
            DrawTextF("状态：游戏中", px, 460, 20, COL_ACCENT_GREEN);

        DrawTextF("P1：W A S D   |   P2：方向键", px, 540, 16, COL_TEXT_DIM);
        DrawTextF("空格：暂停   R：重新开始",        px, 570, 16, COL_TEXT_DIM);
        DrawTextF("Shift/F：加速",                  px, 600, 16, COL_TEXT_DIM);
        DrawTextF("Esc：返回菜单",                   px, 630, 16, COL_TEXT_DIM);
        DrawTextF("模式：双人对战 (1P vs 2P)",        px, 690, 16, COL_TEXT_DIM);
    }
    else if (game->mode == MODE_BATTLE)
    {
        DrawTextF("人机对战", px, 48, 30, COL_TEXT_TITLE);

        DrawTextF("-- 玩家 (绿色) --", px, 90, 20, COL_ACCENT_GREEN);

        snprintf(val, sizeof(val), "%d", game->score);
        draw_stat_line(px, 120, 20, COL_TEXT, "分数：", COL_TEXT, val);
        snprintf(val, sizeof(val), "%d", game->length);
        draw_stat_line(px, 150, 20, COL_TEXT, "长度：", COL_TEXT, val);
        snprintf(val, sizeof(val), "%d", game->food_eaten);
        draw_stat_line(px, 180, 20, COL_TEXT, "食物：", COL_TEXT, val);

        DrawTextF("-- AI (蓝色) --", px, 230, 20, COL_ACCENT_BLUE);

        snprintf(val, sizeof(val), "%d", game->ai_score);
        draw_stat_line(px, 260, 20, COL_TEXT, "分数：", COL_TEXT, val);
        snprintf(val, sizeof(val), "%d", game->ai_length);
        draw_stat_line(px, 290, 20, COL_TEXT, "长度：", COL_TEXT, val);
        snprintf(val, sizeof(val), "%d", game->ai_food_eaten);
        draw_stat_line(px, 320, 20, COL_TEXT, "食物：", COL_TEXT, val);

        DrawTextF(game->ai_is_alive ? "AI 状态：存活" : "AI 状态：死亡",
                 px, 350, 20, game->ai_is_alive ? COL_ACCENT_GREEN : COL_ACCENT_RED);

        snprintf(val, sizeof(val), "%d", game->move_count);
        draw_stat_line(px, 390, 20, COL_TEXT, "步数：", COL_TEXT, val);
        snprintf(val, sizeof(val), "%d", game->grow_step);
        draw_stat_line(px, 420, 20, COL_TEXT, "成长间隔：", COL_TEXT, val);

        if (!game->started)
            DrawTextF("状态：就绪", px, 460, 20, COL_ACCENT_BLUE);
        else if (game->game_over)
        {
            DrawTextF("状态：你输了", px, 460, 20, COL_ACCENT_RED);
            DrawTextF(death_reason_text(game->death_reason), px, 490, 20, COL_ACCENT_RED);
        }
        else if (game->paused)
            DrawTextF("状态：暂停", px, 460, 20, COL_ACCENT_ORANGE);
        else
            DrawTextF("状态：游戏中", px, 460, 20, COL_ACCENT_GREEN);

        DrawTextF("移动：W A S D / 方向键", px, 540, 16, COL_TEXT_DIM);
        DrawTextF("空格：暂停   R：重新开始", px, 570, 16, COL_TEXT_DIM);
        DrawTextF("Shift/F：加速",            px, 600, 16, COL_TEXT_DIM);
        DrawTextF("Esc：返回菜单",             px, 630, 16, COL_TEXT_DIM);

        const char *diff_text = "普通";
        if (game->ai_difficulty == AI_EASY)       diff_text = "简单";
        else if (game->ai_difficulty == AI_HARD)  diff_text = "困难";
        char diff_line[64];
        snprintf(diff_line, sizeof(diff_line), "AI 难度：%s", diff_text);
        DrawTextF(diff_line, px, 660, 16, COL_TEXT_DIM);

        DrawTextF("模式：人机对战 (玩家 vs AI)", px, 690, 16, COL_TEXT_DIM);
    }
    else
    {
        DrawTextF("贪吃蛇", px, 48, 30, COL_TEXT_TITLE);

        snprintf(val, sizeof(val), "%d", game->score);
        draw_stat_line(px, 108, 20, COL_TEXT, "分数：", COL_TEXT, val);
        snprintf(val, sizeof(val), "%d", game->high_score);
        draw_stat_line(px, 138, 20, COL_TEXT, "最高分：", COL_ACCENT_ORANGE, val);
        snprintf(val, sizeof(val), "%d", game->length);
        draw_stat_line(px, 168, 20, COL_TEXT, "长度：", COL_TEXT, val);
        snprintf(val, sizeof(val), "%d", game->move_count);
        draw_stat_line(px, 198, 20, COL_TEXT, "步数：", COL_TEXT, val);
        snprintf(val, sizeof(val), "%d", game->grow_step);
        draw_stat_line(px, 228, 20, COL_TEXT, "成长间隔：", COL_TEXT, val);

        if (!game->started)
            DrawTextF("状态：就绪", px, 278, 20, COL_ACCENT_BLUE);
        else if (game->game_over)
        {
            DrawTextF("状态：游戏结束", px, 278, 20, COL_ACCENT_RED);
            DrawTextF(death_reason_text(game->death_reason), px, 308, 20, COL_ACCENT_RED);
        }
        else if (game->paused)
            DrawTextF("状态：暂停", px, 278, 20, COL_ACCENT_ORANGE);
        else
            DrawTextF("状态：游戏中", px, 278, 20, COL_ACCENT_GREEN);

        DrawTextF("移动：W A S D / 方向键", px, 360, 16, COL_TEXT_DIM);
        DrawTextF("空格：暂停   R：重新开始", px, 390, 16, COL_TEXT_DIM);
        DrawTextF("Shift/F：加速",             px, 420, 16, COL_TEXT_DIM);
        DrawTextF("Esc：返回菜单",              px, 450, 16, COL_TEXT_DIM);
        DrawTextF("模式：经典",                 px, 480, 16, COL_TEXT_DIM);
    }
}

// ──── Game Over ─────────────────────────────────────────────

void draw_game_over(const GameState *game)
{
    int cx = BOARD_LEFT + game->map_size * CELL_SIZE / 2;
    int cy = BOARD_TOP  + game->map_size * CELL_SIZE / 2;

    // Dim overlay
    DrawRectangle(BOARD_LEFT, BOARD_TOP,
                  game->map_size * CELL_SIZE,
                  game->map_size * CELL_SIZE,
                  ColorAlpha(BLACK, 0.55f));

    // Title card
    float card_x = (float)(cx - 170);
    float card_y = (float)(cy - 100);
    float card_w = 340.0f;
    float card_h = 170.0f;
    DrawRectangleRounded({card_x, card_y, card_w, card_h}, 0.15f, 8,
                         ColorAlpha({15, 20, 30, 255}, 0.85f));
    DrawRectangleRoundedLines({card_x, card_y, card_w, card_h}, 0.15f, 8,
                              ColorAlpha(COL_ACCENT_RED, 0.6f));

    const char *title;
    if (game->mode == MODE_PVP)
        title = game->ai_is_alive ? "玩家1 输了" : "玩家2 输了";
    else if (game->mode == MODE_BATTLE)
        title = "你输了";
    else
        title = "游戏结束";

    int tw = MeasureTextF(title, 38);
    DrawTextF(title, cx - tw / 2, cy - 75, 38, RAYWHITE);

    char buf[64];
    snprintf(buf, sizeof(buf), "分数：%d", game->score);
    int bw2 = MeasureTextF(buf, 20);
    DrawTextF(buf, cx - bw2 / 2, cy - 25, 20, ColorAlpha(RAYWHITE, 0.85f));

    const char *retry = "按 R 重新开始";
    int rw = MeasureTextF(retry, 20);
    DrawTextF(retry, cx - rw / 2, cy + 5, 20, ColorAlpha(RAYWHITE, 0.75f));

    const char *esc = "按 Esc 返回菜单";
    int ew = MeasureTextF(esc, 20);
    DrawTextF(esc, cx - ew / 2, cy + 33, 20, ColorAlpha(RAYWHITE, 0.75f));
}

// ──── Menus ─────────────────────────────────────────────────

void draw_menu()
{
    ClearBackground(COL_BG);

    int cx = MENU_WINDOW_WIDTH / 2;

    // Decorative header bar
    DrawRectangle(0, 0, MENU_WINDOW_WIDTH, 180,
                  ColorAlpha(COL_ACCENT_GREEN, 0.08f));

    const char *title = "贪吃蛇游戏";
    int tw = MeasureTextF(title, 48);
    DrawTextF(title, cx - tw / 2, 70, 48, COL_TEXT_TITLE);

    const char *subtitle = "选择游戏模式";
    int sw = MeasureTextF(subtitle, 22);
    DrawTextF(subtitle, cx - sw / 2, 130, 22, COL_TEXT_DIM);

    const char *items[] = {
        "1   经典模式",
        "2   双人对战",
        "3   人机对战"
    };

    for (int i = 0; i < 3; i++)
    {
        int iy = 228 + i * 62;
        // Selection indicator bar
        DrawRectangleRounded(
            {(float)(cx - 190), (float)(iy - 8), 380.0f, 52.0f},
            0.2f, 8, ColorAlpha(COL_ACCENT_GREEN, 0.06f));

        tw = MeasureTextF(items[i], 30);
        DrawTextF(items[i], cx - tw / 2, iy, 30, COL_TEXT);
    }

    const char *desc1 = "经典模式：无限成长";
    const char *desc2 = "双人对战：P1=WASD  P2=方向键 碰头即死";
    const char *desc3 = "人机对战：AI 对手  三种难度  碰头即死并增长";
    DrawTextF(desc1, cx - MeasureTextF(desc1, 17) / 2, 440, 17,
             {105, 112, 125, 255});
    DrawTextF(desc2, cx - MeasureTextF(desc2, 17) / 2, 465, 17,
             {105, 112, 125, 255});
    DrawTextF(desc3, cx - MeasureTextF(desc3, 17) / 2, 490, 17,
             {105, 112, 125, 255});

    const char *esc = "Esc - 退出游戏";
    DrawTextF(esc, cx - MeasureTextF(esc, 20) / 2, 570, 20, COL_TEXT_DIM);
}

void draw_difficulty_menu()
{
    ClearBackground(COL_BG);

    int cx = MENU_WINDOW_WIDTH / 2;

    DrawRectangle(0, 0, MENU_WINDOW_WIDTH, 180,
                  ColorAlpha(COL_ACCENT_RED, 0.06f));

    const char *title = "人机对战 - 难度选择";
    int tw = MeasureTextF(title, 42);
    DrawTextF(title, cx - tw / 2, 70, 42, COL_TEXT_TITLE);

    const char *subtitle = "选择 AI 难度等级";
    int sw = MeasureTextF(subtitle, 22);
    DrawTextF(subtitle, cx - sw / 2, 130, 22, COL_TEXT_DIM);

    struct { const char *label; Color col; } diffs[] = {
        {"1 - 简单",   COL_ACCENT_GREEN},
        {"2 - 普通",   COL_ACCENT_ORANGE},
        {"3 - 困难",   COL_ACCENT_RED},
    };
    for (int i = 0; i < 3; i++)
    {
        int iy = 230 + i * 75;
        DrawRectangleRounded(
            {(float)(cx - 160), (float)(iy - 6), 320.0f, 56.0f},
            0.2f, 8, ColorAlpha(diffs[i].col, 0.08f));
        tw = MeasureTextF(diffs[i].label, 32);
        DrawTextF(diffs[i].label, cx - tw / 2, iy, 32, diffs[i].col);
    }

    const char *back = "Esc - 返回";
    DrawTextF(back, cx - MeasureTextF(back, 20) / 2, 540, 20, COL_TEXT_DIM);
}

void draw_game(const GameState *game)
{
    ClearBackground(COL_BG);
    draw_board(game);
    draw_panel(game);
    if (game->game_over)
        draw_game_over(game);
}
