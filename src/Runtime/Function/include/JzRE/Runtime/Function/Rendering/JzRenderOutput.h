/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzVector.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUFramebufferObject.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUTextureObject.h"

namespace JzRE {

/**
 * @brief Concrete render output resource for UI presentation and offscreen rendering.
 *
 * This class owns framebuffer/color/depth GPU resources and exposes
 * a UI-friendly texture handle for panels/widgets.
 */
class JzRenderOutput {
public:
    /**
     * @brief Constructor.
     *
     * @param debugName Debug label prefix for GPU resources.
     */
    explicit JzRenderOutput(const String &debugName = "RenderOutput");

    /**
     * @brief Destructor.
     */
    ~JzRenderOutput();

    /**
     * @brief Ensure output resources match the requested size.
     *
     * @param size Target size.
     * @return true if resources were recreated.
     */
    Bool EnsureSize(JzIVec2 size);

    /**
     * @brief Get framebuffer resource.
     */
    std::shared_ptr<JzGPUFramebufferObject> GetFramebuffer() const;

    /**
     * @brief Get color texture resource.
     */
    std::shared_ptr<JzGPUTextureObject> GetColorTexture() const;

    /**
     * @brief Get depth texture resource.
     */
    std::shared_ptr<JzGPUTextureObject> GetDepthTexture() const;

    /**
     * @brief Get native texture ID for UI consumption.
     *
     * @return Native texture ID (e.g. GLuint cast to void*).
     */
    void *GetTextureID() const;

    /**
     * @brief Get current output size.
     *
     * @return The output size
     */
    JzIVec2 GetSize() const;

    /**
     * @brief Check if the output is valid and ready for use.
     *
     * @return true if the output is valid
     */
    Bool IsValid() const;

private:
    /**
     * @brief Allocate framebuffer and attachments for current size.
     */
    Bool CreateResources();

    /**
     * @brief Release framebuffer and attachments.
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
