#pragma once

#include "CommonTypes.h"
#include "OGLRenderWindow.h"
#include "OGLRenderer.h"
#include "OGLScene.h"
#include "OGLInput.h"
#include "RenderableObject.h"
#include "OGLModel.h"
#include "OGLCamera.h"
#include "OGLDirectionalLight.h"
#include "OGLUI.h"

namespace JzRE {
class OGLRenderEngine {
public:
    OGLRenderEngine();
    ~OGLRenderEngine();

    Bool Initialize();
    void Run();
    void Shutdown();

private:
    const I32 wndWidth = 1200;
    const I32 wndHeight = 800;

    const String title = "Graphics Interface Rendering Engine";

    Bool isRunning;

    SharedPtr<OGLRenderWindow> window;
    SharedPtr<OGLRenderer> renderer;
    SharedPtr<OGLScene> scene;
    SharedPtr<OGLCamera> camera;
    SharedPtr<OGLUI> simpleUI;

    Bool InitScene();
    void ProcessInput();
};
} // namespace JzRE