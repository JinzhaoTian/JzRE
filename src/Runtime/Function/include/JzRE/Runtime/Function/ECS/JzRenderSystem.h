/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzVector.h"
#include "JzRE/Runtime/Function/ECS/JzEntity.h"
#include "JzRE/Runtime/Function/ECS/JzSystem.h"
#include "JzRE/Runtime/Function/ECS/JzWorld.h"
#include "JzRE/Runtime/Function/Rendering/JzRenderGraph.h"
#include "JzRE/Runtime/Function/Rendering/JzRenderOutput.h"
#include "JzRE/Runtime/Function/Rendering/JzRenderOutputCache.h"
#include "JzRE/Runtime/Function/Rendering/JzRenderTarget.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUFramebufferObject.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUTextureObject.h"
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
    using ViewHandle                                = U32;
    static constexpr ViewHandle INVALID_VIEW_HANDLE = 0;
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
     * @brief Get the color texture for display (e.g., in ImGui).
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
     * @brief Get a render output by view handle.
     *
     * @param handle Handle returned from RegisterView()
     * @return Render output if available, otherwise nullptr
     */
    JzRenderOutput *GetRenderOutput(ViewHandle handle) const;

    /**
     * @brief Get a render output by name.
     *
     * @param name Output name
     * @return Render output if available, otherwise nullptr
     */
    JzRenderOutput *GetRenderOutput(const String &name) const;

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

    // ==================== View Registration ====================

    /**
     * @brief View description for RenderSystem-managed outputs.
     */
    struct JzRenderViewDesc {
        String                   name;
        String                   passName;
        String                   outputName;
        JzEntity                 camera         = INVALID_ENTITY;
        Bool                     includeEditor  = false;
        Bool                     includePreview = false;
        std::function<Bool()>    shouldRender;
        std::function<JzIVec2()> getDesiredSize;
    };

    /**
     * @brief Register a view for rendering.
     *
     * @param desc View description
     * @return Handle to the registered view
     */
    ViewHandle RegisterView(JzRenderViewDesc desc);

    /**
     * @brief Unregister a view.
     *
     * @param handle Handle returned from RegisterView()
     */
    void UnregisterView(ViewHandle handle);

    /**
     * @brief Update the camera for a registered view.
     *
     * @param handle Handle returned from RegisterView()
     * @param camera New camera entity
     */
    void UpdateViewCamera(ViewHandle handle, JzEntity camera);

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
     * @brief Render to a target with entity filtering based on tags.
     *
     * @param world The ECS world
     * @param target The render target
     * @param camera The camera entity
     * @param includeEditor Whether to include JzEditorOnlyTag entities
     * @param includePreview Whether to include JzPreviewOnlyTag entities
     */
    void RenderToTargetFiltered(JzWorld &world, JzRenderTarget &target, JzEntity camera,
                                Bool includeEditor, Bool includePreview);

    /**
     * @brief Render entities with tag filtering.
     *
     * @param world The ECS world
     * @param includeEditor Whether to include JzEditorOnlyTag entities
     * @param includePreview Whether to include JzPreviewOnlyTag entities
     */
    void RenderEntitiesFiltered(JzWorld &world, Bool includeEditor, Bool includePreview);

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

    // Frame state
    JzIVec2 m_frameSize{1280, 720};
    Bool    m_frameSizeChanged = true;
    Bool    m_isInitialized    = false;

    // View registry for unified rendering
    std::vector<std::pair<ViewHandle, JzRenderViewDesc>>            m_views;
    ViewHandle                                                      m_nextViewHandle = 1;
    std::unordered_map<ViewHandle, std::shared_ptr<JzRenderTarget>> m_targets;
    JzRenderOutputCache                                             m_outputCache;

    // Phase 1 RenderGraph (single-pass integration)
    JzRenderGraph m_renderGraph;
};

} // namespace JzRE
