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
#include "JzRE/Runtime/Function/Rendering/JzRenderGraphContribution.h"
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

class JzAssetManager;
class JzDevice;

/**
 * @brief Enhanced render system that integrates with camera system.
 *
 * This system manages:
 * - Main and registered render outputs
 * - RenderGraph pass recording and execution
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
     * @brief Register a graph contribution.
     *
     * If a contribution with the same name already exists, it is replaced.
     */
    void RegisterGraphContribution(JzRenderGraphContribution contribution);

    /**
     * @brief Remove all registered graph contributions.
     */
    void ClearGraphContributions();

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
     * @brief Ensure the main output size/resources match current frame size.
     */
    Bool EnsureMainOutput();

    /**
     * @brief Resolve the geometry rendering pipeline from shader assets.
     */
    std::shared_ptr<JzRHIPipeline> ResolveGeometryPipeline() const;

    /**
     * @brief Execute one contribution for a render target.
     */
    void ExecuteContribution(JzWorld                         &world,
                             const JzRGPassContext           &passContext,
                             JzEntity                         camera,
                             JzRenderVisibility               visibility,
                             JzRenderTargetFeatures           targetFeatures,
                             std::shared_ptr<JzRHIPipeline>   geometryPipeline,
                             const JzRenderGraphContribution &contribution);

    /**
     * @brief Resolve camera matrices and clear color for the current target.
     */
    void ResolveCameraFrameData(JzWorld &world, JzEntity preferredCamera,
                                JzMat4 &viewMatrix, JzMat4 &projectionMatrix,
                                JzVec3 &clearColor) const;

    /**
     * @brief Bind framebuffer/pipeline, set viewport, and clear target.
     */
    void BeginRenderTargetPass(const JzRGPassContext         &passContext,
                               const JzMat4                  &viewMatrix,
                               const JzMat4                  &projectionMatrix,
                               const JzVec3                  &clearColor,
                               std::shared_ptr<JzRHIPipeline> pipeline);

    /**
     * @brief Bind framebuffer and viewport for contribution passes.
     */
    void BeginContributionTargetPass(const JzRGPassContext &passContext);

    /**
     * @brief Render entities for a visibility mask.
     */
    void DrawVisibleEntities(JzWorld &world, JzRenderVisibility visibility,
                             std::shared_ptr<JzRHIPipeline> pipeline);

    /**
     * @brief Draw a single renderable entity with the geometry pipeline.
     */
    void DrawEntity(JzWorld &world, JzEntity entity, JzAssetManager &assetManager,
                    JzDevice &device, std::shared_ptr<JzRHIPipeline> pipeline);

    /**
     * @brief Check if an entity should be rendered by the current visibility mask.
     */
    Bool IsEntityVisible(JzWorld &world, JzEntity entity, JzRenderVisibility visibility) const;

    /**
     * @brief Check if a contribution should run for a target feature mask.
     */
    Bool IsContributionEnabled(JzRenderTargetFeatures targetFeatures,
                               JzRenderTargetFeatures requiredFeature) const;

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
    JzIVec2 m_frameSize{1280, 720};
    Bool    m_frameSizeChanged = true;
    Bool    m_isInitialized    = false;

    std::vector<JzRenderTarget> m_renderTargets;
    JzRenderTargetHandle        m_nextRenderTargetHandle = 1;

    JzRenderGraph                          m_renderGraph;
    std::vector<JzRenderGraphContribution> m_graphContributions;

    std::shared_ptr<JzRenderOutput> m_mainOutput;
};

} // namespace JzRE
