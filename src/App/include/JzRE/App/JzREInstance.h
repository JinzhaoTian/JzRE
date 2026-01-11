/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <filesystem>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Platform/JzDevice.h"
#include "JzRE/Runtime/Resource/JzResourceManager.h"
#include "JzRE/Runtime/Function/Input/JzInputManager.h"
#include "JzRE/Runtime/Function/Window/JzWindow.h"
#include "JzRE/Editor/JzEditor.h"
#include "JzRE/Runtime/Function/Rendering/JzRHIRenderer.h"
#include "JzRE/Runtime/Function/Scene/JzScene.h"

namespace JzRE {

/**
 * @brief Frame data for render thread synchronization
 */
struct JzFrameData {
    F32     deltaTime = 0.0f;
    JzIVec2 frameSize = {0, 0};
};

/**
 * @brief JzRE Instance
 */
class JzREInstance {
public:
    /**
     * @brief Constructor
     */
    JzREInstance(JzERHIType rhiType, std::filesystem::path &openDirectory);

    /**
     * @brief Destructor
     */
    ~JzREInstance();

    /**
     * @brief Run the render engine
     */
    void Run();

    /**
     * @brief Check if the render engine is running
     *
     * @return Bool
     */
    Bool IsRunning() const;

private:
    /**
     * @brief Render thread main function
     */
    void _RenderThread();

    /**
     * @brief Signal the render thread to render a new frame
     *
     * @param frameData Frame data for the current frame
     */
    void _SignalRenderFrame(const JzFrameData &frameData);

    /**
     * @brief Wait for the render thread to complete the current frame
     */
    void _WaitForRenderComplete();

private:
    std::unique_ptr<JzResourceManager> m_resourceManager;
    std::unique_ptr<JzWindow>          m_window;
    std::unique_ptr<JzDevice>          m_device;
    std::unique_ptr<JzInputManager>    m_inputManager;
    std::unique_ptr<JzEditor>          m_editor;
    std::unique_ptr<JzRHIRenderer>     m_renderer;
    std::shared_ptr<JzScene>           m_scene;

    std::thread             m_renderThread;
    std::atomic<Bool>       m_renderThreadRunning{false};
    std::mutex              m_renderMutex;
    std::condition_variable m_renderCondition;
    std::condition_variable m_renderCompleteCondition;
    std::atomic<Bool>       m_frameReady{false};
    std::atomic<Bool>       m_renderComplete{true};
    JzFrameData             m_frameData;
};
} // namespace JzRE