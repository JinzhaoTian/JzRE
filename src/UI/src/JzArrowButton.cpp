/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/UI/JzArrowButton.h"
#include <imgui.h>

JzRE::JzArrowButton::JzArrowButton(JzRE::JzEArrowDirection direction) :
    arrowDirection(direction) { }

void JzRE::JzArrowButton::_Draw_Impl()
{
    auto _mapping = [this](JzEArrowDirection p_direction) {
        switch (p_direction) {
            case JzEArrowDirection::NONE:
                return ImGuiDir_None;
            case JzEArrowDirection::LEFT:
                return ImGuiDir_Left;
            case JzEArrowDirection::RIGHT:
                return ImGuiDir_Right;
            case JzEArrowDirection::UP:
                return ImGuiDir_Up;
            case JzEArrowDirection::DOWN:
                return ImGuiDir_Down;
            default:
                return ImGuiDir_None;
        }
    };

    const bool isDisabled = disabled;

    if (isDisabled) {
        ImGui::BeginDisabled();
    }

    if (ImGui::ArrowButton(m_widgetID.c_str(), _mapping(arrowDirection))) {
        ClickedEvent.Invoke();
    }

    if (isDisabled) {
        ImGui::EndDisabled();
    }
}
