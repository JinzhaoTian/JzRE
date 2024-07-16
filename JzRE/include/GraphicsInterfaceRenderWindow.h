#pragma once

#include "CommonTypes.h"
#include "Framebuffer.h"

namespace JzRE {

class GraphicsInterfaceRenderWindow {
public:
    RawPtr<GLFWwindow> hwnd;

    I32 wndWidth, wndHeight;
    String title;

public:
    GraphicsInterfaceRenderWindow();
    ~GraphicsInterfaceRenderWindow();

    Bool Initialize(I32 w, I32 h, const String &title);

    void ResizeWindow(I32 w, I32 h);

    Bool ShouldClose();
    void PollEvents();

    void CreateFramebuffer();
    void SwapFramebuffer();
    void ClearFramebuffer();

    void ProcessInput();
};

} // namespace JzRE