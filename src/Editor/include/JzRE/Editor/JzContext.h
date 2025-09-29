/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>
#include <filesystem>
#include "JzRE/Core/JzRETypes.h"
#include "JzRE/Editor/JzWindow.h"
#include "JzRE/Editor/JzWindowSettings.h"
#include "JzRE/RHI/JzRHIETypes.h"
#include "JzRE/RHI/JzRHIDevice.h"
#include "JzRE/RHI/JzRHICommandQueue.h"
#include "JzRE/Editor/JzInputManager.h"
#include "JzRE/Editor/JzSceneManager.h"
#include "JzRE/Editor/JzUIManager.h"
#include "JzRE/Resource/JzResourceManager.h"
#include "JzRE/RHI/JzRenderFrontend.h"

namespace JzRE {

/**
 * @brief Context of JzRE
 */
class JzContext {
public:
    /**
     * @brief Get singleton instance Context
     *
     * @return JzContext&
     */
    static JzContext &GetInstance();

    /**
     * @brief Initialize Context
     *
     * @param rhiType Graphics API Types
     * @param openDirectory Current open directory
     * @return Bool
     */
    Bool Initialize(JzERHIType rhiType, std::filesystem::path &openDirectory);

    /**
     * @brief Is Context initialized
     *
     * @return Bool
     */
    Bool IsInitialized() const;

    /**
     * @brief Shutdown
     */
    void Shutdown();

    /**
     * @brief Get the window
     *
     * @return JzWindow& The window
     */
    JzWindow &GetWindow() const;

    /**
     * @brief Get the RHI type
     *
     * @return The RHI type
     */
    JzERHIType GetRHIType() const;

    /**
     * @brief Get the RHI device
     *
     * @return JzRHIDevice& The RHI device
     */
    JzRHIDevice &GetDevice() const;

    /**
     * @brief Get the input manager
     *
     * @return JzInputManager& The input manager
     */
    JzInputManager &GetInputManager() const;

    /**
     * @brief Get the UI manager
     *
     * @return JzUIManager& The UI Manager
     */
    JzUIManager &GetUIManager() const;

    /**
     * @brief Get the scene manager
     *
     * @return JzSceneManager& The scene manager
     */
    JzSceneManager &GetSceneManager() const;

    /**
     * @brief Get the command queue
     *
     * @return JzRHICommandQueue& The command queue
     */
    JzRHICommandQueue &GetCommandQueue() const;

    /**
     * @brief Set the thread count
     *
     * @param threadCount The thread count
     */
    void SetThreadCount(U32 threadCount);

    /**
     * @brief Get the thread count
     *
     * @return The thread count
     */
    U32 GetThreadCount() const;

    /**
     * @brief Get the Current Project Path
     *
     * @return std::filesystem::path
     */
    std::filesystem::path GetCurrentPath() const;

    /**
     * @brief Set the render frontend.
     *
     * @param frontend The render frontend
     */
    void SetRenderFrontend(JzRenderFrontend* frontend);

    /**
     * @brief Get the render frontend.
     *
     * @return JzRenderFrontend& The render frontend
     */
    JzRenderFrontend& GetRenderFrontend() const;

private:
    JzContext()                             = default;
    ~JzContext()                            = default;
    JzContext(const JzContext &)            = delete;
    JzContext &operator=(const JzContext &) = delete;

private:
    JzWindowSettings                   m_windowSettings;
    std::unique_ptr<JzResourceManager> m_resourceManager;
    std::unique_ptr<JzWindow>          m_window;
    std::unique_ptr<JzRHIDevice>       m_device;
    std::unique_ptr<JzInputManager>    m_inputManager;
    std::unique_ptr<JzUIManager>       m_uiManager;
    std::unique_ptr<JzSceneManager>    m_sceneManager;
    std::unique_ptr<JzRHICommandQueue> m_commandQueue;
    JzRenderFrontend*                  m_renderFrontend = nullptr;
    std::filesystem::path              m_workDirectory;
    std::filesystem::path              m_openDirectory;
};

} // namespace JzRE
