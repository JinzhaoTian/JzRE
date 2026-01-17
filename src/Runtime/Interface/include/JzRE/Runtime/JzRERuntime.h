/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Function/Input/JzInputManager.h"
#include "JzRE/Runtime/Function/Rendering/JzRHIRenderer.h"
#include "JzRE/Runtime/Function/Scene/JzScene.h"
#include "JzRE/Runtime/Function/Window/JzWindow.h"
#include "JzRE/Runtime/Platform/JzDevice.h"
#include "JzRE/Runtime/Resource/JzResourceManager.h"

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
 * This class provides core rendering functionality that can be used standalone
 * or as a base for the Editor. It manages the rendering pipeline, scene,
 * input handling, and background worker thread for non-GPU tasks.
 *
 * Usage patterns:
 * 1. Standalone runtime: Override OnStart/OnUpdate/OnStop for custom logic
 * 2. Editor integration: Override OnRender to inject UI rendering after 3D scene
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
    JzRERuntime(JzERHIType rhiType, const String &windowTitle = "JzRE Runtime",
                const JzIVec2 &windowSize = {1280, 720});

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
     * @brief Get the renderer instance
     *
     * @return JzRHIRenderer& Reference to the renderer
     */
    JzRHIRenderer &GetRenderer();

    /**
     * @brief Get the scene instance
     *
     * @return std::shared_ptr<JzScene> Shared pointer to the scene
     */
    std::shared_ptr<JzScene> GetScene();

    /**
     * @brief Get the input manager instance
     *
     * @return JzInputManager& Reference to the input manager
     */
    JzInputManager &GetInputManager();

    /**
     * @brief Get the resource manager instance
     *
     * @return JzResourceManager& Reference to the resource manager
     */
    JzResourceManager &GetResourceManager();

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

    /**
     * @brief Access the current frame data
     *
     * @return const JzRuntimeFrameData& Current frame data
     */
    const JzRuntimeFrameData &GetFrameData() const;

protected:
    /**
     * @brief Update camera position and rotation from orbit parameters
     *
     * Call this method after modifying orbit parameters (m_orbitTarget,
     * m_orbitYaw, m_orbitPitch, m_orbitDistance) to apply the changes
     * to the camera immediately.
     */
    void UpdateCameraFromOrbit();

private:
    /**
     * @brief Worker thread main function for background processing
     */
    void _WorkerThread();

    /**
     * @brief Signal the worker thread to process a new frame
     *
     * @param frameData Frame data for the current frame
     */
    void _SignalWorkerFrame(const JzRuntimeFrameData &frameData);

    /**
     * @brief Wait for the worker thread to complete processing
     */
    void _WaitForWorkerComplete();

    /**
     * @brief Handle default input actions for camera control
     *
     * Provides built-in orbit camera controls:
     * - Left mouse drag: Orbit rotation
     * - Right mouse drag: Panning
     * - Scroll wheel: Zoom in/out
     *
     * @param deltaTime Time elapsed since the last frame in seconds
     */
    void _HandleDefaultInputActions(F32 deltaTime);

    /**
     * @brief Handle orbit rotation from mouse movement
     *
     * @param deltaX Horizontal mouse movement
     * @param deltaY Vertical mouse movement
     */
    void _HandleOrbitRotation(F32 deltaX, F32 deltaY);

    /**
     * @brief Handle camera panning from mouse movement
     *
     * @param deltaX Horizontal mouse movement
     * @param deltaY Vertical mouse movement
     */
    void _HandlePanning(F32 deltaX, F32 deltaY);

    /**
     * @brief Handle zoom from scroll wheel
     *
     * @param scrollY Vertical scroll amount
     */
    void _HandleZoom(F32 scrollY);

protected:
    std::unique_ptr<JzResourceManager> m_resourceManager;
    std::unique_ptr<JzWindow>          m_window;
    std::unique_ptr<JzDevice>          m_device;
    std::unique_ptr<JzInputManager>    m_inputManager;
    std::unique_ptr<JzRHIRenderer>     m_renderer;
    std::shared_ptr<JzScene>           m_scene;

    // Orbit camera control state
    JzVec3 m_orbitTarget{0.0f, 0.0f, 0.0f}; ///< Point the camera orbits around
    F32    m_orbitYaw      = 0.0f;          ///< Horizontal orbit angle in radians
    F32    m_orbitPitch    = 0.3f;          ///< Vertical orbit angle in radians
    F32    m_orbitDistance = 5.0f;          ///< Distance from target

    // Camera control parameters
    F32 m_orbitSensitivity = 0.005f; ///< Sensitivity for orbit rotation
    F32 m_panSensitivity   = 0.002f; ///< Sensitivity for panning
    F32 m_zoomSensitivity  = 0.5f;   ///< Sensitivity for zooming
    F32 m_minDistance      = 0.5f;   ///< Minimum orbit distance
    F32 m_maxDistance      = 100.0f; ///< Maximum orbit distance

    // Mouse tracking state
    Bool   m_leftMousePressed  = false; ///< Left mouse button state
    Bool   m_rightMousePressed = false; ///< Right mouse button state
    Bool   m_firstMouse        = true;  ///< First mouse input flag
    JzVec2 m_lastMousePos{0.0f, 0.0f};  ///< Last mouse position

private:
    // Worker thread for non-GPU tasks
    std::thread             m_workerThread;
    std::atomic<Bool>       m_workerThreadRunning{false};
    std::mutex              m_workerMutex;
    std::condition_variable m_workerCondition;
    std::condition_variable m_workerCompleteCondition;
    std::atomic<Bool>       m_frameReady{false};
    std::atomic<Bool>       m_workerComplete{true};
    JzRuntimeFrameData      m_frameData;
};

} // namespace JzRE
