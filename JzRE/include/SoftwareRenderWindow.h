#pragma once

#include "CommonTypes.h"
#include "Framebuffer.h"

namespace JzRE {

class SoftwareRenderWindow {
public:
    HWND hwnd;
    HDC screenHDC;

    I32 wndWidth, wndHeight;

    bool hasClosed;

    SharedPtr<Framebuffer> frontBuffer;
    SharedPtr<Framebuffer> backBuffer;

public:
    SoftwareRenderWindow(I32 w, I32 h, const String &title);
    ~SoftwareRenderWindow();

    void ResizeWindow(I32 w, I32 h);

    bool ShouldClose();
    void PollEvents();
    void SwapBuffer();
    void ClearBuffer();
};

} // namespace JzRE