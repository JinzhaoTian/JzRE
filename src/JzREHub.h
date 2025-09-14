/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>
#include <optional>
#include <filesystem>
#include <vector>
#include "JzRETypes.h"
#include "JzRHIDevice.h"
#include "JzWindow.h"
#include "JzUIManager.h"
#include "JzCanvas.h"
#include "JzPanelWindow.h"
#include "JzButton.h"

namespace JzRE {

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
    std::unique_ptr<JzWindow>     m_window;
    std::unique_ptr<JzRHIDevice>  m_device;
    std::unique_ptr<JzUIManager>  m_uiManager;
    std::unique_ptr<JzCanvas>     m_canvas;
    std::unique_ptr<JzREHubPanel> m_hubPanel;
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
    void                  _LoadHistory();
    void                  _SaveHistory();
    void                  _AddToHistory(const std::filesystem::path &path);
    void                  _DeleteFromHistory(const std::filesystem::path &path);
    String                _PathToUtf8(const std::filesystem::path &path) const;
    std::filesystem::path _Utf8ToPath(const String &utf8Str) const;
    void                  _OnUpdateGoButton(const String &p_path);
    void                  _OnFailedToOpenPath(const std::filesystem::path &p_path);
    Bool                  _OnFinish(const std::filesystem::path p_result);

private:
    std::optional<std::filesystem::path> m_result;
    JzButton                            *m_goButton        = nullptr;
    JzVec2                               m_windowSize      = {800.0f, 500.0f};
    JzVec2                               m_windowPosition  = {0.0f, 0.0f};
    JzVec2                               m_buttonSize      = {90.0f, 0.0f};
    F32                                  m_inputFieldWidth = 504.0f;
    std::vector<std::filesystem::path>   m_history;
    const Size                           m_maxHistorySize = 10;
    const std::filesystem::path          m_workspaceFilePath;
};

} // namespace JzRE