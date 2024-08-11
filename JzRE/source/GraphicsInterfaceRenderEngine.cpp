#include "GraphicsInterfaceRenderEngine.h"

namespace JzRE {
GraphicsInterfaceRenderEngine::GraphicsInterfaceRenderEngine() :
    isRunning(false) {
}

GraphicsInterfaceRenderEngine::~GraphicsInterfaceRenderEngine() {
    Shutdown();
}

Bool GraphicsInterfaceRenderEngine::Initialize() {
    this->window = CreateSharedPtr<GraphicsInterfaceRenderWindow>(this->wndWidth, this->wndHeight, this->title);
    if (this->window == nullptr) {
        return false;
    }

    GraphicsInterfaceInput::Initialize(this->window->GetGLFWwindow());

    this->renderer = CreateSharedPtr<GraphicsInterfaceRenderer>(this->wndWidth, this->wndHeight);
    if (this->renderer == nullptr) {
        return false;
    }

    this->camera = CreateSharedPtr<GraphicsInterfaceCamera>(this->wndWidth, this->wndHeight, glm::vec3(0.0f, 0.0f, 3.0f));
    if (this->camera == nullptr) {
        return false;
    }

    this->scene = CreateSharedPtr<GraphicsInterfaceScene>();
    if (this->scene == nullptr) {
        return false;
    }

    Bool isSceneInited = InitScene();
    if (!isSceneInited) {
        return false;
    }

    this->scene->SetCamera(this->camera);

    this->isRunning = true;
    return true;
}

void GraphicsInterfaceRenderEngine::Run() {
    auto previousTime = std::chrono::high_resolution_clock::now();

    // render loop
    while (this->isRunning && !this->window->ShouldClose()) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        F32 deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - previousTime).count();
        previousTime = currentTime;

        this->window->PollEvents();

        ProcessInput();

        this->scene->Update(deltaTime);
        this->renderer->RenderScene(this->scene);

        this->window->SwapFramebuffer();
    }
}

void GraphicsInterfaceRenderEngine::Shutdown() {
    GraphicsInterfaceResourceManager::getInstance().Clear();
}

void GraphicsInterfaceRenderEngine::ProcessInput() {
    if (GraphicsInterfaceInput::IsKeyPressed(GLFW_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(this->window->GetGLFWwindow(), true);
    }

    if (GraphicsInterfaceInput::IsKeyPressed(GLFW_KEY_SPACE)) {
        this->camera->ResetPosition(glm::vec3(0.0f, 0.0f, 3.0f));
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
    // auto object = CreateSharedPtr<RenderableObject>();
    // scene->AddObject(object);

    String modelPath = "./resources/models/nanosuit.obj";
    auto model = CreateSharedPtr<GraphicsInterfaceModel>(modelPath, false);
    scene->AddModel(model);

    // auto light = CreateSharedPtr<GraphicsInterfaceDirectionalLight>(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f));
    // scene->AddLight(light);

    return true;
}

} // namespace JzRE
