/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzVector.h"
#include "JzRE/Runtime/Function/Rendering/JzRenderOutput.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUFramebufferObject.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUTextureObject.h"

namespace JzRE {

/**
 * @brief Manages framebuffer and textures for a single view panel.
 *
 * This class encapsulates the GPU resources needed to render to an offscreen
 * target that can be displayed in an ImGui view panel via ImGui::Image().
 */
class JzRenderTarget : public JzRenderOutput,
                       public std::enable_shared_from_this<JzRenderTarget> {
public:
    /**
     * @brief Constructor
     *
     * @param debugName Debug name for GPU resources
     */
    explicit JzRenderTarget(const String &debugName = "RenderTarget");

    /**
     * @brief Destructor
     */
    ~JzRenderTarget();

    /**
     * @brief Ensure the render target matches the requested size.
     *
     * Creates or recreates GPU resources if the size has changed.
     *
     * @param size The desired size
     * @return true if resize occurred and resources were recreated
     */
    Bool EnsureSize(JzIVec2 size);

    /**
     * @brief Get the framebuffer for rendering.
     *
     * @return The framebuffer object
     */
    std::shared_ptr<JzGPUFramebufferObject> GetFramebuffer() const;

    /**
     * @brief Get the color texture.
     *
     * @return The color texture object
     */
    std::shared_ptr<JzGPUTextureObject> GetColorTexture() const;

    /**
     * @brief Get the depth texture.
     *
     * @return The depth texture object
     */
    std::shared_ptr<JzGPUTextureObject> GetDepthTexture() const;

    /**
     * @brief Get the native texture ID for ImGui::Image().
     *
     * @return The native texture ID (e.g., GLuint cast to void*)
     */
    void *GetTextureID() const override;

    /**
     * @brief Get current size.
     *
     * @return The current render target size
     */
    JzIVec2 GetSize() const override;

    /**
     * @brief Check if the render target is valid and ready for use.
     *
     * @return true if framebuffer and textures are created
     */
    Bool IsValid() const override;

private:
    /**
     * @brief Create GPU resources (framebuffer, color texture, depth texture).
     *
     * @return true if creation succeeded
     */
    Bool CreateResources();

    /**
     * @brief Destroy GPU resources.
     */
    void DestroyResources();

private:
    String  m_debugName;
    JzIVec2 m_size{0, 0};

    std::shared_ptr<JzGPUFramebufferObject> m_framebuffer;
    std::shared_ptr<JzGPUTextureObject>     m_colorTexture;
    std::shared_ptr<JzGPUTextureObject>     m_depthTexture;
};

} // namespace JzRE
