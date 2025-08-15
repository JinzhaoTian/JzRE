#pragma once

#include "CommonTypes.h"
#include "JzRHIFactory.h"
#include "JzRHIDevice.h"
#include "JzWindow.h"
#include "JzWindowSettings.h"
#include "JzUIManager.h"
#include "JzCanvas.h"
#include "JzPanelWindow.h"
#include "JzButton.h"
#include "JzInputText.h"
#include "JzSpacing.h"
#include "JzSeparator.h"

namespace JzRE {

class JzREHubPanel;

/**
 * @brief JzRE Hub
 */
class JzREHub {
public:
    /**
     * @brief Constructor
     */
    JzREHub();

    /**
     * @brief Destructor
     */
    ~JzREHub();

    /**
     * @brief Run the JzRE Hub
     */
    std::optional<std::filesystem::path> Run();

private:
    std::unique_ptr<JzRHIDevice> m_device;
    std::unique_ptr<JzWindow>    m_window;
    std::unique_ptr<JzUIManager> m_uiManager;
    JzCanvas                     m_canvas;
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
     * @brief Implement of Draw Method
     */
    void Draw() override;

    /**
     * @brief Get Result
     */
    std::optional<std::filesystem::path> GetResult() const;

private:
    void _UpdateGoButton(const String &p_path);
    void _OnFailedToOpenCorruptedProject(const std::filesystem::path &p_projectPath);
    void _OnFailedToCreateProject(const std::filesystem::path &p_projectPath);
    void _ShowError(const String &p_title, const String &p_message);
    Bool _ValidateResult(const std::filesystem::path &p_result);
    Bool _TryFinish(const std::filesystem::path p_result);

private:
    std::optional<std::filesystem::path> m_result;
    JzButton                            *m_goButton = nullptr;
};

} // namespace JzRE