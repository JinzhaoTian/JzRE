/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <vector>
#include <memory>
#include <glad/glad.h>
#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUFramebufferObject.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUTextureObject.h"

namespace JzRE {
/**
 * @brief OpenGL Implementation of RHI Framebuffer
 */
class JzOpenGLFramebuffer : public JzGPUFramebufferObject {
public:
    /**
     * @brief Constructor
     * @param debugName The debug name of the framebuffer
     */
    JzOpenGLFramebuffer(const String &debugName = "");

    /**
     * @brief Destructor
     */
    ~JzOpenGLFramebuffer() override;

    /**
     * @brief Attach a color texture to the framebuffer
     * @param texture The texture to attach
     * @param attachmentIndex The attachment index
     */
    void AttachColorTexture(std::shared_ptr<JzGPUTextureObject> texture, U32 attachmentIndex = 0) override;

    /**
     * @brief Attach a depth texture to the framebuffer
     * @param texture The texture to attach
     */
    void AttachDepthTexture(std::shared_ptr<JzGPUTextureObject> texture) override;

    /**
     * @brief Attach a depth stencil texture to the framebuffer
     * @param texture The texture to attach
     */
    void AttachDepthStencilTexture(std::shared_ptr<JzGPUTextureObject> texture) override;

    /**
     * @brief Check if the framebuffer is complete
     * @return True if the framebuffer is complete, false otherwise
     */
    Bool IsComplete() const override;

    /**
     * @brief Get the handle of the framebuffer
     * @return The handle of the framebuffer
     */
    GLuint GetHandle() const;

private:
    GLuint                                           m_handle = 0;
    std::vector<std::shared_ptr<JzGPUTextureObject>> m_colorAttachments;
    std::shared_ptr<JzGPUTextureObject>              m_depthAttachment;
    std::shared_ptr<JzGPUTextureObject>              m_depthStencilAttachment;
};
} // namespace JzRE