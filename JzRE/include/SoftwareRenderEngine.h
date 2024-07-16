#pragma once

#include "CommonTypes.h"
#include "SoftwareRenderWindow.h"
#include "Renderer.h"

namespace JzRE {
class SoftwareRenderEngine {
public:
    SoftwareRenderEngine();
    ~SoftwareRenderEngine();

    bool Initialize();
    void Run();
    void Shutdown();

private:
    const I32 wndWidth = 800;
    const I32 wndHeight = 600;

    const String title = "Software/CPU Render Engine";

    Bool isRunning;

    SoftwareRenderWindow window;
    Renderer renderer;
};
} // namespace JzRE