#pragma once

#include "CommonTypes.h"
#include "GraphicsInterfaceRenderWindow.h"

namespace JzRE {
class GraphicsInterfaceRenderEngine {
public:
    GraphicsInterfaceRenderEngine();
    ~GraphicsInterfaceRenderEngine();

    bool Initialize();
    void Run();
    void Shutdown();

private:
    const int wndWidth = 800;
    const int wndHeight = 600;

    const String title = "Graphics Interface Render Engine";

    bool isRunning;

    GraphicsInterfaceRenderWindow window;
};
} // namespace JzRE