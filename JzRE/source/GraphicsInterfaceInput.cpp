#include "GraphicsInterfaceInput.h"

namespace JzRE {

RawPtr<GLFWwindow> GraphicsInterfaceInput::window = nullptr;
F32 GraphicsInterfaceInput::lastX = 0.0;
F32 GraphicsInterfaceInput::lastY = 0.0;
F32 GraphicsInterfaceInput::deltaX = 0.0;
F32 GraphicsInterfaceInput::deltaY = 0.0;
Bool GraphicsInterfaceInput::firstMouse = true;

void GraphicsInterfaceInput::Initialize(RawPtr<GLFWwindow> window) {
    GraphicsInterfaceInput::window = window;

    // callback: mouse
    glfwSetCursorPosCallback(window, callback_cursor_pos);
    glfwSetScrollCallback(window, callback_scroll);
    glfwSetMouseButtonCallback(window, callback_mouse_button);

    // callback: key
    glfwSetKeyCallback(window, callback_key);
}

Bool GraphicsInterfaceInput::IsKeyPressed(I32 key) {
    return glfwGetKey(window, key) == GLFW_PRESS;
}

Bool GraphicsInterfaceInput::IsMouseButtonPressed(I32 button) {
    return glfwGetMouseButton(window, button) == GLFW_PRESS;
}

glm::vec2 GraphicsInterfaceInput::GetMousePosition() {
    F64 xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    return glm::vec2(StaticCast<F32>(xpos), StaticCast<F32>(ypos));
}

glm::vec2 GraphicsInterfaceInput::GetMouseDelta() {
    return glm::vec2(StaticCast<F32>(deltaX), StaticCast<F32>(deltaY));
}

void GraphicsInterfaceInput::callback_cursor_pos(RawPtr<GLFWwindow> window, F64 xpos, F64 ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    deltaX = xpos - lastX;
    deltaY = lastY - ypos; // 反转y轴方向

    lastX = xpos;
    lastY = ypos;
}

void GraphicsInterfaceInput::callback_scroll(RawPtr<GLFWwindow> window, F64 xoffset, F64 yoffset) {
}

void GraphicsInterfaceInput::callback_mouse_button(RawPtr<GLFWwindow> window, I32 button, I32 action, I32 mods) {
}

void GraphicsInterfaceInput::callback_key(RawPtr<GLFWwindow> window, I32 key, I32 scancode, I32 action, I32 mode) {
}
} // namespace JzRE