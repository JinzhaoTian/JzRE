/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>
#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Platform/JzGPUTextureObject.h"

namespace JzRE {

/**
 * @brief Interface of GPU Framebuffer Object
 */
class JzGPUFramebufferObject : public JzGPUResource {
public:
    /**
     * @brief Constructor
     *
     * @param debugName The debug name of the framebuffer
     */
    JzGPUFramebufferObject(const String &debugName = "") :
        JzGPUResource(debugName) { }

    /**
     * @brief Destructor
     */
    virtual ~JzGPUFramebufferObject() = default;

    /**
     * @brief Attach a color texture to the framebuffer
     *
     * @param texture The texture to attach
     * @param attachmentIndex The attachment index
     */
    virtual void AttachColorTexture(std::shared_ptr<JzGPUTextureObject> texture, U32 attachmentIndex = 0) = 0;

    /**
     * @brief Attach a depth texture to the framebuffer
     */
    virtual void AttachDepthTexture(std::shared_ptr<JzGPUTextureObject> texture) = 0;

    /**
     * @brief Attach a depth stencil texture to the framebuffer
     *
     * @param texture The texture to attach
     */
    virtual void AttachDepthStencilTexture(std::shared_ptr<JzGPUTextureObject> texture) = 0;

    /**
     * @brief Check if the framebuffer is complete
     *
     * @return True if the framebuffer is complete, false otherwise
     */
    virtual Bool IsComplete() const = 0;
};

} // namespace JzRE