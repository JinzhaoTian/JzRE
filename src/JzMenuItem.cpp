/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzMenuItem.h"
#include <imgui.h>

JzRE::JzMenuItem::JzMenuItem(const String &name, const String &shortcut, Bool is_checkable, Bool is_checked) :
    JzDataWidget(m_selected),
    name(name),
    shortcut(shortcut),
    checkable(is_checkable),
    checked(is_checked) { }

void JzRE::JzMenuItem::_Draw_Impl()
{
    JzRE::Bool previousValue = checked;

    if (ImGui::MenuItem((name + m_widgetID).c_str(), shortcut.c_str(), checkable ? &checked : nullptr, enabled))
        ClickedEvent.Invoke();

    if (checked != previousValue) {
        ValueChangedEvent.Invoke(checked);
        this->NotifyChange();
    }
}