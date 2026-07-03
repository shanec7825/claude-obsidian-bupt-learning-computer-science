#pragma once
#include "game.h"

// Chinese-capable game font
extern Font g_game_font;

// Color palette
extern const Color COL_WALL;
extern const Color COL_OBSTACLE;
extern const Color COL_HEAD;
extern const Color COL_BODY;
extern const Color COL_HEAD_ALT;
extern const Color COL_BODY_ALT;
extern const Color COL_FOOD;
extern const Color COL_EMPTY;
extern const Color COL_GRID_LINE;
extern const Color COL_BG;
extern const Color COL_TEXT;
extern const Color COL_TEXT_DIM;
extern const Color COL_TEXT_TITLE;
extern const Color COL_PANEL_BG;
extern const Color COL_ACCENT_GREEN;
extern const Color COL_ACCENT_BLUE;
extern const Color COL_ACCENT_RED;
extern const Color COL_ACCENT_ORANGE;

void draw_board(const GameState *game);
void draw_panel(const GameState *game);
void draw_game_over(const GameState *game);
void draw_menu();
void draw_difficulty_menu();
void draw_game(const GameState *game);

// Load resources (fonts, etc.) — call once at startup
void init_render_resources();
void unload_render_resources();
