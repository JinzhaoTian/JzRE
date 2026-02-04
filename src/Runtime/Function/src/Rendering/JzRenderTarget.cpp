/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Function/Rendering/JzRenderTarget.h"

#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Runtime/Platform/RHI/JzDevice.h"

namespace JzRE {

JzRenderTarget::JzRenderTarget(const String &debugName) :
    m_debugName(debugName) { }

JzRenderTarget::~JzRenderTarget()
{
    DestroyResources();
}

Bool JzRenderTarget::EnsureSize(JzIVec2 size)
{
    // Skip if size is invalid
    if (size.x <= 0 || size.y <= 0) {
        return false;
    }

    // Skip if size hasn't changed and resources exist
    if (m_size == size && IsValid()) {
        return false;
    }

    // Update size and recreate resources
    m_size = size;
    return CreateResources();
}

std::shared_ptr<JzGPUFramebufferObject> JzRenderTarget::GetFramebuffer() const
{
    return m_framebuffer;
}

std::shared_ptr<JzGPUTextureObject> JzRenderTarget::GetColorTexture() const
{
    return m_colorTexture;
}

std::shared_ptr<JzGPUTextureObject> JzRenderTarget::GetDepthTexture() const
{
    return m_depthTexture;
}

void *JzRenderTarget::GetTextureID() const
{
    if (m_colorTexture) {
        return m_colorTexture->GetTextureID();
    }
    return nullptr;
}

JzIVec2 JzRenderTarget::GetSize() const
{
    return m_size;
}

Bool JzRenderTarget::IsValid() const
{
    return m_framebuffer != nullptr && m_colorTexture != nullptr && m_depthTexture != nullptr;
}

Bool JzRenderTarget::CreateResources()
{
    if (!JzServiceContainer::Has<JzDevice>()) {
        return false;
    }

    auto &device = JzServiceContainer::Get<JzDevice>();

    // Create framebuffer
    m_framebuffer = device.CreateFramebuffer(m_debugName + "_FB");
    if (!m_framebuffer) {
        return false;
    }

    // Create color texture
    JzGPUTextureObjectDesc colorDesc;
    colorDesc.type      = JzETextureResourceType::Texture2D;
    colorDesc.format    = JzETextureResourceFormat::RGBA8;
    colorDesc.width     = static_cast<U32>(m_size.x);
    colorDesc.height    = static_cast<U32>(m_size.y);
    colorDesc.debugName = m_debugName + "_Color";
    m_colorTexture      = device.CreateTexture(colorDesc);

    if (m_colorTexture) {
        m_framebuffer->AttachColorTexture(m_colorTexture, 0);
    }

    // Create depth texture
    JzGPUTextureObjectDesc depthDesc;
    depthDesc.type      = JzETextureResourceType::Texture2D;
    depthDesc.format    = JzETextureResourceFormat::Depth24;
    depthDesc.width     = static_cast<U32>(m_size.x);
    depthDesc.height    = static_cast<U32>(m_size.y);
    depthDesc.debugName = m_debugName + "_Depth";
    m_depthTexture      = device.CreateTexture(depthDesc);

    if (m_depthTexture) {
        m_framebuffer->AttachDepthTexture(m_depthTexture);
    }

    return IsValid();
}

void JzRenderTarget::DestroyResources()
{
    m_depthTexture.reset();
    m_colorTexture.reset();
    m_framebuffer.reset();
}

} // namespace JzRE
