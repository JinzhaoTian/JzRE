#include "SoftwareRenderEngine.h"

namespace JzRE {
SoftwareRenderEngine::SoftwareRenderEngine() :
    isRunning(false) {
}

SoftwareRenderEngine::~SoftwareRenderEngine() {
    Shutdown();
}

bool SoftwareRenderEngine::Initialize() {
    bool isWndCreated = window.Initialize(this->wndWidth, this->wndHeight, this->title);
    if (!isWndCreated) {
        return false;
    }

    // TODO: init
    renderer = Renderer(wndWidth, wndHeight);
    renderer.BindFrambuffer(window.backBuffer);

    isRunning = true;
    return true;
}

void SoftwareRenderEngine::Run() {
    while (!window.ShouldClose()) {
        renderer.DrawLine(0, 0, 100, 500, Color::white);

        window.SwapBuffer();
        window.PollEvents();

        Sleep(1000);
    }
}
void SoftwareRenderEngine::Shutdown() {
    // TODO: 释放资源
}

} // namespace JzRE