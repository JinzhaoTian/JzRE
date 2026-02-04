/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzVector.h"
#include "JzRE/Runtime/Function/ECS/JzEntity.h"
#include "JzRE/Runtime/Function/ECS/JzSystem.h"
#include "JzRE/Runtime/Function/ECS/JzWorld.h"
#include "JzRE/Runtime/Function/Rendering/JzRenderTarget.h"
#include "JzRE/Runtime/Function/Rendering/JzRenderTargetRegistry.h"
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

    // ==================== RenderTarget Registration ====================

    /**
     * @brief Register a render target entry.
     *
     * Views should call this during initialization to register their
     * render targets for unified rendering.
     *
     * @param entry The entry containing target, camera, and filter settings
     * @return Handle to the registered entry
     */
    JzRenderTargetRegistry::Handle RegisterTarget(JzRenderTargetEntry entry);

    /**
     * @brief Unregister a render target.
     *
     * @param handle Handle returned from RegisterTarget()
     */
    void UnregisterTarget(JzRenderTargetRegistry::Handle handle);

    /**
     * @brief Update the camera for a registered target.
     *
     * @param handle Handle returned from RegisterTarget()
     * @param camera New camera entity
     */
    void UpdateTargetCamera(JzRenderTargetRegistry::Handle handle, JzEntity camera);

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
     * @brief Render all registered targets.
     *
     * Called during Update() to render all View targets.
     */
    void RenderAllTargets(JzWorld &world);

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

    // Render target registry for unified View rendering
    JzRenderTargetRegistry m_targetRegistry;
};

} // namespace JzRE
