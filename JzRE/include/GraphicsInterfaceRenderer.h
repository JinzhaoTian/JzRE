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

    void SetShader(SharedPtr<GraphicsInterfaceShader> shader);
    SharedPtr<GraphicsInterfaceShader> GetShader() const;

    void SetTexture(SharedPtr<GraphicsInterfaceTexture> texture);
    SharedPtr<GraphicsInterfaceTexture> GetTexture() const;

private:
    GLuint framebuffer;
    GLuint textureColorbuffer;
    GLuint rboDepthStencil;

    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

    SharedPtr<GraphicsInterfaceShader> shader;
    SharedPtr<GraphicsInterfaceTexture> texture;

    void Clear();
    Bool CreateFramebuffer(I32 width, I32 height);
    void CleanFramebuffer();
};
} // namespace JzRE