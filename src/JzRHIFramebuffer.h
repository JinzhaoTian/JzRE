#pragma once

#include "JzRETypes.h"
#include "JzRHIDescription.h"
#include "JzRHITexture.h"

#include <memory>

namespace JzRE {
/**
 * @brief Interface of RHI Framebuffer
 */
class JzRHIFramebuffer : public JzRHIResource {
public:
    /**
     * @brief Constructor
     * @param debugName The debug name of the framebuffer
     */
    JzRHIFramebuffer(const String &debugName = "") :
        JzRHIResource(debugName) { }

    /**
     * @brief Destructor
     */
    virtual ~JzRHIFramebuffer() = default;

    /**
     * @brief Attach a color texture to the framebuffer
     * @param texture The texture to attach
     * @param attachmentIndex The attachment index
     */
    virtual void AttachColorTexture(std::shared_ptr<JzRHITexture> texture, U32 attachmentIndex = 0) = 0;

    /**
     * @brief Attach a depth texture to the framebuffer
     */
    virtual void AttachDepthTexture(std::shared_ptr<JzRHITexture> texture) = 0;

    /**
     * @brief Attach a depth stencil texture to the framebuffer
     * @param texture The texture to attach
     */
    virtual void AttachDepthStencilTexture(std::shared_ptr<JzRHITexture> texture) = 0;

    /**
     * @brief Check if the framebuffer is complete
     * @return True if the framebuffer is complete, false otherwise
     */
    virtual Bool IsComplete() const = 0;
};

} // namespace JzRE