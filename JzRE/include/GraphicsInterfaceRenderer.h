#pragma once

#include "CommonTypes.h"
#include "GraphicsInterfaceShader.h"
#include "GraphicsInterfaceScene.h"
#include "RenderableObject.h"

namespace JzRE {
class GraphicsInterfaceRenderer {
public:
    GraphicsInterfaceRenderer();
    ~GraphicsInterfaceRenderer();

    Bool Initialize();
    void Shutdown();

    void Render(const GraphicsInterfaceScene &scene);
    void RenderObject(const RenderableObject &object) const;

    void Clear();

private:
};
} // namespace JzRE