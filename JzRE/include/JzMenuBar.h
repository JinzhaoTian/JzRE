#pragma once

#include "CommonTypes.h"
#include "JzPanelMenuBar.h"
#include "JzPanelWindow.h"

namespace JzRE {
/**
 * @brief A menu bar
 */
class JzMenuBar : public JzPanelMenuBar {
public:
    /**
     * @brief Constructor
     *
     * @param panelId The ID of the menu bar
     */
    JzMenuBar(const String &panelId);

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
};
} // namespace JzRE