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
    void RenderScene(std::shared_ptr<JzScene> scene);

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

private:
    Bool CreateFramebuffer();
    Bool CreateDefaultPipeline();
    void CleanupResources();
    void RenderImmediate(std::shared_ptr<JzScene> scene);
    void RenderWithCommandList(std::shared_ptr<JzScene> scene);
    void SetupViewport();
    void ClearBuffers();

private:
    std::shared_ptr<JzRHIFramebuffer> m_framebuffer;
    std::shared_ptr<JzRHITexture>     m_colorTexture;
    std::shared_ptr<JzRHITexture>     m_depthTexture;
    std::shared_ptr<JzRHIPipeline>    m_defaultPipeline;
    Bool                              m_useCommandList = true;
    Bool                              m_isInitialized  = false;
};

} // namespace JzRE