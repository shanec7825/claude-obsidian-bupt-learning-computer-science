#pragma once
#ifndef INPUT_H
#define INPUT_H

// ============================================================================
// input.h — 键盘输入处理模块（Windows 平台）
//
// 基于 Win32 GetAsyncKeyState 实现轻量级键盘输入检测：
//   - key_pressed() ：边缘触发，仅按键"按下瞬间"返回 true，适合菜单 / 转向
//   - key_held()    ：电平触发，按键持续按住期间均返回 true，适合加速等持续操作
//   - disable_ime() ：禁用 IME，防止中文输入法拦截字母键
//   - reset_key_state() ：清空按键历史状态（窗口 resize 后调用，避免残留按键）
//
// 依赖：Win32 API（windows.h, imm.h, imm32.lib）
// ============================================================================

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

// 按键"按下瞬间"检测（边缘触发）。
// 参数 vk 为虚拟键码（如 'W', VK_ESCAPE, VK_UP 等）。
// 返回值：本帧按下且上一帧未按下时为 true；连续按住仅首次返回 true。
bool key_pressed(int vk);

// 按键"持续按住"检测（电平触发）。
// 参数 vk 为虚拟键码（如 VK_SHIFT, 'F' 等）。
// 返回值：只要按键处于按下状态即返回 true。
bool key_held(int vk);

// 禁用当前图形窗口的 IME 输入法。
// 调用后中文输入法不再拦截字母 / 数字键，确保 WASD / 123 等键正常响应。
void disable_ime();

// 重置按键历史状态（将所有键的"上一帧状态"清零）。
// 通常在窗口 resize 后调用，防止前一窗口的按键状态泄漏到新窗口。
void reset_key_state();

#ifdef __cplusplus
}
#endif

#endif // INPUT_H
