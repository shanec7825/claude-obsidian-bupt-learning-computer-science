#include <raylib.h>
#include "input.h"
#include "scene.h"
#include "render.h"
#include "audio.h"

#ifdef _WIN32
// 手动声明 ImmAssociateContext，避免 imm.h 与 raylib 内部 windows.h 冲突
// HWND 和 HIMC 本质上都是 void*，无需引入任何 Windows 头文件
extern "C" void* __stdcall ImmAssociateContext(void* hWnd, void* hIMC);
#pragma comment(lib, "imm32.lib")
#endif

// Resize the raylib window in-place (no close+reopen needed)
static void resize_raylib_window(int width, int height)
{
    SetWindowSize(width, height);
}

int main()
{
    SetConfigFlags(FLAG_VSYNC_HINT);  // smooth rendering
    InitWindow(MENU_WINDOW_WIDTH, MENU_WINDOW_HEIGHT, "Snake Game");
    SetExitKey(KEY_NULL);  // 禁用默认的ESC退出，由场景自行处理

#ifdef _WIN32
    // 禁用中文输入法，防止干扰游戏按键（WASD等）
    ImmAssociateContext(GetWindowHandle(), NULL);
#endif

    SetTargetFPS(60);

    init_render_resources();
    init_audio();

    Scene *current = new MenuScene();

    while (!WindowShouldClose())
    {
        // --- Update ---
        current->update();

        SceneType next = current->next_scene();
        if (next == SCENE_EXIT)
            break;

        if (next != SCENE_MENU && dynamic_cast<MenuScene *>(current) != nullptr)
        {
            // Entering game from menu
            MenuScene *menu = static_cast<MenuScene *>(current);
            GameMode mode = menu->selected_mode;
            delete current;
            if (mode == MODE_CLASSIC)
                resize_raylib_window(CLASSIC_WINDOW_WIDTH, CLASSIC_WINDOW_HEIGHT);
            else
                resize_raylib_window(BATTLE_WINDOW_WIDTH, BATTLE_WINDOW_HEIGHT);
            current = new PlayScene(mode, menu->selected_difficulty());
            audio_play_start();
        }
        else if (next == SCENE_MENU && dynamic_cast<PlayScene *>(current) != nullptr)
        {
            delete current;
            resize_raylib_window(MENU_WINDOW_WIDTH, MENU_WINDOW_HEIGHT);
            current = new MenuScene();
        }

        // --- Render ---
        BeginDrawing();
        current->render();
        EndDrawing();
    }

    delete current;
    unload_audio();
    unload_render_resources();
    CloseWindow();
    return 0;
}
