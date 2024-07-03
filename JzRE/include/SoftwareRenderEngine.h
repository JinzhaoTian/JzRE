#pragma once

#include "CommonTypes.h"
#include "SoftwareRenderWindow.h"
#include "Renderer.h"

namespace JzRE {
class SoftwareRenderEngine {
public:
    SoftwareRenderEngine();
    void Render();

private:
    const int wndWidth = 800;
    const int wndHeight = 600;

    const String title = "Software/CPU Render Engine";

    SharedPtr<SoftwareRenderWindow> window;
    SharedPtr<Renderer> renderer;
};
} // namespace JzRE