/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Function/Rendering/JzRenderOutput.h"

#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Runtime/Platform/RHI/JzDevice.h"

namespace JzRE {

JzRenderOutput::JzRenderOutput(const String &debugName) :
    m_debugName(debugName) { }

JzRenderOutput::~JzRenderOutput()
{
    DestroyResources();
}

Bool JzRenderOutput::EnsureSize(JzIVec2 size)
{
    if (size.x <= 0 || size.y <= 0) {
        return false;
    }

    if (m_size == size && IsValid()) {
        return false;
    }

    m_size = size;
    return CreateResources();
}

std::shared_ptr<JzGPUFramebufferObject> JzRenderOutput::GetFramebuffer() const
{
    return m_framebuffer;
}

std::shared_ptr<JzGPUTextureObject> JzRenderOutput::GetColorTexture() const
{
    return m_colorTexture;
}

std::shared_ptr<JzGPUTextureObject> JzRenderOutput::GetDepthTexture() const
{
    return m_depthTexture;
}

void *JzRenderOutput::GetTextureID() const
{
    if (m_colorTexture) {
        return m_colorTexture->GetTextureID();
    }
    return nullptr;
}

JzIVec2 JzRenderOutput::GetSize() const
{
    return m_size;
}

Bool JzRenderOutput::IsValid() const
{
    return m_framebuffer != nullptr && m_colorTexture != nullptr && m_depthTexture != nullptr;
}

Bool JzRenderOutput::CreateResources()
{
    if (!JzServiceContainer::Has<JzDevice>()) {
        return false;
    }

    auto &device = JzServiceContainer::Get<JzDevice>();

    m_framebuffer = device.CreateFramebuffer(m_debugName + "_FB");
    if (!m_framebuffer) {
        return false;
    }

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

void JzRenderOutput::DestroyResources()
{
    m_depthTexture.reset();
    m_colorTexture.reset();
    m_framebuffer.reset();
}

} // namespace JzRE
