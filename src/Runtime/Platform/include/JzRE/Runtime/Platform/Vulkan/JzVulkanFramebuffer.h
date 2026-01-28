/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Platform/RHI/JzGPUFramebufferObject.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUTextureObject.h"

namespace JzRE {
/**
 * @brief Vulkan Framebuffer Implementation (Stub)
 */
class JzVulkanFramebuffer : public JzGPUFramebufferObject {
public:
    JzVulkanFramebuffer(const String &debugName = "");
    ~JzVulkanFramebuffer() override;

    void AttachColorTexture(std::shared_ptr<JzGPUTextureObject> texture, U32 attachmentIndex = 0) override;
    void AttachDepthTexture(std::shared_ptr<JzGPUTextureObject> texture) override;
    void AttachDepthStencilTexture(std::shared_ptr<JzGPUTextureObject> texture) override;
    Bool IsComplete() const override;

private:
    // VkFramebuffer framebuffer;
    // VkRenderPass renderPass;
};
} // namespace JzRE