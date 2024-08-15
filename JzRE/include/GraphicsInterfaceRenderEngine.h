#pragma once

#include "CommonTypes.h"
#include "GraphicsInterfaceRenderWindow.h"
#include "GraphicsInterfaceRenderer.h"
#include "GraphicsInterfaceScene.h"
#include "GraphicsInterfaceInput.h"
#include "RenderableObject.h"
#include "GraphicsInterfaceModel.h"
#include "GraphicsInterfaceCamera.h"
#include "GraphicsInterfaceDirectionalLight.h"
#include "GraphicsInterfaceUI.h"

namespace JzRE {
class GraphicsInterfaceRenderEngine {
public:
    GraphicsInterfaceRenderEngine();
    ~GraphicsInterfaceRenderEngine();

    Bool Initialize();
    void Run();
    void Shutdown();

private:
    const I32 wndWidth = 1200;
    const I32 wndHeight = 800;

    const String title = "Graphics Interface Rendering Engine";

    Bool isRunning;

    SharedPtr<GraphicsInterfaceRenderWindow> window;
    SharedPtr<GraphicsInterfaceRenderer> renderer;
    SharedPtr<GraphicsInterfaceScene> scene;
    SharedPtr<GraphicsInterfaceCamera> camera;
    SharedPtr<GraphicsInterfaceUI> simpleUI;

    Bool InitScene();
    void ProcessInput();
};
} // namespace JzRE