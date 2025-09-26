/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzPanel.h"

JzRE::U64 JzRE::JzPanel::__PANEL_ID_INCREMENT = 0;

JzRE::JzPanel::JzPanel()
{
    m_panelId = "##" + std::to_string(__PANEL_ID_INCREMENT++);
}

const JzRE::String &JzRE::JzPanel::GetPanelID() const
{
    return m_panelId;
}

void JzRE::JzPanel::Draw()
{
    if (enabled)
        _Draw_Impl();
}
