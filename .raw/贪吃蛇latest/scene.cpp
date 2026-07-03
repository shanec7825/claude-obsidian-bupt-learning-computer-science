// scene.cpp
#include "scene.h"
#include "render.h"
#include "input.h"
#include "audio.h"

// --- MenuScene ---
void MenuScene::update()
{
    if (_showing_difficulty)
    {
        if (is_key_pressed(KEY_ONE))
        {
            _selected_difficulty = AI_EASY;
            selected_mode = MODE_BATTLE;
            _next = SCENE_PLAY;
        }
        else if (is_key_pressed(KEY_TWO))
        {
            _selected_difficulty = AI_NORMAL;
            selected_mode = MODE_BATTLE;
            _next = SCENE_PLAY;
        }
        else if (is_key_pressed(KEY_THREE))
        {
            _selected_difficulty = AI_HARD;
            selected_mode = MODE_BATTLE;
            _next = SCENE_PLAY;
        }
        else if (is_key_pressed(KEY_ESCAPE))
        {
            _showing_difficulty = false;
        }
        return;
    }

    if (is_key_pressed(KEY_ONE))
    {
        selected_mode = MODE_CLASSIC;
        _next = SCENE_PLAY;
    }
    else if (is_key_pressed(KEY_TWO))
    {
        selected_mode = MODE_PVP;
        _next = SCENE_PLAY;
    }
    else if (is_key_pressed(KEY_THREE))
    {
        _showing_difficulty = true;
    }
    else if (is_key_pressed(KEY_ESCAPE))
    {
        _next = SCENE_EXIT;
    }
}
void MenuScene::render()
{
    if (_showing_difficulty)
        draw_difficulty_menu();
    else
        draw_menu();
}

// --- PlayScene ---
void PlayScene::update()
{
    // Handle input
    if (is_key_pressed(KEY_ESCAPE))
    {
        _next = SCENE_MENU;
        return;
    }
    if (is_key_pressed(KEY_R))
    {
        restart_game();
        audio_play_start();
        return;
    }
    if (is_key_pressed(KEY_SPACE) && _game.started && !_game.game_over)
        _game.paused = !_game.paused;
    if (!_game.game_over)
    {
        // Player 1: WASD
        if (is_key_pressed(KEY_W))
            set_direction(&_game, UP);
        else if (is_key_pressed(KEY_S))
            set_direction(&_game, DOWN);
        else if (is_key_pressed(KEY_A))
            set_direction(&_game, LEFT);
        else if (is_key_pressed(KEY_D))
            set_direction(&_game, RIGHT);

        // Player 2 (PvP only): Arrow keys
        if (_game.mode == MODE_PVP && _game.ai_is_alive)
        {
            if (is_key_pressed(KEY_UP))
                set_p2_direction(&_game, UP);
            else if (is_key_pressed(KEY_DOWN))
                set_p2_direction(&_game, DOWN);
            else if (is_key_pressed(KEY_LEFT))
                set_p2_direction(&_game, LEFT);
            else if (is_key_pressed(KEY_RIGHT))
                set_p2_direction(&_game, RIGHT);
        }

        // Drop-tail mechanic (G for P1 in PvP/Battle, L for P2 in PvP)
        if (_game.mode != MODE_CLASSIC && _game.length > 3 && is_key_pressed(KEY_G))
            _game.p1_drop_requested = true;
        if (_game.mode == MODE_PVP && _game.ai_is_alive && _game.ai_length > 3 && is_key_pressed(KEY_L))
            _game.p2_drop_requested = true;
    }

    // Timed movement with independent per-player speed-up
    double now = GetTime();

    if (_game.mode == MODE_PVP || _game.mode == MODE_BATTLE)
    {
        // ── PvP / Battle: independent per-player intervals ──
        double p1_interval = MOVE_INTERVAL_MS / 1000.0;
        double p2_interval = MOVE_INTERVAL_MS / 1000.0;

        // P1 speed-up: F key (4x)
        if (is_key_held(KEY_F))
            p1_interval = (MOVE_INTERVAL_MS / 4) / 1000.0;

        // P2 speed-up: Right Shift (PvP only, 4x)
        if (_game.mode == MODE_PVP && is_key_held(KEY_RIGHT_SHIFT))
            p2_interval = (MOVE_INTERVAL_MS / 4) / 1000.0;

        bool move_p1 = (now - _last_p1_move >= p1_interval);
        bool move_p2 = (now - _last_p2_move >= p2_interval);

        if (move_p1 || move_p2)
        {
            int prev_eat = _game.food_eaten;
            int prev_ai_eat = _game.ai_food_eaten;

            if (_game.mode == MODE_PVP && !_game.game_over)
                move_pvp_snakes(&_game, move_p1, move_p2);
            else if (_game.mode == MODE_BATTLE && !_game.game_over)
                move_battle_snakes(&_game, move_p1, move_p2);

            if (move_p1) _last_p1_move = now;
            if (move_p2) _last_p2_move = now;

            // Audio: food eaten
            if (_game.food_eaten > prev_eat || _game.ai_food_eaten > prev_ai_eat)
                audio_play_eat();

            // Audio: game over just triggered
            if (_game.game_over && !_prev_game_over)
                audio_play_death();
            _prev_game_over = _game.game_over;
        }
    }
    else
    {
        // ── Classic mode: single timer (original behavior) ──
        double interval = MOVE_INTERVAL_MS / 1000.0;

        if (is_key_held(KEY_LEFT_SHIFT) || is_key_held(KEY_RIGHT_SHIFT) || is_key_held(KEY_F))
            interval = (MOVE_INTERVAL_MS / 4) / 1000.0;

        if (now - _last_move >= interval)
        {
            int prev_eat = _game.food_eaten;

            move_snake(&_game);
            _last_move = now;

            if (_game.food_eaten > prev_eat)
                audio_play_eat();

            if (_game.game_over && !_prev_game_over)
                audio_play_death();
            _prev_game_over = _game.game_over;
        }
    }
}
void PlayScene::render()
{
    draw_game(&_game);
}
