/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzText.h"
#include <imgui.h>

JzRE::JzText::JzText(const String &p_content) :
    JzDataWidget(content),
    content(p_content) { }

void JzRE::JzText::_Draw_Impl()
{
    ImGui::Text("%s", content.c_str());
}