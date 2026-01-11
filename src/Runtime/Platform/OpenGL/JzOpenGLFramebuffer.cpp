/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzOpenGLFramebuffer.h"
#include "JzOpenGLTexture.h"

JzRE::JzOpenGLFramebuffer::JzOpenGLFramebuffer(const JzRE::String &debugName) :
    JzGPUFramebufferObject(debugName)
{
    glGenFramebuffers(1, &m_handle);

    if (!debugName.empty() && glObjectLabel != nullptr) {
        glObjectLabel(GL_FRAMEBUFFER, m_handle, -1, debugName.c_str());
    }
}

JzRE::JzOpenGLFramebuffer::~JzOpenGLFramebuffer()
{
    if (m_handle != 0) {
        glDeleteFramebuffers(1, &m_handle);
        m_handle = 0;
    }
}

void JzRE::JzOpenGLFramebuffer::AttachColorTexture(std::shared_ptr<JzRE::JzGPUTextureObject> texture, JzRE::U32 attachmentIndex)
{
    if (!texture) {
        return;
    }

    // Convert RHI texture to OpenGL texture
    auto glTexture = std::dynamic_pointer_cast<JzOpenGLTexture>(texture);
    if (!glTexture) {
        return;
    }

    // Bind framebuffer
    GLint previousFramebuffer;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_handle);

    // Attach color texture
    GLenum attachment = GL_COLOR_ATTACHMENT0 + attachmentIndex;
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, glTexture->GetTarget(), (GLuint)(uintptr_t)glTexture->GetTextureID(), 0);

    // Store attached texture
    if (attachmentIndex >= m_colorAttachments.size()) {
        m_colorAttachments.resize(attachmentIndex + 1);
    }
    m_colorAttachments[attachmentIndex] = texture;

    // Restore previous framebuffer binding
    glBindFramebuffer(GL_FRAMEBUFFER, previousFramebuffer);
}

void JzRE::JzOpenGLFramebuffer::AttachDepthTexture(std::shared_ptr<JzRE::JzGPUTextureObject> texture)
{
    if (!texture) {
        return;
    }

    // Convert RHI texture to OpenGL texture
    auto glTexture = std::dynamic_pointer_cast<JzOpenGLTexture>(texture);
    if (!glTexture) {
        return;
    }

    // Bind framebuffer
    GLint previousFramebuffer;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_handle);

    // Attach depth texture
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, glTexture->GetTarget(), (GLuint)(uintptr_t)glTexture->GetTextureID(), 0);

    // Store attached texture
    m_depthAttachment = texture;

    // Restore previous framebuffer binding
    glBindFramebuffer(GL_FRAMEBUFFER, previousFramebuffer);
}

void JzRE::JzOpenGLFramebuffer::AttachDepthStencilTexture(std::shared_ptr<JzRE::JzGPUTextureObject> texture)
{
    if (!texture) {
        return;
    }

    // Convert RHI texture to OpenGL texture
    auto glTexture = std::dynamic_pointer_cast<JzOpenGLTexture>(texture);
    if (!glTexture) {
        return;
    }

    // Bind framebuffer
    GLint previousFramebuffer;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_handle);

    // Attach depth stencil texture
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, glTexture->GetTarget(), (GLuint)(uintptr_t)glTexture->GetTextureID(), 0);

    // Store attached texture
    m_depthStencilAttachment = texture;

    // Restore previous framebuffer binding
    glBindFramebuffer(GL_FRAMEBUFFER, previousFramebuffer);
}

JzRE::Bool JzRE::JzOpenGLFramebuffer::IsComplete() const
{
    // Bind framebuffer
    GLint previousFramebuffer;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_handle);

    // Check framebuffer completeness
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    // Restore previous framebuffer binding
    glBindFramebuffer(GL_FRAMEBUFFER, previousFramebuffer);

    return status == GL_FRAMEBUFFER_COMPLETE;
}

GLuint JzRE::JzOpenGLFramebuffer::GetHandle() const
{
    return m_handle;
}
