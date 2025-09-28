/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/UI/JzMenuList.h"
#include <imgui.h>

JzRE::JzMenuList::JzMenuList(const String &name, Bool isLocked) :
    name(name),
    locked(isLocked) { }

void JzRE::JzMenuList::_Draw_Impl()
{
    if (ImGui::BeginMenu(name.c_str(), !locked)) {
        if (!m_opened) {
            ClickedEvent.Invoke();
            m_opened = true;
        }

        DrawWidgets();

        ImGui::EndMenu();
    } else {
        m_opened = false;
    }
}