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
#include "JzRE/Core/JzRETypes.h"
#include "JzRE/RHI/JzDevice.h"
#include "JzRE/Resource/JzResourceManager.h"
#include "JzRE/Editor/JzInputManager.h"
#include "JzRE/Editor/JzUIManager.h"
#include "JzRE/Editor/JzWindow.h"
#include "JzRE/Editor/JzEditor.h"

namespace JzRE {

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
    void _RenderThread();

private:
    std::unique_ptr<JzResourceManager> m_resourceManager;
    std::unique_ptr<JzWindow>          m_window;
    std::unique_ptr<JzDevice>          m_device;
    std::unique_ptr<JzInputManager>    m_inputManager;
    std::unique_ptr<JzUIManager>       m_uiManager;
    std::unique_ptr<JzEditor>          m_editor;

    std::thread             m_renderThread;
    std::atomic<Bool>       m_renderThreadRunning{false};
    std::mutex              m_renderMutex;
    std::condition_variable m_renderCondition;
    std::atomic<Bool>       m_frameReady{false};
    std::atomic<Bool>       m_shouldRender{false};
};
} // namespace JzRE