#pragma once

#include "CommonTypes.h"
#include "Framebuffer.h"

namespace JzRE {

class GraphicsInterfaceRenderWindow {
public:
    GraphicsInterfaceRenderWindow();
    ~GraphicsInterfaceRenderWindow();

    Bool Initialize(I32 w, I32 h, const String &title);

    RawPtr<GLFWwindow> GetGLFWwindow();
    void ResizeWindow(I32 w, I32 h);

    Bool ShouldClose();
    void PollEvents();

    void CreateFramebuffer();
    void SwapFramebuffer();
    void ClearFramebuffer();

    void ProcessInput();

private:
    RawPtr<GLFWwindow> hwnd;

    I32 wndWidth, wndHeight;
    String title;
};

} // namespace JzRE