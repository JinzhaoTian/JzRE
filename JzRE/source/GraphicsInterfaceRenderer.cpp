#include "GraphicsInterfaceRenderer.h"

namespace JzRE {
GraphicsInterfaceRenderer::GraphicsInterfaceRenderer(I32 width, I32 height) {
    // 初始化帧缓冲对象
    if (!CreateFramebuffer(width, height)) {
        std::cerr << "Failed to initialize framebuffer" << std::endl;
        return;
    }

    // 配置 OpenGL 状态
    glEnable(GL_DEPTH_TEST);

    this->shader = GraphicsInterfaceResourceManager::getInstance()
                       .LoadShader("example", "./resources/shaders/example.vert", "./resources/shaders/example.frag");
    this->texture = GraphicsInterfaceResourceManager::getInstance()
                        .LoadTexture("example", "./resources/textures/example.png");

    if (!this->texture || !this->shader) {
        std::cerr << "Failed to load resources" << std::endl;
        return;
    }
}

GraphicsInterfaceRenderer::~GraphicsInterfaceRenderer() {
    CleanFramebuffer();
}

void GraphicsInterfaceRenderer::RenderScene(SharedPtr<GraphicsInterfaceScene> scene) {
    Clear();

    if (scene->GetCamera()) {
        SetViewMatrix(scene->GetCamera()->GetViewMatrix());
        SetProjectionMatrix(scene->GetCamera()->GetProjectionMatrix());
    }

    for (Size i = 0; i < scene->GetLights().size(); ++i) {
        scene->GetLights()[i]->ApplyLight(this->shader, StaticCast<I32>(i));
    }

    this->shader->Use();

    // 设置纹理
    if (this->texture) {
        texture->Bind(0);
        shader->SetUniform("texture1", 0);
    }

    this->shader->SetUniform("view", this->viewMatrix);
    this->shader->SetUniform("projection", this->projectionMatrix);

    for (const auto object : scene->GetObjects()) {
        // 设置模型矩阵
        shader->SetUniform("model", object->GetModelMatrix());
        object->Draw();
    }
}

void GraphicsInterfaceRenderer::SetViewMatrix(const glm::mat4 &viewMatrix) {
    this->viewMatrix = viewMatrix;
}

void GraphicsInterfaceRenderer::SetProjectionMatrix(const glm::mat4 &projectionMatrix) {
    this->projectionMatrix = projectionMatrix;
}

void GraphicsInterfaceRenderer::SetShader(SharedPtr<GraphicsInterfaceShader> shader) {
    this->shader = shader;
}

SharedPtr<GraphicsInterfaceShader> GraphicsInterfaceRenderer::GetShader() const {
    return this->shader;
}

void GraphicsInterfaceRenderer::SetTexture(SharedPtr<GraphicsInterfaceTexture> texture) {
    this->texture = texture;
}

SharedPtr<GraphicsInterfaceTexture> GraphicsInterfaceRenderer::GetTexture() const {
    return this->texture;
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