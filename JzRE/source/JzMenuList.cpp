#include "JzMenuList.h"

JzRE::JzMenuList::JzMenuList(const String &name, Bool is_locked) :
    name(name), locked(is_locked) { }

void JzRE::JzMenuList::_Draw_Impl()
{
    if (ImGui::BeginMenu(name.c_str(), !locked)) {
        if (!m_opened) {
            clickedEvent.Invoke();
            m_opened = true;
        }

        DrawWidgets();

        ImGui::EndMenu();
    } else {
        m_opened = false;
    }
}