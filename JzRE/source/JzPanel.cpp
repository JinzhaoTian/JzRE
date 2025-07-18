#include "JzPanel.h"

JzRE::U64 JzRE::JzPanel::__PANEL_ID_INCREMENT = 0;

JzRE::JzPanel::JzPanel(const String &id)
{
    m_panelId = "Panel_" + std::to_string(__PANEL_ID_INCREMENT++);
}

const JzRE::String &JzRE::JzPanel::GetPanelID() const
{
    return m_panelId;
}

void JzRE::JzPanel::Draw()
{
    if (enabled)
        Draw_Impl();
}
