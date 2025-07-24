#pragma once

#include "JzPanel.h"

namespace JzRE {
/**
 * @brief A menu bar panel
 */
class JzPanelMenuBar : public JzPanel {
public:
    /**
     * @brief Constructor
     * 
     * @param panelId The ID of the panel
     */
    JzPanelMenuBar(const String& panelId);

protected:
    /**
     * @brief Draw the menu bar
     */
    void _Draw_Impl() override;
};

} // namespace JzRE