/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Function/ECS/JzEnttEntity.h"
#include "JzRE/Runtime/Function/ECS/JzEnttWorld.h"
#include "JzRE/Runtime/Function/ECS/JzEnttInputSystem.h"
#include "JzRE/Runtime/Function/ECS/JzEnttCameraSystem.h"
#include "JzRE/Runtime/Function/ECS/JzEnttLightSystem.h"
#include "JzRE/Runtime/Function/ECS/JzEnttRenderSystem.h"
#include "JzRE/Runtime/Function/ECS/JzAssetLoadingSystem.h"
#include "JzRE/Runtime/Function/Input/JzInputManager.h"
#include "JzRE/Runtime/Function/Window/JzWindow.h"
#include "JzRE/Runtime/Platform/JzDevice.h"
#include "JzRE/Runtime/Resource/JzAssetManager.h"

namespace JzRE {

/**
 * @brief Frame data for runtime thread synchronization
 */
struct JzRuntimeFrameData {
    F32     deltaTime = 0.0f;
    JzIVec2 frameSize = {0, 0};
};

/**
 * @brief JzRE Runtime Application
 *
 * This class provides core rendering functionality using an ECS-based architecture.
 * It manages the rendering pipeline through EnTT systems and input handling.
 *
 * Usage patterns:
 * 1. Standalone runtime: Override OnStart/OnUpdate/OnStop for custom logic
 * 2. Editor integration: Override OnRender to inject UI rendering after 3D scene
 *
 * ECS Systems (updated in order):
 * 1. JzEnttInputSystem - Input processing and component updates
 * 2. JzEnttCameraSystem - Camera matrix updates, orbit control
 * 3. JzEnttLightSystem - Light data collection
 * 4. JzEnttRenderSystem - Renders all entities with Transform + Mesh + Material
 */
class JzRERuntime {
public:
    /**
     * @brief Constructor
     *
     * @param rhiType The RHI type to use for rendering (e.g., OpenGL, Vulkan)
     * @param windowTitle Title of the window
     * @param windowSize Initial size of the window
     */
    JzRERuntime(JzERHIType rhiType, const String &windowTitle = "JzRE Runtime", const JzIVec2 &windowSize = {1280, 720});

    /**
     * @brief Virtual destructor
     */
    virtual ~JzRERuntime();

    /**
     * @brief Run the runtime application main loop
     */
    void Run();

    /**
     * @brief Check if the runtime application is running
     *
     * @return Bool True if the application is running, false otherwise
     */
    Bool IsRunning() const;

    /**
     * @brief Get the window instance
     *
     * @return JzWindow& Reference to the window
     */
    JzWindow &GetWindow();

    /**
     * @brief Get the device instance
     *
     * @return JzDevice& Reference to the device
     */
    JzDevice &GetDevice();

    /**
     * @brief Get the ECS world
     *
     * @return JzEnttWorld& Reference to the ECS world
     */
    JzEnttWorld &GetWorld();

    /**
     * @brief Get the asset manager instance
     *
     * @return JzAssetManager& Reference to the asset manager
     */
    JzAssetManager &GetAssetManager();

    /**
     * @brief Get current frame delta time
     *
     * @return F32 Delta time in seconds
     */
    F32 GetDeltaTime() const;

protected:
    /**
     * @brief Called before the main loop starts
     *
     * Override this method to perform custom initialization.
     */
    virtual void OnStart();

    /**
     * @brief Called once per frame before rendering
     *
     * @param deltaTime Time elapsed since the last frame in seconds
     *
     * Override this method to update game logic, camera movement, etc.
     */
    virtual void OnUpdate(F32 deltaTime);

    /**
     * @brief Called after 3D scene rendering, before buffer swap
     *
     * @param deltaTime Time elapsed since the last frame in seconds
     *
     * Override this method to render additional content (e.g., ImGui UI).
     * The 3D scene has already been rendered to the framebuffer at this point.
     */
    virtual void OnRender(F32 deltaTime);

    /**
     * @brief Called after the main loop ends
     *
     * Override this method to perform custom cleanup.
     */
    virtual void OnStop();

    /**
     * @brief Should the renderer blit the framebuffer content to the screen
     *
     * Override this method and return false if you handle rendering display
     * yourself (e.g., in Editor with ImGui). Default returns true for
     * standalone runtime applications.
     *
     * @return Bool True to blit framebuffer to screen, false otherwise
     */
    virtual Bool ShouldBlitToScreen() const;

private:
    /**
     * @brief Initialize the ECS world and systems
     */
    void InitializeECS();

    /**
     * @brief Create the default camera entity with orbit controller
     */
    void CreateDefaultCameraEntity();

    /**
     * @brief Create the default directional light entity
     */
    void CreateDefaultLightEntity();

    // ==================== Frame Phase Methods ====================

    /**
     * @brief Update ECS logic systems (movement, physics, AI, animations)
     *
     * This phase can run in parallel with GPU work from the previous frame.
     *
     * @param frameData Current frame data
     */
    void _UpdateECSLogic(const JzRuntimeFrameData &frameData);

    /**
     * @brief Update ECS pre-render systems (camera, lights, culling)
     *
     * Prepares data for rendering after logic updates are complete.
     *
     * @param frameData Current frame data
     */
    void _UpdateECSPreRender(const JzRuntimeFrameData &frameData);

    /**
     * @brief Update ECS render systems and execute rendering
     *
     * Performs actual GPU rendering operations.
     *
     * @param frameData Current frame data
     */
    void _UpdateECSRender(const JzRuntimeFrameData &frameData);

    /**
     * @brief Execute the rendering pipeline
     *
     * Handles BeginFrame, EndFrame, and BlitToScreen operations.
     *
     * @param frameData Current frame data
     */
    void _ExecuteRendering(const JzRuntimeFrameData &frameData);

    /**
     * @brief Finish the frame (swap buffers, clear input, update clock)
     *
     * @param frameData Current frame data
     */
    void _FinishFrame(const JzRuntimeFrameData &frameData);

protected:
    std::unique_ptr<JzWindow>       m_window;
    std::unique_ptr<JzDevice>       m_device;
    std::unique_ptr<JzInputManager> m_inputManager;
    std::unique_ptr<JzAssetManager> m_assetManager;

    // ECS world and systems
    std::unique_ptr<JzEnttWorld>          m_world;
    std::shared_ptr<JzEnttInputSystem>    m_inputSystem;
    std::shared_ptr<JzEnttCameraSystem>   m_cameraSystem;
    std::shared_ptr<JzEnttLightSystem>    m_lightSystem;
    std::shared_ptr<JzEnttRenderSystem>   m_renderSystem;
    std::shared_ptr<JzAssetLoadingSystem> m_assetLoadingSystem;

    // Main camera entity
    JzEnttEntity m_mainCameraEntity = INVALID_ENTT_ENTITY;

private:
    JzRuntimeFrameData m_frameData;
};

} // namespace JzRE
