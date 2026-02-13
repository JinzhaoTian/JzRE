/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "JzRE/Runtime/Core/JzMatrix.h"
#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzVector.h"
#include "JzRE/Runtime/Function/ECS/JzEntity.h"
#include "JzRE/Runtime/Function/ECS/JzSystem.h"
#include "JzRE/Runtime/Function/ECS/JzWorld.h"
#include "JzRE/Runtime/Function/Rendering/JzRenderGraph.h"
#include "JzRE/Runtime/Function/Rendering/JzRenderOutput.h"
#include "JzRE/Runtime/Function/Rendering/JzRenderPass.h"
#include "JzRE/Runtime/Function/Rendering/JzRenderTarget.h"
#include "JzRE/Runtime/Function/Rendering/JzRenderVisibility.h"
#include "JzRE/Runtime/Platform/Command/JzRHIDrawCommand.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUBufferObject.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUFramebufferObject.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUTextureObject.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUVertexArrayObject.h"
#include "JzRE/Runtime/Platform/RHI/JzRHIPipeline.h"

namespace JzRE {

/**
 * @brief Enhanced render system that integrates with camera system.
 *
 * This system manages:
 * - Framebuffer, color texture, and depth texture
 * - Default rendering pipeline with shaders
 * - Rendering all entities with Transform + Mesh + Material components
 * - Blitting to screen for standalone runtime
 */
class JzRenderSystem : public JzSystem {
public:
    /**
     * @brief Constructs the render system.
     */
    JzRenderSystem();

    void OnInit(JzWorld &world) override;
    void Update(JzWorld &world, F32 delta) override;
    void OnShutdown(JzWorld &world) override;

    /**
     * @brief Render system runs in Render phase.
     */
    JzSystemPhase GetPhase() const override
    {
        return JzSystemPhase::Render;
    }

    // ==================== Framebuffer Management ====================

    /**
     * @brief Get the current frame size.
     */
    JzIVec2 GetCurrentFrameSize() const;

    /**
     * @brief Get the framebuffer.
     */
    std::shared_ptr<JzGPUFramebufferObject> GetFramebuffer() const;

    /**
     * @brief Get the color texture for host UI presentation.
     */
    std::shared_ptr<JzGPUTextureObject> GetColorTexture() const;

    /**
     * @brief Get the depth texture.
     */
    std::shared_ptr<JzGPUTextureObject> GetDepthTexture() const;

    /**
     * @brief Get the default rendering pipeline.
     */
    std::shared_ptr<JzRHIPipeline> GetDefaultPipeline() const;

    /**
     * @brief Get a render output by render target handle.
     *
     * @param handle Handle returned from RegisterRenderTarget()
     * @return Render output if available, otherwise nullptr
     */
    JzRenderOutput *GetRenderOutput(JzRenderTargetHandle handle) const;

    // ==================== Frame Control ====================

    /**
     * @brief Begin frame rendering.
     */
    void BeginFrame();

    /**
     * @brief End frame rendering.
     */
    void EndFrame();

    /**
     * @brief Blit the framebuffer content to the screen.
     *
     * @param screenWidth The width of the screen.
     * @param screenHeight The height of the screen.
     */
    void BlitToScreen(U32 screenWidth, U32 screenHeight);

    /**
     * @brief Check if the render system is initialized.
     */
    Bool IsInitialized() const;

    /**
     * @brief Register a render pass.
     *
     * If a pass with the same name already exists, it is replaced in-place.
     *
     * @param pass Render pass descriptor.
     */
    void RegisterRenderPass(JzRenderPass pass);

    /**
     * @brief Remove all registered render passes.
     */
    void ClearRenderPasses();

    // ==================== Render Target Registration ====================

    /**
     * @brief Register a logical render target.
     *
     * @param desc Render target description
     * @return Handle to the registered render target
     */
    JzRenderTargetHandle RegisterRenderTarget(JzRenderTargetDesc desc);

    /**
     * @brief Unregister a logical render target.
     *
     * @param handle Handle returned from RegisterRenderTarget()
     */
    void UnregisterRenderTarget(JzRenderTargetHandle handle);

    /**
     * @brief Update the camera for a registered render target.
     *
     * @param handle Handle returned from RegisterRenderTarget()
     * @param camera New camera entity
     */
    void UpdateRenderTargetCamera(JzRenderTargetHandle handle, JzEntity camera);

    /**
     * @brief Update the feature mask for a registered render target.
     *
     * @param handle Handle returned from RegisterRenderTarget()
     * @param features New feature mask
     */
    void UpdateRenderTargetFeatures(JzRenderTargetHandle handle, JzRenderTargetFeatures features);

    /**
     * @brief Update visibility mask for a registered render target.
     *
     * @param handle Handle returned from RegisterRenderTarget()
     * @param visibility New visibility mask
     */
    void UpdateRenderTargetVisibility(JzRenderTargetHandle handle, JzRenderVisibility visibility);

private:
    /**
     * @brief Create the framebuffer and its attachments.
     */
    Bool CreateFramebuffer();

    /**
     * @brief Create the default rendering pipeline with shaders.
     */
    Bool CreateDefaultPipeline();

    /**
     * @brief Setup viewport and clear the framebuffer.
     */
    void SetupViewportAndClear(JzWorld &world);

    /**
     * @brief Render all entities with Transform + Mesh + Material.
     */
    void RenderEntities(JzWorld &world);

    /**
     * @brief Render to an output with entity filtering based on visibility.
     *
     * @param world The ECS world
     * @param output The render output
     * @param camera The camera entity
     * @param visibility Visibility mask for entity filtering
     * @param features Target feature mask
     */
    void RenderToTargetFiltered(JzWorld &world, JzRenderOutput &output,
                                JzEntity camera, JzRenderVisibility visibility,
                                JzRenderTargetFeatures features);

    /**
     * @brief Render entities with visibility filtering.
     *
     * @param world The ECS world
     * @param visibility Visibility mask for entity filtering
     */
    void RenderEntitiesFiltered(JzWorld &world, JzRenderVisibility visibility);

    /**
     * @brief Execute render passes enabled for a render target.
     *
     * @param world The ECS world
     * @param features Feature mask for the current target
     * @param viewMatrix Current camera view matrix
     * @param projectionMatrix Current camera projection matrix
     */
    void ExecuteRenderPasses(JzWorld &world, JzRenderTargetFeatures features,
                             const JzMat4 &viewMatrix, const JzMat4 &projectionMatrix);

    /**
     * @brief Apply render graph transitions (backend-specific).
     */
    void ApplyRenderGraphTransitions(const JzRGPassDesc                &passDesc,
                                     const std::vector<JzRGTransition> &transitions);

    /**
     * @brief Clean up all GPU resources.
     */
    void CleanupResources();

private:
    // GPU resources
    std::shared_ptr<JzGPUFramebufferObject> m_framebuffer;
    std::shared_ptr<JzGPUTextureObject>     m_colorTexture;
    std::shared_ptr<JzGPUTextureObject>     m_depthTexture;
    std::shared_ptr<JzRHIPipeline>          m_defaultPipeline;
    std::vector<JzRenderPass>               m_renderPasses;

    // Frame state
    JzIVec2 m_frameSize{1280, 720};
    Bool    m_frameSizeChanged = true;
    Bool    m_isInitialized    = false;

    // Logical render target registry
    std::vector<JzRenderTarget> m_renderTargets;
    JzRenderTargetHandle        m_nextRenderTargetHandle = 1;

    // Phase 1 RenderGraph (single-pass integration)
    JzRenderGraph m_renderGraph;
};

} // namespace JzRE
