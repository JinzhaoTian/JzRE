#pragma once

#include "JzPanel.h"

namespace JzRE {
/**
 * @brief Menu Bar Panel
 */
class JzPanelMenuBar : public JzPanel {
protected:
    /**
     * @brief Implementation of the Draw method
     */
    void _Draw_Impl() override;
};

} // namespace JzRE