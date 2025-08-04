#pragma once

#include "CommonTypes.h"
#include "JzCamera.h"
#include "JzModel.h"
#include "JzScene.h"
#include "OGLDirectionalLight.h"
#include "OGLInput.h"
#include "OGLRenderWindow.h"
#include "OGLRenderer.h"
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
    const I32 wndWidth  = 1200;
    const I32 wndHeight = 800;

    const String title = "Graphics Interface Rendering Engine";

    Bool isRunning;

    std::shared_ptr<OGLRenderWindow> window;
    std::shared_ptr<OGLRenderer>     renderer;
    std::shared_ptr<JzScene>         scene;
    std::shared_ptr<JzCamera>        camera;
    std::shared_ptr<OGLUI>           simpleUI;

    Bool InitScene();
    void ProcessInput();
};
} // namespace JzRE