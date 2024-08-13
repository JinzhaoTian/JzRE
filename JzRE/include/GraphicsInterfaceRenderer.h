#pragma once

#include "CommonTypes.h"
#include "GraphicsInterfaceShader.h"
#include "GraphicsInterfaceTexture.h"
#include "GraphicsInterfaceScene.h"
#include "GraphicsInterfaceResourceManager.h"
#include "GraphicsInterfaceRenderWindow.h"

namespace JzRE {
class GraphicsInterfaceRenderer {
public:
    GraphicsInterfaceRenderer(SharedPtr<GraphicsInterfaceRenderWindow> wnd, I32 width, I32 height);
    ~GraphicsInterfaceRenderer();

    void RenderScene(SharedPtr<GraphicsInterfaceScene> scene);

private:
    GLuint framebuffer;
    GLuint textureColorbuffer;
    GLuint rboDepthStencil;

    SharedPtr<GraphicsInterfaceShader> shader;
    Bool AddShader(const String &name, const String &vertexPath, const String &fragmentPath);

    void Clear();
    Bool CreateFramebuffer(I32 width, I32 height);
    void CleanFramebuffer();

    static RawPtr<GraphicsInterfaceRenderer> instance;
    static void callback_framebuffer_size(RawPtr<GLFWwindow> window, int width, int height);
};
} // namespace JzRE