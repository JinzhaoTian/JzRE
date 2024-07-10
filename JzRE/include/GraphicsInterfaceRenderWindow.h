#pragma once

#include <iostream>
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "CommonTypes.h"
#include "Framebuffer.h"

namespace JzRE {

class GraphicsInterfaceRenderWindow {
public:
    GLFWwindow *hwnd;

    I32 wndWidth, wndHeight;

public:
    GraphicsInterfaceRenderWindow(I32 w, I32 h, const String &title);
    ~GraphicsInterfaceRenderWindow();

    void ResizeWindow(I32 w, I32 h);

    bool ShouldClose();
    void PollEvents();
    void SwapBuffer();
    void ClearBuffer();

    void ProcessInput();
};

} // namespace JzRE