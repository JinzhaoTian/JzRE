#pragma once

#include "CommonTypes.h"

namespace JzRE {
class GraphicsInterfaceInput {
public:
    static void Initialize(RawPtr<GLFWwindow> window);

    static Bool IsKeyPressed(I32 key);
    static Bool IsMouseButtonPressed(I32 button);
    static glm::vec2 GetMousePosition();
    static glm::vec2 GetMouseDelta();

private:
    static RawPtr<GLFWwindow> window;
    static F32 lastX, lastY;
    static F32 deltaX, deltaY;
    static Bool firstMouse;

    static void callback_cursor_pos(RawPtr<GLFWwindow> window, F64 xpos, F64 ypos);

    static void callback_scroll(RawPtr<GLFWwindow> window, F64 xoffset, F64 yoffset);

    static void callback_mouse_button(RawPtr<GLFWwindow> window, I32 button, I32 action, I32 mods);

    static void callback_key(RawPtr<GLFWwindow> window, I32 key, I32 scancode, I32 action, I32 mode);
};
} // namespace JzRE