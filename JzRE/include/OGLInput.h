#pragma once

#include "CommonTypes.h"

namespace JzRE {
class OGLInput {
public:
    static void Initialize(GLFWwindow* window);
    static Bool IsKeyPressed(I32 key);
    static Bool IsMouseButtonPressed(I32 button);
    static glm::vec2 GetMousePosition();
    static glm::vec2 GetMouseMovement();
    static glm::vec2 GetMouseScroll();

private:
    static GLFWwindow* window;
    static F32 lastX, lastY;
    static F32 deltaX, deltaY;
    static F32 offsetX, offsetY;
    static Bool firstMouse;

    static std::unordered_map<int, bool> keys;
    static std::unordered_map<int, bool> mouseButtons;

    static void callback_mouse_cursor_pos(GLFWwindow* window, F64 xposin, F64 yposin);
    static void callback_mouse_scroll(GLFWwindow* window, F64 xoffset, F64 yoffset);
    static void callback_mouse_button(GLFWwindow* window, I32 button, I32 action, I32 mods);
    static void callback_keyboard(GLFWwindow* window, I32 key, I32 scancode, I32 action, I32 mode);
};
} // namespace JzRE