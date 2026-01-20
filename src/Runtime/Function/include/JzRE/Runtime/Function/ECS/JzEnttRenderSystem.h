/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzVector.h"
#include "JzRE/Runtime/Function/ECS/JzEnttSystem.h"
#include "JzRE/Runtime/Function/ECS/JzEnttWorld.h"
#include "JzRE/Runtime/Platform/JzGPUFramebufferObject.h"
#include "JzRE/Runtime/Platform/JzGPUTextureObject.h"
#include "JzRE/Runtime/Platform/JzRHIPipeline.h"

namespace JzRE {

// Forward declarations
class JzEnttCameraSystem;

/**
 * @brief Enhanced render system that integrates with camera system.
 *
 * This system manages:
 * - Framebuffer, color texture, and depth texture
 * - Default rendering pipeline with shaders
 * - Rendering all entities with Transform + Mesh + Material components
 * - Blitting to screen for standalone runtime
 */
class JzEnttRenderSystem : public JzEnttSystem {
public:
    /**
     * @brief Constructs the render system.
     */
    JzEnttRenderSystem();

    void OnInit(JzEnttWorld &world) override;
    void Update(JzEnttWorld &world, F32 delta) override;
    void OnShutdown(JzEnttWorld &world) override;

    /**
     * @brief Render system runs in Render phase.
     */
    JzSystemPhase GetPhase() const override
    {
        return JzSystemPhase::Render;
    }

    // ==================== Camera System Reference ====================

    /**
     * @brief Set the camera system reference.
     */
    void SetCameraSystem(std::shared_ptr<JzEnttCameraSystem> cameraSystem);

    // ==================== Framebuffer Management ====================

    /**
     * @brief Set the frame size for the framebuffer.
     *
     * @param size The width and height of the frame.
     */
    void SetFrameSize(JzIVec2 size);

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
    void SetupViewportAndClear();

    /**
     * @brief Render all entities with Transform + Mesh + Material.
     */
    void RenderEntities(JzEnttWorld &world);

    /**
     * @brief Clean up all GPU resources.
     */
    void CleanupResources();

private:
    // System references
    std::shared_ptr<JzEnttCameraSystem> m_cameraSystem;

    // GPU resources
    std::shared_ptr<JzGPUFramebufferObject> m_framebuffer;
    std::shared_ptr<JzGPUTextureObject>     m_colorTexture;
    std::shared_ptr<JzGPUTextureObject>     m_depthTexture;
    std::shared_ptr<JzRHIPipeline>          m_defaultPipeline;

    // Frame state
    JzIVec2 m_frameSize{1280, 720};
    Bool    m_frameSizeChanged = true;
    Bool    m_isInitialized    = false;
};

} // namespace JzRE
