#pragma once

#include "CommonTypes.h"
#include "OGLShader.h"
#include "OGLTexture.h"
#include "OGLScene.h"
#include "OGLResourceManager.h"
#include "OGLRenderWindow.h"

namespace JzRE {
class OGLRenderer {
public:
    OGLRenderer(std::shared_ptr<OGLRenderWindow> wnd, I32 width, I32 height);
    ~OGLRenderer();

    void RenderScene(std::shared_ptr<OGLScene> scene);

private:
    GLuint framebuffer;
    GLuint textureColorbuffer;
    GLuint rboDepthStencil;

    std::shared_ptr<OGLShader> shader;
    Bool AddShader(const String &name, const String &vertexPath, const String &fragmentPath);

    void Clear();
    Bool CreateFramebuffer(I32 width, I32 height);
    void CleanFramebuffer();

    static OGLRenderer* instance;
    static void callback_framebuffer_size(GLFWwindow* window, int width, int height);
};
} // namespace JzRE