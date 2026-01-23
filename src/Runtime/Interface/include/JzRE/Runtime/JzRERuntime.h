/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Function/ECS/JzEntity.h"
#include "JzRE/Runtime/Function/ECS/JzWorld.h"
#include "JzRE/Runtime/Function/ECS/JzInputSystem.h"
#include "JzRE/Runtime/Function/ECS/JzCameraSystem.h"
#include "JzRE/Runtime/Function/ECS/JzLightSystem.h"
#include "JzRE/Runtime/Function/ECS/JzRenderSystem.h"
#include "JzRE/Runtime/Function/ECS/JzAssetLoadingSystem.h"
#include "JzRE/Runtime/Function/Input/JzInputManager.h"
#include "JzRE/Runtime/Function/Window/JzWindow.h"
#include "JzRE/Runtime/Platform/JzDevice.h"
#include "JzRE/Runtime/Resource/JzAssetManager.h"

namespace JzRE {

struct JzRERuntimeSettings {
    String     windowTitle = "JzRE Runtime";
    JzIVec2    windowSize  = {1280, 720};
    JzERHIType rhiType     = JzERHIType::OpenGL;
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
    JzRERuntime(const JzRERuntimeSettings &settings = {});

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
     * @return JzWorld& Reference to the ECS world
     */
    JzWorld &GetWorld();

    /**
     * @brief Get the asset manager instance
     *
     * @return JzAssetManager& Reference to the asset manager
     */
    JzAssetManager &GetAssetManager();

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

private:
    void Startup();

    void CreateSubsystems();

    void RegisterComponents();

    void RegisterSystems();

    void InitializeSubsystems();

    void PreloadAssets();

    void OnFrameBegin();

    void UpdateSystems(F32 deltaTime);

    void SynchronizeSystems();

    void OnFrameEnd();

    void Shutdown();

    void SaveGameState();

    void ShutdownSubsystems();

    void CleanupGlobals();

    /**
     * @brief Create the global config entity
     */
    void CreateGlobalConfigEntity();

    /**
     * @brief Create the default camera entity with orbit controller
     */
    void CreateDefaultCameraEntity();

    /**
     * @brief Create the default directional light entity
     */
    void CreateDefaultLightEntity();

protected:
    JzRERuntimeSettings m_settings;

    std::unique_ptr<JzWindow>       m_window;
    std::unique_ptr<JzDevice>       m_device;
    std::unique_ptr<JzInputManager> m_inputManager;
    std::unique_ptr<JzAssetManager> m_assetManager;

    // ECS world and systems
    std::unique_ptr<JzWorld>              m_world;
    std::shared_ptr<JzInputSystem>        m_inputSystem;
    std::shared_ptr<JzCameraSystem>       m_cameraSystem;
    std::shared_ptr<JzLightSystem>        m_lightSystem;
    std::shared_ptr<JzRenderSystem>       m_renderSystem;
    std::shared_ptr<JzAssetLoadingSystem> m_assetLoadingSystem;

    JzEntity m_mainCameraEntity   = INVALID_ENTITY;
    JzEntity m_globalConfigEntity = INVALID_ENTITY;
};

} // namespace JzRE
