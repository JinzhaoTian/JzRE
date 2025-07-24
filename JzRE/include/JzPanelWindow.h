#pragma once

#include "JzPanel.h"

namespace JzRE {

class JzPanelWindow : public JzPanel {
public:
    /**
     * @brief Constructor
     *
     * @param panelId The ID of the panel window
     */
    JzPanelWindow(const String &panelId);

protected:
    void _Draw_Impl() override;
};

} // namespace JzRE