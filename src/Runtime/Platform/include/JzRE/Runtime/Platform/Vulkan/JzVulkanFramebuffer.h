/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#pragma once

#include <memory>
#include <vector>

#include "JzRE/Runtime/Platform/RHI/JzGPUFramebufferObject.h"

namespace JzRE {

/**
 * @brief Vulkan framebuffer abstraction used by render output and render graph.
 *
 * The backend keeps attachment references and resolves concrete Vulkan render
 * pass/framebuffer objects in the device path when needed.
 */
class JzVulkanFramebuffer : public JzGPUFramebufferObject {
public:
    explicit JzVulkanFramebuffer(const String &debugName = "");
    ~JzVulkanFramebuffer() override;

    void AttachColorTexture(std::shared_ptr<JzGPUTextureObject> texture, U32 attachmentIndex = 0) override;
    void AttachDepthTexture(std::shared_ptr<JzGPUTextureObject> texture) override;
    void AttachDepthStencilTexture(std::shared_ptr<JzGPUTextureObject> texture) override;
    Bool IsComplete() const override;

    /**
     * @brief Color attachment list.
     */
    const std::vector<std::shared_ptr<JzGPUTextureObject>> &GetColorAttachments() const
    {
        return m_colorAttachments;
    }

    /**
     * @brief Depth attachment.
     */
    std::shared_ptr<JzGPUTextureObject> GetDepthAttachment() const
    {
        return m_depthAttachment;
    }

    /**
     * @brief Depth-stencil attachment.
     */
    std::shared_ptr<JzGPUTextureObject> GetDepthStencilAttachment() const
    {
        return m_depthStencilAttachment;
    }

private:
    std::vector<std::shared_ptr<JzGPUTextureObject>> m_colorAttachments;
    std::shared_ptr<JzGPUTextureObject>              m_depthAttachment;
    std::shared_ptr<JzGPUTextureObject>              m_depthStencilAttachment;
};

} // namespace JzRE
