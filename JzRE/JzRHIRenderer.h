#pragma once

#include "JzRHIFramebuffer.h"
#include "JzRHIPipeline.h"
#include "JzRHIStats.h"
#include "JzRHITexture.h"
#include "JzScene.h"

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
     * @return std::shared_ptr<JzRHIFramebuffer>
     */
    std::shared_ptr<JzRHIFramebuffer> GetFramebuffer() const;

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
    void BindFramebuffer(std::shared_ptr<JzRHIFramebuffer> framebuffer = nullptr);

    /**
     * @brief Set render state
     *
     * @param state Render state to set
     */
    void SetRenderState(const JzRenderState &state);

private:
    Bool CreateFramebuffer();
    Bool CreateDefaultPipeline();
    void CleanupResources();
    void RenderImmediate(std::shared_ptr<JzScene> scene);
    void RenderWithCommandList(std::shared_ptr<JzScene> scene);

private:
    std::shared_ptr<JzRHIFramebuffer> m_framebuffer;
    std::shared_ptr<JzRHITexture>     m_colorTexture;
    std::shared_ptr<JzRHITexture>     m_depthTexture;
    std::shared_ptr<JzRHIPipeline>    m_defaultPipeline;
    Bool                              m_useCommandList = false;
    Bool                              m_isInitialized  = false;
};

} // namespace JzRE