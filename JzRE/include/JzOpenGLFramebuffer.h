#pragma once

#include "CommonTypes.h"
#include "JzRHIETypes.h"
#include "JzRHIFramebuffer.h"
#include "JzRHITexture.h"

namespace JzRE {

/**
 * OpenGL帧缓冲实现
 */
class JzOpenGLFramebuffer : public JzRHIFramebuffer {
public:
    JzOpenGLFramebuffer(const String &debugName = "");
    ~JzOpenGLFramebuffer() override;

    void   AttachColorTexture(std::shared_ptr<JzRHITexture> texture, U32 attachmentIndex = 0) override;
    void   AttachDepthTexture(std::shared_ptr<JzRHITexture> texture) override;
    void   AttachDepthStencilTexture(std::shared_ptr<JzRHITexture> texture) override;
    Bool   IsComplete() const override;
    GLuint GetHandle() const;

private:
    GLuint                                     handle = 0;
    std::vector<std::shared_ptr<JzRHITexture>> colorAttachments;
    std::shared_ptr<JzRHITexture>              depthAttachment;
    std::shared_ptr<JzRHITexture>              depthStencilAttachment;
};
} // namespace JzRE