#include "JzTextClickable.h"

JzRE::JzTextClickable::JzTextClickable(const JzRE::String &p_content) :
    JzText(p_content) { }

void JzRE::JzTextClickable::_Draw_Impl()
{
    Bool useless = false;

    if (ImGui::Selectable((content + m_widgetID).c_str(), &useless, ImGuiSelectableFlags_AllowDoubleClick)) {
        if (ImGui::IsMouseDoubleClicked(0)) {
            DoubleClickedEvent.Invoke();
        } else {
            ClickedEvent.Invoke();
        }
    }
}
