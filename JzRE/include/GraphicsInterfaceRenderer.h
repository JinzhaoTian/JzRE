#pragma once

#include "CommonTypes.h"
#include "GraphicsInterfaceShader.h"
#include "GraphicsInterfaceScene.h"

namespace JzRE {
class GraphicsInterfaceRenderer {
public:
    GraphicsInterfaceRenderer();
    ~GraphicsInterfaceRenderer();

    Bool Initialize(I32 width, I32 height);
    void Shutdown();

    void Render(const GraphicsInterfaceScene &scene) const;

    void Clear();

private:
    GLuint framebuffer;
    GLuint textureColorbuffer;
    GLuint rboDepthStencil;

    Bool CreateFramebuffer(I32 width, I32 height);
    void CleanFramebuffer();
};
} // namespace JzRE