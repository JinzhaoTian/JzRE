/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzGroup.h"
#include <imgui.h>
#include "JzConverter.h"

JzRE::JzGroup::JzGroup(JzRE::JzEHorizontalAlignment p_horizontalAlignment, JzRE::JzVec2 p_size) :
    horizontalAlignment(p_horizontalAlignment),
    size{p_size} { }

void JzRE::JzGroup::_Draw_Impl()
{
    ImVec2 windowSize = ImGui::GetContentRegionAvail();
    ImVec2 groupSize  = JzConverter::ToImVec2(size);

    ImVec2 cursorPos = ImGui::GetCursorPos();
    switch (horizontalAlignment) {
        case JzEHorizontalAlignment::LEFT:
            break;
        case JzEHorizontalAlignment::CENTER:
            cursorPos.x += (windowSize.x - groupSize.x) * 0.5f;
            break;
        case JzEHorizontalAlignment::RIGHT:
            cursorPos.x += windowSize.x - groupSize.x;
            break;
    }

    ImGui::SetCursorPos(cursorPos);

    ImGui::BeginGroup();
    DrawWidgets();
    ImGui::EndGroup();
}