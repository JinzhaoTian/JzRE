#include "GraphicsInterfaceRenderEngine.h"

namespace JzRE {
GraphicsInterfaceRenderEngine::GraphicsInterfaceRenderEngine() :
    isRunning(false) {
}

GraphicsInterfaceRenderEngine::~GraphicsInterfaceRenderEngine() {
    Shutdown();
}

Bool GraphicsInterfaceRenderEngine::Initialize() {
    Bool isWndInited = this->window.Initialize(this->wndWidth, this->wndHeight, this->title);
    if (!isWndInited) {
        return false;
    }

    Bool isRendererInited = this->renderer.Initialize(this->wndWidth, this->wndHeight);
    if (!isRendererInited) {
        return false;
    }

    GraphicsInterfaceInput::Initialize(this->window.GetGLFWwindow());

    Bool isSceneInited = InitScene();
    if (!isSceneInited) {
        return false;
    }

    this->isRunning = true;
    return true;
}

void GraphicsInterfaceRenderEngine::Run() {
    auto previousTime = std::chrono::high_resolution_clock::now();

    // render loop
    while (isRunning && !window.ShouldClose()) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        F32 deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - previousTime).count();
        previousTime = currentTime;

        window.ProcessInput();

        scene.Update(deltaTime);

        renderer.Clear();
        renderer.Render(scene);

        window.SwapFramebuffer();
        window.PollEvents();
    }
}

void GraphicsInterfaceRenderEngine::Shutdown() {
    GraphicsInterfaceResourceManager::getInstance().Clear();
    renderer.Shutdown();
    window.Shutdown();
}

void GraphicsInterfaceRenderEngine::ProcessInput() {
}

Bool GraphicsInterfaceRenderEngine::InitScene() {
    auto texture = GraphicsInterfaceResourceManager::getInstance().LoadTexture("example", "./resource/texture/example.png");
    auto shader = GraphicsInterfaceResourceManager::getInstance().LoadShader("example", "./resource/shader/example.vert", "./resource/shader/example.frag");

    if (!texture || !shader) {
        std::cerr << "Failed to load resources" << std::endl;
        return false;
    }

    auto object = CreateSharedPtr<RenderableObject>();
    object->SetTexture(texture);
    object->SetShader(shader);
    scene.AddObject(object);

    return true;
}

} // namespace JzRE
