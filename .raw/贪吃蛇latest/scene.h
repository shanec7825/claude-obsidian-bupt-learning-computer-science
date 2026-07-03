#pragma once
#include "game.h"

enum SceneType
{
    SCENE_MENU,
    SCENE_PLAY,
    SCENE_EXIT
};

struct Scene
{
    virtual ~Scene() = default;
    virtual void update() = 0;
    virtual void render() = 0;
    virtual SceneType next_scene() const = 0;
};

class MenuScene : public Scene
{
    SceneType _next = SCENE_MENU;
    bool _showing_difficulty = false;
    AIDifficulty _selected_difficulty = AI_NORMAL;

public:
    void update() override;
    void render() override;
    SceneType next_scene() const override { return _next; }
    GameMode selected_mode = MODE_CLASSIC;
    AIDifficulty selected_difficulty() const { return _selected_difficulty; }
    bool showing_difficulty() const { return _showing_difficulty; }
};

class PlayScene : public Scene
{
    GameState _game;
    double _last_move = 0;       // classic mode
    double _last_p1_move = 0;    // PvP / Battle: independent P1 timer
    double _last_p2_move = 0;    // PvP / Battle: independent P2 timer
    SceneType _next = SCENE_PLAY;
    AIDifficulty _difficulty;
    int _prev_food_eaten = 0;
    int _prev_ai_food_eaten = 0;
    bool _prev_game_over = false;

    void restart_game()
    {
        if (_game.mode == MODE_BATTLE)
        {
            // Clear AI body from old map before reinit
            for (int r = 0; r < _game.map_size; r++)
                for (int c = 0; c < _game.map_size; c++)
                    if (_game.map[r][c] == 'A' || _game.map[r][c] == 'C')
                        _game.map[r][c] = '.';
            init_battle_game(&_game, _difficulty);
        }
        else if (_game.mode == MODE_PVP)
        {
            // Clear P2 body from old map before reinit
            for (int r = 0; r < _game.map_size; r++)
                for (int c = 0; c < _game.map_size; c++)
                    if (_game.map[r][c] == 'A' || _game.map[r][c] == 'C')
                        _game.map[r][c] = '.';
            init_pvp_game(&_game);
        }
        else
        {
            init_game(&_game, _game.mode);
        }
        _last_move = GetTime();
        _last_p1_move = _last_move;
        _last_p2_move = _last_move;
    }

public:
    PlayScene(GameMode mode, AIDifficulty difficulty = AI_NORMAL)
        : _difficulty(difficulty)
    {
        if (mode == MODE_BATTLE)
            init_battle_game(&_game, difficulty);
        else if (mode == MODE_PVP)
            init_pvp_game(&_game);
        else
            init_game(&_game, mode);
        _last_move = GetTime();
        _last_p1_move = _last_move;
        _last_p2_move = _last_move;
    }
    void update() override;
    void render() override;
    SceneType next_scene() const override { return _next; }
};
