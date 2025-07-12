#pragma once

#include "CommonTypes.h"
#include "Framebuffer.h"

namespace JzRE {

class SoftwareRenderWindow {
public:
    GLFWwindow* window;
    
    I32 wndWidth, wndHeight;
    String title;

    Bool hasClosed;

    SharedPtr<Framebuffer> frontBuffer;
    SharedPtr<Framebuffer> backBuffer;

public:
    SoftwareRenderWindow();
    ~SoftwareRenderWindow();

    bool Initialize(I32 w, I32 h, const String &title);

    void ResizeWindow(I32 w, I32 h);

    bool ShouldClose();
    void PollEvents();
    void SwapBuffer();
    void ClearBuffer();
};

} // namespace JzRE