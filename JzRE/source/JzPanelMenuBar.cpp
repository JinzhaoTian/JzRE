#include "JzPanelMenuBar.h"

void JzRE::JzPanelMenuBar::_Draw_Impl()
{
    if (!m_widgets.empty() && ImGui::BeginMainMenuBar()) {
        DrawWidgets();
        ImGui::EndMainMenuBar();
    }
}