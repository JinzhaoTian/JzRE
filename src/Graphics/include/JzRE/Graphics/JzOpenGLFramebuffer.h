/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <glad/glad.h>
#include "JzRE/Core/JzRETypes.h"
#include "JzRE/RHI/JzRHIFramebuffer.h"
#include "JzRE/RHI/JzRHITexture.h"

namespace JzRE {
/**
 * @brief OpenGL Implementation of RHI Framebuffer
 */
class JzOpenGLFramebuffer : public JzRHIFramebuffer {
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
    void AttachColorTexture(std::shared_ptr<JzRHITexture> texture, U32 attachmentIndex = 0) override;

    /**
     * @brief Attach a depth texture to the framebuffer
     * @param texture The texture to attach
     */
    void AttachDepthTexture(std::shared_ptr<JzRHITexture> texture) override;

    /**
     * @brief Attach a depth stencil texture to the framebuffer
     * @param texture The texture to attach
     */
    void AttachDepthStencilTexture(std::shared_ptr<JzRHITexture> texture) override;

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
    GLuint                                     m_handle = 0;
    std::vector<std::shared_ptr<JzRHITexture>> m_colorAttachments;
    std::shared_ptr<JzRHITexture>              m_depthAttachment;
    std::shared_ptr<JzRHITexture>              m_depthStencilAttachment;
};
} // namespace JzRE