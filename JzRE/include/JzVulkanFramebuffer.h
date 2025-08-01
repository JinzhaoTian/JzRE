#pragma once

#include "JzRHIFramebuffer.h"
#include "JzRHITexture.h"

namespace JzRE {
/**
 * @brief Vulkan Framebuffer Implementation (Stub)
 */
class JzVulkanFramebuffer : public JzRHIFramebuffer {
public:
    JzVulkanFramebuffer(const String &debugName = "");
    ~JzVulkanFramebuffer() override;

    void AttachColorTexture(std::shared_ptr<JzRHITexture> texture, U32 attachmentIndex = 0) override;
    void AttachDepthTexture(std::shared_ptr<JzRHITexture> texture) override;
    void AttachDepthStencilTexture(std::shared_ptr<JzRHITexture> texture) override;
    Bool IsComplete() const override;

private:
    // VkFramebuffer framebuffer;
    // VkRenderPass renderPass;
};
} // namespace JzRE