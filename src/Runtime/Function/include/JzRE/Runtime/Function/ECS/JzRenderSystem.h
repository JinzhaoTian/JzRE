/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzMatrix.h"
#include "JzRE/Runtime/Core/JzVector.h"
#include "JzRE/Runtime/Function/ECS/JzEntity.h"
#include "JzRE/Runtime/Function/ECS/JzSystem.h"
#include "JzRE/Runtime/Function/ECS/JzWorld.h"
#include "JzRE/Runtime/Function/Rendering/JzRenderGraph.h"
#include "JzRE/Runtime/Function/Rendering/JzRenderOutput.h"
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
 * @brief Per-view render feature flags.
 *
 * A view can opt in to additional editor rendering features
 * (such as skybox and axis helpers) without forcing those
 * features into all render targets.
 */
enum class JzRenderViewFeatures : U32 {
    None   = 0,
    Skybox = 1 << 0,
    Axis   = 1 << 1,
    Grid   = 1 << 2,
    Gizmo  = 1 << 3,
};

/**
 * @brief Bitwise OR for JzRenderViewFeatures.
 */
inline constexpr JzRenderViewFeatures operator|(JzRenderViewFeatures lhs, JzRenderViewFeatures rhs)
{
    return static_cast<JzRenderViewFeatures>(static_cast<U32>(lhs) | static_cast<U32>(rhs));
}

/**
 * @brief Bitwise AND for JzRenderViewFeatures.
 */
inline constexpr JzRenderViewFeatures operator&(JzRenderViewFeatures lhs, JzRenderViewFeatures rhs)
{
    return static_cast<JzRenderViewFeatures>(static_cast<U32>(lhs) & static_cast<U32>(rhs));
}

/**
 * @brief Check if feature mask contains a specific feature.
 */
inline constexpr Bool HasFeature(JzRenderViewFeatures mask, JzRenderViewFeatures feature)
{
    return static_cast<U32>(mask & feature) != 0;
}

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
     * This is mainly intended for named RenderGraph exports. View outputs
     * are generated internally from view names and can be queried directly
     * by handle via GetRenderOutput(ViewHandle).
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

    /**
     * @brief Description for a feature-gated helper rendering pass.
     *
     * Helper passes are executed after filtered entity rendering in each view.
     * Each pass is controlled by a feature flag (Skybox/Grid/Axis/...) and
     * shares a unified execution path in RenderSystem.
     */
    struct JzRenderHelperPass {
        String                                                       name;
        JzRenderViewFeatures                                         feature = JzRenderViewFeatures::None;
        std::shared_ptr<JzRHIPipeline>                               pipeline;
        std::shared_ptr<JzGPUVertexArrayObject>                      vertexArray;
        JzDrawParams                                                 drawParams;
        std::function<void(const std::shared_ptr<JzRHIPipeline> &,
                           JzWorld &, const JzMat4 &, const JzMat4 &)> setupPass;
    };

    /**
     * @brief Register a helper pass.
     *
     * If a pass with the same name already exists, it is replaced in-place.
     *
     * @param pass Helper pass descriptor.
     */
    void RegisterHelperPass(JzRenderHelperPass pass);

    /**
     * @brief Remove all registered helper passes.
     */
    void ClearHelperPasses();

    // ==================== View Registration ====================

    /**
     * @brief View description for RenderSystem-managed outputs.
     *
     * This struct only captures view semantics. Internal pass/output
     * names are generated by RenderSystem to reduce duplicated naming state.
     */
    struct JzRenderViewDesc {
        String                   name;
        JzEntity                 camera     = INVALID_ENTITY;
        JzRenderVisibility       visibility = JzRenderVisibility::Untagged;
        JzRenderViewFeatures     features   = JzRenderViewFeatures::None;
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

    /**
     * @brief Update the feature mask for a registered view.
     *
     * @param handle Handle returned from RegisterView()
     * @param features New feature mask
     */
    void UpdateViewFeatures(ViewHandle handle, JzRenderViewFeatures features);

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
     * @brief Render to a target with entity filtering based on visibility.
     *
     * @param world The ECS world
     * @param target The render target
     * @param camera The camera entity
     * @param visibility Visibility mask for entity filtering
     */
    void RenderToTargetFiltered(JzWorld &world, JzRenderTarget &target, JzEntity camera,
                                JzRenderVisibility visibility, JzRenderViewFeatures features);

    /**
     * @brief Render entities with visibility filtering.
     *
     * @param world The ECS world
     * @param visibility Visibility mask for entity filtering
     */
    void RenderEntitiesFiltered(JzWorld &world, JzRenderVisibility visibility);

    /**
     * @brief Execute helper passes enabled for a view.
     *
     * @param world The ECS world
     * @param features Feature mask for the current view
     * @param viewMatrix Current camera view matrix
     * @param projectionMatrix Current camera projection matrix
     */
    void RenderHelperPasses(JzWorld &world, JzRenderViewFeatures features,
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
    /**
     * @brief Runtime view record combining view configuration and render target.
     *
     * This merges the previously separated "view descriptor" and "target map"
     * into one owning structure to avoid duplicated state.
     */
    struct JzRenderView {
        ViewHandle                     handle = INVALID_VIEW_HANDLE;
        JzRenderViewDesc               desc;
        String                         passName;
        String                         outputName;
        std::shared_ptr<JzRenderTarget> target;
    };

    // GPU resources
    std::shared_ptr<JzGPUFramebufferObject> m_framebuffer;
    std::shared_ptr<JzGPUTextureObject>     m_colorTexture;
    std::shared_ptr<JzGPUTextureObject>     m_depthTexture;
    std::shared_ptr<JzRHIPipeline>          m_defaultPipeline;
    std::vector<JzRenderHelperPass>         m_helperPasses;

    // Frame state
    JzIVec2 m_frameSize{1280, 720};
    Bool    m_frameSizeChanged = true;
    Bool    m_isInitialized    = false;

    // View registry for unified rendering
    std::vector<JzRenderView> m_views;
    ViewHandle                m_nextViewHandle = 1;

    // Phase 1 RenderGraph (single-pass integration)
    JzRenderGraph m_renderGraph;
};

} // namespace JzRE
