#pragma once

#include "CommonTypes.h"
#include "GraphicsInterfaceRenderWindow.h"
#include "GraphicsInterfaceRenderer.h"

namespace JzRE {
class GraphicsInterfaceRenderEngine {
public:
    GraphicsInterfaceRenderEngine();
    ~GraphicsInterfaceRenderEngine();

    Bool Initialize();
    void Run();
    void Shutdown();

private:
    const I32 wndWidth = 800;
    const I32 wndHeight = 600;

    const String title = "Graphics Interface Render Engine";

    Bool isRunning;

    GraphicsInterfaceRenderWindow window;
    GraphicsInterfaceRenderer renderer;
};
} // namespace JzRE