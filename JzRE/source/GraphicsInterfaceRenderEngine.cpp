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

    GraphicsInterfaceInput::Initialize(this->window.GetGLFWwindow());

    Bool isRendererInited = this->renderer.Initialize(this->wndWidth, this->wndHeight);
    if (!isRendererInited) {
        return false;
    }

    this->camera = CreateSharedPtr<GraphicsInterfaceCamera>(this->wndWidth, this->wndHeight, glm::vec3(0.0f, 0.0f, 3.0f));
    if (this->camera == nullptr) {
        return false;
    }

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

        ProcessInput();

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
    if (GraphicsInterfaceInput::IsKeyPressed(GLFW_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(window.GetGLFWwindow(), true);
    }

    if (GraphicsInterfaceInput::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
        this->camera->ProcessMouseMovement(GraphicsInterfaceInput::GetMouseMovement());
    }

    if (GraphicsInterfaceInput::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
        this->camera->ProcessKeyboardMovement(GraphicsInterfaceInput::GetMouseMovement());
    }

    this->camera->ProcessMouseScroll(GraphicsInterfaceInput::GetMouseScroll());
}

Bool GraphicsInterfaceRenderEngine::InitScene() {
    scene.SetCamera(this->camera);

    auto texture = GraphicsInterfaceResourceManager::getInstance().LoadTexture("example", "./resources/textures/example.png");
    auto shader = GraphicsInterfaceResourceManager::getInstance().LoadShader("example", "./resources/shaders/example.vert", "./resources/shaders/example.frag");

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
