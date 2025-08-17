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
#include "JzGroup.h"
#include "JzText.h"
#include "JzInputText.h"
#include "JzSpacing.h"
#include "JzSeparator.h"
#include "JzColumns.h"
#include "JzOpenFileDialog.h"

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
    std::unique_ptr<JzRHIDevice>  m_device;
    std::unique_ptr<JzWindow>     m_window;
    std::unique_ptr<JzUIManager>  m_uiManager;
    JzCanvas                      m_canvas;
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
     * @brief Implementation of Draw Method
     */
    void Draw() override;

    /**
     * @brief Get Result
     */
    std::optional<std::filesystem::path> GetResult() const;

private:
    void _OnUpdateGoButton(const String &p_path);
    void _OnFailedToOpenPath(const std::filesystem::path &p_path);
    Bool _OnFinish(const std::filesystem::path p_result);

private:
    std::optional<std::filesystem::path> m_result;
    JzButton                            *m_goButton = nullptr;
};

} // namespace JzRE