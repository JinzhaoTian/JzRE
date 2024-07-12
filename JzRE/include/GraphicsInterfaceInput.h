#pragma once

#include "CommonTypes.h"

namespace JzRE {
class GraphicsInterfaceInput {
public:
    static void Initialize(GLFWwindow *window);

    static bool IsKeyPressed(int key);
    static bool IsMouseButtonPressed(int button);
    static glm::vec2 getMousePosition();
    static glm::vec2 getMouseDelta();

private:
    static GLFWwindow *window;
    static double lastX, lastY;
    static double deltaX, deltaY;
    static bool firstMouse;

    static void CursorPositionCallback(GLFWwindow *window, double xpos, double ypos);
};
} // namespace JzRE