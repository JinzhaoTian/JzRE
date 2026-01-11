/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>
#include "JzRE/Runtime/Platform/JzGPUFramebufferObject.h"
#include "JzRE/Runtime/Platform/JzRHIPipeline.h"
#include "JzRE/Runtime/Platform/JzGPUTextureObject.h"
#include "JzRE/Runtime/Function/Scene/JzScene.h"

namespace JzRE {
/**
 * @brief RHI Renderer, providing cross-platform rendering support
 */
class JzRHIRenderer {
public:
    /**
     * @brief Constructor
     */
    JzRHIRenderer();

    /**
     * @brief Destructor
     */
    ~JzRHIRenderer();

    /**
     * @brief Render the Scene
     *
     * @param scene
     */
    void RenderScene(JzScene *scene);

    /**
     * @brief Set the Use Command List
     *
     * @param useCommandList
     */
    void SetUseCommandList(Bool useCommandList);

    /**
     * @brief Is Using Command List
     *
     * @return Bool
     */
    Bool IsUsingCommandList() const;

    /**
     * @brief Set the Thread Count
     *
     * @param threadCount
     */
    void SetThreadCount(U32 threadCount);

    /**
     * @brief Get the Thread Count
     *
     * @return U32
     */
    U32 GetThreadCount() const;

    /**
     * @brief Get the current framebuffer
     *
     * @return std::shared_ptr<JzGPUFramebufferObject>
     */
    std::shared_ptr<JzGPUFramebufferObject> GetFramebuffer() const;

    /**
     * @brief Get the default pipeline
     *
     * @return std::shared_ptr<JzRHIPipeline>
     */
    std::shared_ptr<JzRHIPipeline> GetDefaultPipeline() const;

    /**
     * @brief Check if renderer is initialized
     *
     * @return Bool
     */
    Bool IsInitialized() const;

    /**
     * @brief Force initialization
     *
     * @return Bool
     */
    Bool Initialize();

    /**
     * @brief Begin frame rendering
     */
    void BeginFrame();

    /**
     * @brief End frame rendering
     */
    void EndFrame();

    /**
     * @brief Setup viewport based on current framebuffer
     */
    void SetupViewport();

    /**
     * @brief Clear current framebuffer
     */
    void ClearBuffers();

    /**
     * @brief Bind framebuffer for rendering
     *
     * @param framebuffer Framebuffer to bind (nullptr for default)
     */
    void BindFramebuffer(std::shared_ptr<JzGPUFramebufferObject> framebuffer = nullptr);

    /**
     * @brief Set render state
     *
     * @param state Render state to set
     */
    void SetRenderState(const JzRenderState &state);

    /**
     * @brief Get current frame size
     */
    void SetFrameSize(JzIVec2 p_size);

    /**
     * @brief Get current frame size
     */
    JzIVec2 GetCurrentFrameSize() const;

    /**
     * @brief Get framebuffer output texture
     */
    std::shared_ptr<JzGPUTextureObject> GetCurrentTexture();

private:
    Bool CreateFramebuffer();
    Bool CreateDefaultPipeline();
    void CleanupResources();
    void RenderImmediate(std::shared_ptr<JzScene> scene);
    void RenderWithCommandList(std::shared_ptr<JzScene> scene);

private:
    std::shared_ptr<JzGPUFramebufferObject> m_framebuffer;
    std::shared_ptr<JzGPUTextureObject>     m_colorTexture;
    std::shared_ptr<JzGPUTextureObject>     m_depthTexture;
    std::shared_ptr<JzRHIPipeline>          m_defaultPipeline;
    Bool                                    m_useCommandList = false;
    Bool                                    m_isInitialized  = false;
    JzIVec2                                 m_frameSize;
    Bool                                    m_frameSizeChanged = false;
};

} // namespace JzRE