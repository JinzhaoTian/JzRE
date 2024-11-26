#pragma once

#include "CommonTypes.h"
#include "Framebuffer.h"

namespace JzRE {

class OGLRenderWindow {
public:
    OGLRenderWindow(I32 width, I32 height, const String &title);
    ~OGLRenderWindow();

    RawPtr<GLFWwindow> GetGLFWwindow();
    I32 GetWindowWidth();
    I32 GetWindowHeight();

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