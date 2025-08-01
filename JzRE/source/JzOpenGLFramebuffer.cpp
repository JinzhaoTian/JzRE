#include "JzOpenGLFramebuffer.h"

JzRE::JzOpenGLFramebuffer::JzOpenGLFramebuffer(const JzRE::String &debugName) { }

JzRE::JzOpenGLFramebuffer::~JzOpenGLFramebuffer() { }

void JzRE::JzOpenGLFramebuffer::AttachColorTexture(std::shared_ptr<JzRE::JzRHITexture> texture, U32 attachmentIndex) { }

void JzRE::JzOpenGLFramebuffer::AttachDepthTexture(std::shared_ptr<JzRE::JzRHITexture> texture) { }

void JzRE::JzOpenGLFramebuffer::AttachDepthStencilTexture(std::shared_ptr<JzRE::JzRHITexture> texture) { }

JzRE::Bool JzRE::JzOpenGLFramebuffer::IsComplete() const { }

GLuint JzRE::JzOpenGLFramebuffer::GetHandle() const
{
    return handle;
}