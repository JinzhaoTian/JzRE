/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <filesystem>
#include <optional>
#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Function/ECS/JzWindowSystem.h"
#include "JzRE/Runtime/Platform/JzDevice.h"
#include "JzRE/Editor/JzUIManager.h"
#include "JzRE/Editor/JzCanvas.h"
#include "JzRE/Editor/UI/JzPanelWindow.h"
#include "JzRE/Editor/UI/JzButton.h"

namespace JzRE {

class JzREHubMenuBar;
class JzREHubPanel;

/**
 * @brief JzRE Hub
 */
class JzREHub {
public:
    /**
     * @brief Constructor
     *
     * @param rhiType
     */
    JzREHub(JzERHIType rhiType);

    /**
     * @brief Destructor
     */
    ~JzREHub();

    /**
     * @brief Run the JzRE Hub
     */
    std::optional<std::filesystem::path> Run();

private:
    std::unique_ptr<JzWindowSystem> m_windowSystem;
    std::unique_ptr<JzDevice>       m_device;
    std::unique_ptr<JzUIManager>    m_uiManager;
    std::unique_ptr<JzCanvas>       m_canvas;
    std::unique_ptr<JzREHubMenuBar> m_menuBar;
    std::unique_ptr<JzREHubPanel>   m_hubPanel;
};

/**
 * @brief JzRE Hub Panel Menu Bar
 */
class JzREHubMenuBar : public JzPanel {
public:
    /**
     * @brief Constructor
     */
    JzREHubMenuBar(JzWindowSystem &windowSystem);

protected:
    /**
     * @brief Implementation of the Draw method
     */
    void _Draw_Impl() override;

private:
    void HandleDragging();

private:
    JzWindowSystem &m_windowSystem;
    JzVec2    m_buttonSize;
    String    m_backgroudColor;
    Bool      m_isDragging;
    JzIVec2   m_dragStartMousePos;
    JzIVec2   m_dragStartWindowPos;
};

/**
 * @brief JzRE Hub Panel Window
 */
class JzREHubPanel : public JzPanelWindow {
public:
    /**
     * @brief Constructor
     */
    JzREHubPanel();

    /**
     * @brief Destructor
     */
    ~JzREHubPanel();

    /**
     * @brief Implementation of Draw Method
     */
    void Draw() override;

    /**
     * @brief Get Result
     */
    std::optional<std::filesystem::path> GetResult() const;

private:
    JzVec2                _CalculateContentSize();
    void                  _LoadHistory();
    void                  _SaveHistory();
    void                  _AddToHistory(const std::filesystem::path &path);
    void                  _DeleteFromHistory(const std::filesystem::path &path);
    String                _PathToUtf8(const std::filesystem::path &path) const;
    std::filesystem::path _Utf8ToPath(const String &utf8Str) const;
    void                  _OnUpdateGoButton(const String &path);
    void                  _OnFailedToOpenPath(const std::filesystem::path &path);
    Bool                  _OnFinish(const std::filesystem::path path);

private:
    std::optional<std::filesystem::path> m_result;
    JzButton                            *m_goButton        = nullptr;
    JzVec2                               m_windowSize      = {800.0f, 480.0f};
    JzVec2                               m_windowPosition  = {0.0f, 20.0f};
    String                               m_backgroudColor  = "#2A2A2A";
    JzVec2                               m_buttonSize      = {90.0f, 0.0f};
    F32                                  m_inputFieldWidth = 504.0f;
    std::vector<std::filesystem::path>   m_history;
    const Size                           m_maxHistorySize = 10;
    const std::filesystem::path          m_workspaceFilePath;
};

} // namespace JzRE
