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
 * This class provides core rendering functionality without Editor features.
 * It manages the rendering pipeline, scene, and background worker thread
 * for non-GPU tasks like culling and animation updates.
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
     * @brief Destructor
     */
    ~JzRERuntime();

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
     * @return JzScene& Reference to the scene
     */
    std::shared_ptr<JzScene> GetScene();

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
     * Override this method to update game logic.
     */
    virtual void OnUpdate(F32 deltaTime);

    /**
     * @brief Called after the main loop ends
     *
     * Override this method to perform custom cleanup.
     */
    virtual void OnStop();

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

private:
    std::unique_ptr<JzResourceManager> m_resourceManager;
    std::unique_ptr<JzWindow>          m_window;
    std::unique_ptr<JzDevice>          m_device;
    std::unique_ptr<JzRHIRenderer>     m_renderer;
    std::shared_ptr<JzScene>           m_scene;

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
