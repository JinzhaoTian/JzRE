#pragma once

#include "JzPanel.h"

namespace JzRE {
/**
 * @brief A menu bar panel
 */
class JzPanelMenuBar : public JzPanel {
protected:
    /**
     * @brief Draw the menu bar
     */
    void _Draw_Impl() override;
};

} // namespace JzRE