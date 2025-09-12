/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzInputText.h"
#include <imgui.h>

JzRE::JzInputText::JzInputText(const JzRE::String &p_content, const JzRE::String &p_label) :
    JzDataWidget(content),
    content(p_content),
    label(p_label) { }

void JzRE::JzInputText::_Draw_Impl()
{
    String previousContent = content;

    content.resize(256, '\0');
    ImGui::SetNextItemWidth(width);
    Bool enterPressed = ImGui::InputText((label + m_widgetID).c_str(), &content[0], 256, ImGuiInputTextFlags_EnterReturnsTrue | (selectAllOnClick ? ImGuiInputTextFlags_AutoSelectAll : 0));
    content           = content.c_str();

    if (content != previousContent) {
        ContentChangedEvent.Invoke(content);
        this->NotifyChange();
    }

    if (enterPressed)
        EnterPressedEvent.Invoke(content);
}