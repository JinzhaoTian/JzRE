#include "OGLInput.h"

namespace JzRE {

RawPtr<GLFWwindow> OGLInput::window = nullptr;
F32 OGLInput::lastX = 0.0;
F32 OGLInput::lastY = 0.0;
F32 OGLInput::deltaX = 0.0;
F32 OGLInput::deltaY = 0.0;
F32 OGLInput::offsetX = 0.0;
F32 OGLInput::offsetY = 0.0;
Bool OGLInput::firstMouse = true;

UnorderedMap<int, bool> OGLInput::keys;
UnorderedMap<int, bool> OGLInput::mouseButtons;

void OGLInput::Initialize(RawPtr<GLFWwindow> window) {
    OGLInput::window = window;

    // callback: mouse
    glfwSetCursorPosCallback(window, callback_mouse_cursor_pos);
    glfwSetScrollCallback(window, callback_mouse_scroll);
    glfwSetMouseButtonCallback(window, callback_mouse_button);

    // callback: key
    glfwSetKeyCallback(window, callback_keyboard);
}

Bool OGLInput::IsKeyPressed(I32 key) {
    auto it = keys.find(key);
    return it != keys.end() && it->second;
}

Bool OGLInput::IsMouseButtonPressed(I32 button) {
    auto it = mouseButtons.find(button);
    return it != mouseButtons.end() && it->second;
}

glm::vec2 OGLInput::GetMousePosition() {
    F64 xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    return glm::vec2(StaticCast<F32>(xpos), StaticCast<F32>(ypos));
}

glm::vec2 OGLInput::GetMouseMovement() {
    return glm::vec2(deltaX, deltaY);
}

glm::vec2 OGLInput::GetMouseScroll() {
    return glm::vec2(offsetX, offsetY);
}

void OGLInput::callback_mouse_cursor_pos(RawPtr<GLFWwindow> window, F64 xposin, F64 yposin) {
    F32 xpos = StaticCast<F32>(xposin);
    F32 ypos = StaticCast<F32>(yposin);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    deltaX = xpos - lastX;
    deltaY = ypos - lastY;

    lastX = xpos;
    lastY = ypos;
}

void OGLInput::callback_mouse_scroll(RawPtr<GLFWwindow> window, F64 xoffset, F64 yoffset) {
    offsetX = StaticCast<F32>(xoffset);
    offsetY = StaticCast<F32>(yoffset);
}

void OGLInput::callback_mouse_button(RawPtr<GLFWwindow> window, I32 button, I32 action, I32 mods) {
    if (action == GLFW_PRESS) {
        mouseButtons[button] = true;
    } else if (action == GLFW_RELEASE) {
        mouseButtons[button] = false;
    }
}

void OGLInput::callback_keyboard(RawPtr<GLFWwindow> window, I32 key, I32 scancode, I32 action, I32 mode) {
    if (action == GLFW_PRESS) {
        keys[key] = true;
    } else if (action == GLFW_RELEASE) {
        keys[key] = false;
    }
}

} // namespace JzRE