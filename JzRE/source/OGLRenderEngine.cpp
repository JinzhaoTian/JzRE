#include "OGLRenderEngine.h"

namespace JzRE {
OGLRenderEngine::OGLRenderEngine() :
    isRunning(false) {
}

OGLRenderEngine::~OGLRenderEngine() {
    Shutdown();
}

Bool OGLRenderEngine::Initialize() {
    this->window = std::make_shared<OGLRenderWindow>(this->wndWidth, this->wndHeight, this->title);
    if (this->window == nullptr) {
        return false;
    }

    this->renderer = std::make_shared<OGLRenderer>(this->window, this->wndWidth, this->wndHeight);
    if (this->renderer == nullptr) {
        return false;
    }

    OGLInput::Initialize(this->window->GetGLFWwindow());

    this->simpleUI = std::make_shared<OGLUI>(this->window->GetGLFWwindow());
    if (this->simpleUI == nullptr) {
        return false;
    }

    this->camera = std::make_shared<OGLCamera>(this->wndWidth, this->wndHeight, glm::vec3(0.0f, 0.0f, 3.0f));
    if (this->camera == nullptr) {
        return false;
    }

    this->scene = std::make_shared<OGLScene>();
    if (this->scene == nullptr) {
        return false;
    }

    Bool isSceneInited = InitScene();
    if (!isSceneInited) {
        return false;
    }

    this->scene->SetCamera(this->camera);

    this->simpleUI->SetCallbackOpenFile([&](const String &filePath) {
        auto model = std::make_shared<OGLModel>(filePath, false);
        this->scene->AddModel(model);
    });

    this->isRunning = true;
    return true;
}

void OGLRenderEngine::Run() {
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

        this->simpleUI->Render();

        this->window->SwapFramebuffer();
    }
}

void OGLRenderEngine::Shutdown() {
    OGLResourceManager::getInstance().Clear();
}

void OGLRenderEngine::ProcessInput() {
    if (OGLInput::IsKeyPressed(GLFW_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(this->window->GetGLFWwindow(), true);
    }

    if (OGLInput::IsKeyPressed(GLFW_KEY_SPACE)) {
        this->camera->ResetPosition(glm::vec3(0.0f, 0.0f, 3.0f));
    }

    if (OGLInput::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
        this->camera->ProcessMouseMovement(OGLInput::GetMouseMovement());
    }

    if (OGLInput::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
        this->camera->ProcessKeyboardMovement(OGLInput::GetMouseMovement());
    }

    this->camera->ProcessMouseScroll(OGLInput::GetMouseScroll());
}

Bool OGLRenderEngine::InitScene() {
    auto light = std::make_shared<OGLDirectionalLight>(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f));
    scene->AddLight(light);

    return true;
}

} // namespace JzRE
