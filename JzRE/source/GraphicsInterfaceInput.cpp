#include "GraphicsInterfaceInput.h"

namespace JzRE {

GLFWwindow *GraphicsInterfaceInput::window = nullptr;
double GraphicsInterfaceInput::lastX = 0.0;
double GraphicsInterfaceInput::lastY = 0.0;
double GraphicsInterfaceInput::deltaX = 0.0;
double GraphicsInterfaceInput::deltaY = 0.0;
bool GraphicsInterfaceInput::firstMouse = true;

void GraphicsInterfaceInput::Initialize(GLFWwindow *window) {
    GraphicsInterfaceInput::window = window;
    glfwSetCursorPosCallback(window, CursorPositionCallback);
}

bool GraphicsInterfaceInput::IsKeyPressed(int key) {
    return glfwGetKey(window, key) == GLFW_PRESS;
}

bool GraphicsInterfaceInput::IsMouseButtonPressed(int button) {
    return glfwGetMouseButton(window, button) == GLFW_PRESS;
}

glm::vec2 GraphicsInterfaceInput::getMousePosition() {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    return glm::vec2(StaticCast<float>(xpos), StaticCast<float>(ypos));
}

glm::vec2 GraphicsInterfaceInput::getMouseDelta() {
    return glm::vec2(StaticCast<float>(deltaX), StaticCast<float>(deltaY));
}

void GraphicsInterfaceInput::CursorPositionCallback(GLFWwindow *window, double xpos, double ypos) {
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
} // namespace JzRE