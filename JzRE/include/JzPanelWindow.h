#pragma once

#include "JzPanel.h"

namespace JzRE {

class JzPanelWindow : public JzPanel {
public:
    /**
     * @brief Constructor
     */
    JzPanelWindow();

protected:
    void _Draw_Impl() override;
};

} // namespace JzRE