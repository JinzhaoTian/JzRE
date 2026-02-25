/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#include "JzRE/Runtime/Platform/D3D12/JzD3D12Framebuffer.h"

#if defined(_WIN32)

#include <algorithm>

namespace JzRE {

JzD3D12Framebuffer::JzD3D12Framebuffer(const String &debugName) :
    JzGPUFramebufferObject(debugName)
{ }

void JzD3D12Framebuffer::AttachColorTexture(std::shared_ptr<JzGPUTextureObject> texture, U32 attachmentIndex)
{
    if (!texture) {
        return;
    }

    if (attachmentIndex >= m_colorAttachments.size()) {
        m_colorAttachments.resize(static_cast<Size>(attachmentIndex) + 1U);
    }

    m_colorAttachments[attachmentIndex] = std::move(texture);
}

void JzD3D12Framebuffer::AttachDepthTexture(std::shared_ptr<JzGPUTextureObject> texture)
{
    m_depthAttachment = std::move(texture);
}

void JzD3D12Framebuffer::AttachDepthStencilTexture(std::shared_ptr<JzGPUTextureObject> texture)
{
    m_depthStencilAttachment = std::move(texture);
}

Bool JzD3D12Framebuffer::IsComplete() const
{
    const Bool hasColor = std::any_of(
        m_colorAttachments.begin(),
        m_colorAttachments.end(),
        [](const std::shared_ptr<JzGPUTextureObject> &tex) {
            return tex != nullptr;
        });

    return hasColor || m_depthAttachment != nullptr || m_depthStencilAttachment != nullptr;
}

const std::vector<std::shared_ptr<JzGPUTextureObject>> &JzD3D12Framebuffer::GetColorAttachments() const
{
    return m_colorAttachments;
}

std::shared_ptr<JzGPUTextureObject> JzD3D12Framebuffer::GetDepthAttachment() const
{
    return m_depthAttachment;
}

std::shared_ptr<JzGPUTextureObject> JzD3D12Framebuffer::GetDepthStencilAttachment() const
{
    return m_depthStencilAttachment;
}

} // namespace JzRE

#endif // _WIN32
