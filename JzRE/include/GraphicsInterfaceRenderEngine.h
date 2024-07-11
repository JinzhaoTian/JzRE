#pragma once

#include "CommonTypes.h"
#include "GraphicsInterfaceRenderWindow.h"

namespace JzRE {
class GraphicsInterfaceRenderEngine {
public:
    GraphicsInterfaceRenderEngine();
    void Render();

private:
    const int wndWidth = 800;
    const int wndHeight = 600;

    const String title = "GraphicsInterface Render Engine";

    SharedPtr<GraphicsInterfaceRenderWindow> window;
};
} // namespace JzRE