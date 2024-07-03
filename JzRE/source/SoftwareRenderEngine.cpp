#include "SoftwareRenderEngine.h"

namespace JzRE {
SoftwareRenderEngine::SoftwareRenderEngine() {
    window = CreateSharedPtr<SoftwareRenderWindow>(wndWidth, wndHeight, title);
    renderer = CreateSharedPtr<Renderer>(wndWidth, wndHeight);

    renderer->BindFrambuffer(window->backBuffer);
}

void SoftwareRenderEngine::Render() {
    while (!window->ShouldClose()) {
        renderer->DrawLine(0, 0, 100, 500, Color::white);

        window->SwapBuffer();
        window->PollEvents();

        Sleep(1000);
    }
}

} // namespace JzRE