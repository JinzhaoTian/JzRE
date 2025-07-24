#include "JzPanel.h"

JzRE::U64 JzRE::JzPanel::__PANEL_ID_INCREMENT = 0;

JzRE::JzPanel::JzPanel(const String &panelId) :
    m_panelId(panelId)
{
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
