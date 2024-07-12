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
    const int wndWidth = 800;
    const int wndHeight = 600;

    const String title = "Software/CPU Render Engine";

    bool isRunning;

    SoftwareRenderWindow window;
    Renderer renderer;
};
} // namespace JzRE