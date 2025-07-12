#include "SoftwareRenderEngine.h"
#include <thread>
#include <chrono>

namespace JzRE {
SoftwareRenderEngine::SoftwareRenderEngine() :
    isRunning(false) {
}

SoftwareRenderEngine::~SoftwareRenderEngine() {
    Shutdown();
}

Bool SoftwareRenderEngine::Initialize() {
    Bool isWndCreated = this->window.Initialize(this->wndWidth, this->wndHeight, this->title);
    if (!isWndCreated) {
        return false;
    }

    // TODO: init
    this->renderer = Renderer(wndWidth, wndHeight);
    this->renderer.BindFrambuffer(window.backBuffer);

    this->isRunning = true;
    return true;
}

void SoftwareRenderEngine::Run() {
    while (this->isRunning && !this->window.ShouldClose()) {
        this->renderer.DrawLine(0, 0, 100, 500, Color::white);

        this->window.SwapBuffer();
        this->window.PollEvents();

        // 使用跨平台的睡眠函数
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void SoftwareRenderEngine::Shutdown() {
    // TODO: 释放资源
}

} // namespace JzRE