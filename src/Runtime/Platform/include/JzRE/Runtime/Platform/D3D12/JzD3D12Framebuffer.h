/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#pragma once

#include <vector>

#include "JzRE/Runtime/Platform/RHI/JzGPUFramebufferObject.h"

namespace JzRE {

#if defined(_WIN32)

/**
 * @brief Direct3D 12 framebuffer abstraction.
 */
class JzD3D12Framebuffer final : public JzGPUFramebufferObject {
public:
    explicit JzD3D12Framebuffer(const String &debugName = "");
    ~JzD3D12Framebuffer() override = default;

    void AttachColorTexture(std::shared_ptr<JzGPUTextureObject> texture, U32 attachmentIndex = 0) override;
    void AttachDepthTexture(std::shared_ptr<JzGPUTextureObject> texture) override;
    void AttachDepthStencilTexture(std::shared_ptr<JzGPUTextureObject> texture) override;
    Bool IsComplete() const override;

    const std::vector<std::shared_ptr<JzGPUTextureObject>> &GetColorAttachments() const;
    std::shared_ptr<JzGPUTextureObject>                     GetDepthAttachment() const;
    std::shared_ptr<JzGPUTextureObject>                     GetDepthStencilAttachment() const;

private:
    std::vector<std::shared_ptr<JzGPUTextureObject>> m_colorAttachments;
    std::shared_ptr<JzGPUTextureObject>              m_depthAttachment;
    std::shared_ptr<JzGPUTextureObject>              m_depthStencilAttachment;
};

#endif // _WIN32

} // namespace JzRE
