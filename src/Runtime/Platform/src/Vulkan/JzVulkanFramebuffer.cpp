/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#include "JzRE/Runtime/Platform/Vulkan/JzVulkanFramebuffer.h"

#include <algorithm>

namespace JzRE {

JzVulkanFramebuffer::JzVulkanFramebuffer(const String &debugName) :
    JzGPUFramebufferObject(debugName)
{ }

JzVulkanFramebuffer::~JzVulkanFramebuffer() = default;

void JzVulkanFramebuffer::AttachColorTexture(std::shared_ptr<JzGPUTextureObject> texture, U32 attachmentIndex)
{
    if (!texture) {
        return;
    }

    if (attachmentIndex >= m_colorAttachments.size()) {
        m_colorAttachments.resize(static_cast<Size>(attachmentIndex) + 1U);
    }

    m_colorAttachments[attachmentIndex] = std::move(texture);
}

void JzVulkanFramebuffer::AttachDepthTexture(std::shared_ptr<JzGPUTextureObject> texture)
{
    m_depthAttachment = std::move(texture);
}

void JzVulkanFramebuffer::AttachDepthStencilTexture(std::shared_ptr<JzGPUTextureObject> texture)
{
    m_depthStencilAttachment = std::move(texture);
}

Bool JzVulkanFramebuffer::IsComplete() const
{
    const Bool hasColor = std::any_of(
        m_colorAttachments.begin(),
        m_colorAttachments.end(),
        [](const std::shared_ptr<JzGPUTextureObject> &texture) {
            return texture != nullptr;
        });

    return hasColor || m_depthAttachment != nullptr || m_depthStencilAttachment != nullptr;
}

} // namespace JzRE
