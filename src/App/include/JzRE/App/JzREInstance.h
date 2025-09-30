/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <filesystem>
#include "JzRE/Core/JzRETypes.h"
#include "JzRE/RHI/JzRHIDevice.h"
#include "JzRE/Resource/JzResourceManager.h"
#include "JzRE/Editor/JzInputManager.h"
#include "JzRE/Editor/JzSceneManager.h"
#include "JzRE/Editor/JzUIManager.h"
#include "JzRE/Editor/JzWindow.h"
#include "JzRE/Editor/JzEditor.h"

namespace JzRE {
/**
 * @brief Jz Render Engine
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
    std::unique_ptr<JzResourceManager> m_resourceManager;
    std::unique_ptr<JzWindow>          m_window;
    std::unique_ptr<JzRHIDevice>       m_device;
    std::unique_ptr<JzInputManager>    m_inputManager;
    std::unique_ptr<JzUIManager>       m_uiManager;
    std::unique_ptr<JzSceneManager>    m_sceneManager;
    std::unique_ptr<JzEditor>          m_editor;
};
} // namespace JzRE