#pragma once

#include "CommonTypes.h"

namespace JzRE {
class GraphicsInterfaceInput {
public:
    static void Initialize(RawPtr<GLFWwindow> window);

    static Bool IsKeyPressed(I32 key);
    static Bool IsMouseButtonPressed(I32 button);
    static glm::vec2 GetMousePosition();
    static glm::vec2 GetMouseMovement();
    static glm::vec2 GetMouseScroll();

private:
    static RawPtr<GLFWwindow> window;
    static F32 lastX, lastY;
    static F32 deltaX, deltaY;
    static F32 offsetX, offsetY;
    static Bool firstMouse;

    static void callback_cursor_pos(RawPtr<GLFWwindow> window, F64 xposin, F64 yposin);
    static void callback_mouse_scroll(RawPtr<GLFWwindow> window, F64 xoffset, F64 yoffset);
    static void callback_mouse_button(RawPtr<GLFWwindow> window, I32 button, I32 action, I32 mods);
    static void callback_keyboard(RawPtr<GLFWwindow> window, I32 key, I32 scancode, I32 action, I32 mode);
};
} // namespace JzRE