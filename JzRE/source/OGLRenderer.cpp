#include "OGLRenderer.h"

namespace JzRE {

RawPtr<OGLRenderer> OGLRenderer::instance = nullptr;

OGLRenderer::OGLRenderer(SharedPtr<OGLRenderWindow> wnd, I32 width, I32 height) {
    instance = this;

    // callback: frame buffer size
    glfwSetFramebufferSizeCallback(wnd->GetGLFWwindow(), callback_framebuffer_size);

    // configure framebuffer
    if (!this->CreateFramebuffer(width, height)) {
        throw std::runtime_error("Failed to initialize framebuffer");
    }

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);

    // configure shaders
    if (!this->AddShader("example", "./resources/shaders/example.vert", "./resources/shaders/example.frag")) {
        throw std::runtime_error("Failed to load resources");
    }
}

OGLRenderer::~OGLRenderer() {
    this->CleanFramebuffer();
}

void OGLRenderer::RenderScene(SharedPtr<OGLScene> scene) {
    this->Clear();

    this->shader->Use();

    // camera properties
    if (scene->GetCamera()) {
        this->shader->SetUniform("view", scene->GetCamera()->GetViewMatrix());
        this->shader->SetUniform("projection", scene->GetCamera()->GetProjectionMatrix());
        this->shader->SetUniform("viewPos", scene->GetCamera()->GetCameraPosition());
    }

    // light properties
    for (Size i = 0; i < scene->GetLights().size(); ++i) {
        scene->GetLights()[i]->ApplyLight(this->shader, StaticCast<I32>(i));
    }

    // object properties
    for (const auto object : scene->GetObjects()) {
        this->shader->SetUniform("model", object->GetModelMatrix());
        object->Draw();
    }

    for (const auto model : scene->GetModels()) {
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        this->shader->SetUniform("model", modelMatrix);
        model->Draw(this->shader);
    }
}

Bool OGLRenderer::AddShader(const String &name, const String &vertexPath, const String &fragmentPath) {
    this->shader = OGLResourceManager::getInstance()
                       .LoadShader(name, vertexPath, fragmentPath);
    return true;
}

void OGLRenderer::Clear() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

Bool OGLRenderer::CreateFramebuffer(I32 width, I32 height) {
    // create framebuffer object
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // create a color attachment texture
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

    // create a renderbuffer object for depth and stencil attachment
    glGenRenderbuffers(1, &rboDepthStencil);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepthStencil);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepthStencil);

    // to check if framebuffer is actually complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer is not complete!" << std::endl;
        return false;
    }

    // unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

void OGLRenderer::CleanFramebuffer() {
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

void OGLRenderer::callback_framebuffer_size(RawPtr<GLFWwindow> window, int width, int height) {
    auto wnd = StaticCast<RawPtr<OGLRenderWindow>>(glfwGetWindowUserPointer(window));
    wnd->ResizeWindow(width, height);

    instance->CleanFramebuffer();

    if (!instance->CreateFramebuffer(width, height)) {
        throw std::runtime_error("Failed to initialize framebuffer");
    }
}

} // namespace JzRE