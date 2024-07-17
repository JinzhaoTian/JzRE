#include "GraphicsInterfaceRenderer.h"

namespace JzRE {
GraphicsInterfaceRenderer::GraphicsInterfaceRenderer() :
    framebuffer(0), textureColorbuffer(0), rboDepthStencil(0) {
}

GraphicsInterfaceRenderer::~GraphicsInterfaceRenderer() {
    Shutdown();
}

Bool GraphicsInterfaceRenderer::Initialize(I32 width, I32 height) {
    // 初始化帧缓冲对象
    if (!CreateFramebuffer(width, height)) {
        std::cerr << "Failed to initialize framebuffer" << std::endl;
        return false;
    }

    // 配置 OpenGL 状态
    glEnable(GL_DEPTH_TEST);

    return true;
}

void GraphicsInterfaceRenderer::Shutdown() {
    CleanFramebuffer();
}

void GraphicsInterfaceRenderer::Render(const GraphicsInterfaceScene &scene) const {
    scene.Draw();
}

void GraphicsInterfaceRenderer::Clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

Bool GraphicsInterfaceRenderer::CreateFramebuffer(I32 width, I32 height) {
    // 创建帧缓冲对象
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // 创建纹理附件
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

    // 创建深度和模板缓冲附件
    glGenRenderbuffers(1, &rboDepthStencil);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepthStencil);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepthStencil);

    // 检查帧缓冲完整性
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer is not complete!" << std::endl;
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

void GraphicsInterfaceRenderer::CleanFramebuffer() {
    if (framebuffer) {
        glDeleteFramebuffers(1, &framebuffer);
        framebuffer = 0;
    }
    if (textureColorbuffer) {
        glDeleteTextures(1, &textureColorbuffer);
        textureColorbuffer = 0;
    }
    if (rboDepthStencil) {
        glDeleteRenderbuffers(1, &rboDepthStencil);
        rboDepthStencil = 0;
    }
}

} // namespace JzRE