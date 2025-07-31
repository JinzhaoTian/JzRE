#pragma once

#include "CommonTypes.h"
#include "JzRHITexture.h"

namespace JzRE {
/**
 * RHI帧缓冲接口
 */
class JzRHIFramebuffer : public JzRHIResource {
public:
    JzRHIFramebuffer(const String &debugName = "") :
        JzRHIResource(debugName) { }
    virtual ~JzRHIFramebuffer() = default;

    virtual void AttachColorTexture(std::shared_ptr<JzRHITexture> texture, U32 attachmentIndex = 0) = 0;
    virtual void AttachDepthTexture(std::shared_ptr<JzRHITexture> texture)                          = 0;
    virtual void AttachDepthStencilTexture(std::shared_ptr<JzRHITexture> texture)                   = 0;
    virtual Bool IsComplete() const                                                                 = 0;
};

} // namespace JzRE