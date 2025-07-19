#include "OGLInput.h"

namespace JzRE {

GLFWwindow* OGLInput::window = nullptr;
F32 OGLInput::lastX = 0.0;
F32 OGLInput::lastY = 0.0;
F32 OGLInput::deltaX = 0.0;
F32 OGLInput::deltaY = 0.0;
F32 OGLInput::offsetX = 0.0;
F32 OGLInput::offsetY = 0.0;
Bool OGLInput::firstMouse = true;

std::unordered_map<int, bool> OGLInput::keys;
std::unordered_map<int, bool> OGLInput::mouseButtons;

void OGLInput::Initialize(GLFWwindow* window) {
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
    return glm::vec2(static_cast<F32>(xpos), static_cast<F32>(ypos));
}

glm::vec2 OGLInput::GetMouseMovement() {
    return glm::vec2(deltaX, deltaY);
}

glm::vec2 OGLInput::GetMouseScroll() {
    return glm::vec2(offsetX, offsetY);
}

void OGLInput::callback_mouse_cursor_pos(GLFWwindow* window, F64 xposin, F64 yposin) {
    F32 xpos = static_cast<F32>(xposin);
    F32 ypos = static_cast<F32>(yposin);

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

void OGLInput::callback_mouse_scroll(GLFWwindow* window, F64 xoffset, F64 yoffset) {
    offsetX = static_cast<F32>(xoffset);
    offsetY = static_cast<F32>(yoffset);
}

void OGLInput::callback_mouse_button(GLFWwindow* window, I32 button, I32 action, I32 mods) {
    if (action == GLFW_PRESS) {
        mouseButtons[button] = true;
    } else if (action == GLFW_RELEASE) {
        mouseButtons[button] = false;
    }
}

void OGLInput::callback_keyboard(GLFWwindow* window, I32 key, I32 scancode, I32 action, I32 mode) {
    if (action == GLFW_PRESS) {
        keys[key] = true;
    } else if (action == GLFW_RELEASE) {
        keys[key] = false;
    }
}

} // namespace JzRE