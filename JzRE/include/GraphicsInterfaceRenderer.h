#pragma once

#include "CommonTypes.h"
#include "GraphicsInterfaceShader.h"
#include "GraphicsInterfaceScene.h"

namespace JzRE {
class GraphicsInterfaceRenderer {
public:
    GraphicsInterfaceRenderer(I32 width, I32 height);
    ~GraphicsInterfaceRenderer();

    void BindScene(SharedPtr<GraphicsInterfaceScene> scene);
    void Render();
    void Clear();

private:
    GLuint framebuffer;
    GLuint textureColorbuffer;
    GLuint rboDepthStencil;

    SharedPtr<GraphicsInterfaceScene> scene;

    Bool CreateFramebuffer(I32 width, I32 height);
    void CleanFramebuffer();
};
} // namespace JzRE