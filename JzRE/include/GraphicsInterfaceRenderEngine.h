#pragma once

#include "CommonTypes.h"
#include "GraphicsInterfaceRenderWindow.h"
#include "GraphicsInterfaceRenderer.h"
#include "GraphicsInterfaceScene.h"
#include "GraphicsInterfaceInput.h"
#include "GraphicsInterfaceResourceManager.h"
#include "RenderableObject.h"
#include "GraphicsInterfaceCamera.h"

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

    SharedPtr<GraphicsInterfaceRenderWindow> window;
    SharedPtr<GraphicsInterfaceRenderer> renderer;
    SharedPtr<GraphicsInterfaceScene> scene;
    SharedPtr<GraphicsInterfaceCamera> camera;

    Bool InitScene();
    void ProcessInput();
};
} // namespace JzRE