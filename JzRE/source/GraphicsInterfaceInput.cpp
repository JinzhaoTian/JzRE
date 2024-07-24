#include "GraphicsInterfaceInput.h"

namespace JzRE {

RawPtr<GLFWwindow> GraphicsInterfaceInput::window = nullptr;
F32 GraphicsInterfaceInput::lastX = 0.0;
F32 GraphicsInterfaceInput::lastY = 0.0;
F32 GraphicsInterfaceInput::deltaX = 0.0;
F32 GraphicsInterfaceInput::deltaY = 0.0;
F32 GraphicsInterfaceInput::offsetX = 0.0;
F32 GraphicsInterfaceInput::offsetY = 0.0;
Bool GraphicsInterfaceInput::firstMouse = true;

void GraphicsInterfaceInput::Initialize(RawPtr<GLFWwindow> window) {
    GraphicsInterfaceInput::window = window;

    // callback: mouse
    glfwSetCursorPosCallback(window, callback_cursor_pos);
    glfwSetScrollCallback(window, callback_mouse_scroll);
    glfwSetMouseButtonCallback(window, callback_mouse_button);

    // callback: key
    glfwSetKeyCallback(window, callback_keyboard);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void GraphicsInterfaceInput::Update() {
}

Bool GraphicsInterfaceInput::IsKeyPressed(I32 key) {
    return keys[key];
}

Bool GraphicsInterfaceInput::IsMouseButtonPressed(I32 button) {
    return mouseButtons[button];
}

glm::vec2 GraphicsInterfaceInput::GetMousePosition() {
    F64 xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    return glm::vec2(StaticCast<F32>(xpos), StaticCast<F32>(ypos));
}

glm::vec2 GraphicsInterfaceInput::GetMouseMovement() {
    return glm::vec2(deltaX, deltaY);
}

glm::vec2 GraphicsInterfaceInput::GetMouseScroll() {
    return glm::vec2(offsetX, offsetY);
}

void GraphicsInterfaceInput::callback_cursor_pos(RawPtr<GLFWwindow> window, F64 xposin, F64 yposin) {
    F32 xpos = StaticCast<F32>(xposin);
    F32 ypos = StaticCast<F32>(yposin);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    deltaX = xpos - lastX;
    deltaY = -(ypos - lastY); // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;
}

void GraphicsInterfaceInput::callback_mouse_scroll(RawPtr<GLFWwindow> window, F64 xoffset, F64 yoffset) {
    offsetX = StaticCast<F32>(xoffset);
    offsetY = StaticCast<F32>(yoffset);
}

void GraphicsInterfaceInput::callback_mouse_button(RawPtr<GLFWwindow> window, I32 button, I32 action, I32 mods) {
    if (action == GLFW_PRESS) {
        mouseButtons[button] = true;
    } else if (action == GLFW_RELEASE) {
        mouseButtons[button] = false;
    }
}

void GraphicsInterfaceInput::callback_keyboard(RawPtr<GLFWwindow> window, I32 key, I32 scancode, I32 action, I32 mode) {
    if (action == GLFW_PRESS) {
        keys[key] = true;
    } else if (action == GLFW_RELEASE) {
        keys[key] = false;
    }
}

} // namespace JzRE