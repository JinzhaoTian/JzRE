#pragma once

#include "CommonTypes.h"
#include "GraphicsInterfaceShader.h"
#include "GraphicsInterfaceTexture.h"
#include "GraphicsInterfaceScene.h"
#include "GraphicsInterfaceResourceManager.h"

namespace JzRE {
class GraphicsInterfaceRenderer {
public:
    GraphicsInterfaceRenderer(I32 width, I32 height);
    ~GraphicsInterfaceRenderer();

    void RenderScene(SharedPtr<GraphicsInterfaceScene> scene);

    Bool AddShader(const String &name, const String &vertexPath, const String &fragmentPath);

private:
    GLuint framebuffer;
    GLuint textureColorbuffer;
    GLuint rboDepthStencil;

    UnorderedMap<String, GraphicsInterfaceShader> shaders;

    void Clear();
    Bool CreateFramebuffer(I32 width, I32 height);
    void CleanFramebuffer();
};
} // namespace JzRE