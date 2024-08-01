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

    void SetViewMatrix(const glm::mat4 &viewMatrix);
    void SetProjectionMatrix(const glm::mat4 &projectionMatrix);

private:
    GLuint framebuffer;
    GLuint textureColorbuffer;
    GLuint rboDepthStencil;

    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

    SharedPtr<GraphicsInterfaceShader> shader;
    UnorderedMap<String, SharedPtr<GraphicsInterfaceTexture>> textures;

    void Clear();
    Bool CreateFramebuffer(I32 width, I32 height);
    void CleanFramebuffer();
};
} // namespace JzRE