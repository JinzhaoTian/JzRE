#pragma once

#include "JzRETypes.h"
#include "JzPanel.h"
#include "JzPanelWindow.h"
#include "JzMenuItem.h"
#include "JzMenuList.h"

namespace JzRE {
/**
 * @brief Menu Bar
 */
class JzMenuBar : public JzPanel {
public:
    /**
     * @brief Constructor
     */
    JzMenuBar();

    /**
     * @brief Handle the shortcuts
     *
     * @param deltaTime
     */
    void HandleShortcuts(F32 deltaTime);

    /**
     * @brief Register a panel
     *
     * @param name
     * @param panel
     */
    void RegisterPanel(const String &name, JzPanelWindow &panel);

    /**
     * @brief Initialize the settings menu
     */
    void InitializeSettingsMenu();

protected:
    /**
     * @brief Implementation of the Draw method
     */
    void _Draw_Impl() override;

private:
    void CreateFileMenu();
    void CreateBuildMenu();
    void CreateWindowMenu();
    void CreateActorsMenu();
    void CreateResourcesMenu();
    void CreateToolsMenu();
    void CreateSettingsMenu();
    void CreateLayoutMenu();
    void CreateHelpMenu();
    void CreateRunButton();

    void UpdateToggleableItems();
    void OpenEveryWindows(Bool state);

private:
    std::unordered_map<std::string, std::pair<std::reference_wrapper<JzPanelWindow>, std::reference_wrapper<JzMenuItem>>> m_panels;

    JzMenuList *m_settingsMenu = nullptr;
    JzMenuList *m_windowMenu   = nullptr;
};
} // namespace JzRE