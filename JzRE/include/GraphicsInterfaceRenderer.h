#pragma once

#include "CommonTypes.h"
#include "GraphicsInterfaceShader.h"
#include "GraphicsInterfaceScene.h"

namespace JzRE {
class GraphicsInterfaceRenderer {
public:
    GraphicsInterfaceRenderer();
    ~GraphicsInterfaceRenderer();

    Bool Initialize();
    void Shutdown();

    void Render(const GraphicsInterfaceScene &scene);
    void Clear();

private:
    GraphcsInterfaceShader shader;
};
} // namespace JzRE