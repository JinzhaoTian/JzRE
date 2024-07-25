#pragma once

#include "CommonTypes.h"
#include "Framebuffer.h"

namespace JzRE {

class GraphicsInterfaceRenderWindow {
public:
    GraphicsInterfaceRenderWindow(I32 width, I32 height, const String &title);
    ~GraphicsInterfaceRenderWindow();

    RawPtr<GLFWwindow> GetGLFWwindow();
    void ResizeWindow(I32 w, I32 h);
    Bool ShouldClose();
    void PollEvents();
    void SwapFramebuffer();

private:
    RawPtr<GLFWwindow> hwnd;

    I32 wndWidth, wndHeight;
    String title;
};

} // namespace JzRE