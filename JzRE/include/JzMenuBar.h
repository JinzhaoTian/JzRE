#pragma once

#include "CommonTypes.h"
#include "JzMenuItem.h"
#include "JzMenuList.h"
#include "JzPanelMenuBar.h"
#include "JzPanelWindow.h"
#include "JzSeparator.h"
#include "JzText.h"

namespace JzRE {
/**
 * @brief Menu Bar
 */
class JzMenuBar : public JzPanelMenuBar {
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

    void UpdateToggleableItems();
    void OpenEveryWindows(Bool state);

private:
    std::unordered_map<std::string, std::pair<std::reference_wrapper<JzPanelWindow>, std::reference_wrapper<JzMenuItem>>> m_panels;

    JzMenuList *m_settingsMenu = nullptr;
    JzMenuList *m_windowMenu   = nullptr;
};
} // namespace JzRE