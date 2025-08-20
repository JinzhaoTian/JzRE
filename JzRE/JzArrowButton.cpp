#include "JzArrowButton.h"

JzRE::JzArrowButton::JzArrowButton(JzRE::JzEArrowDirection p_direction, JzRE::JzEAlign p_align) :
    arrowDirection(p_direction),
    buttonAlign(p_align) { }

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
        }
    };

    const bool isDisabled = disabled;

    if (isDisabled) {
        ImGui::BeginDisabled();
    }

    switch (buttonAlign) {
        case JzEAlign::LEFT:
            break;
        case JzEAlign::CENTER:
            ImGui::SameLine(ImGui::GetWindowWidth() * 0.5f - 15);
            break;
        case JzEAlign::RIGHT:
            ImGui::SameLine(ImGui::GetWindowWidth() - 30);
            break;
    }

    if (ImGui::ArrowButton(m_widgetID.c_str(), _mapping(arrowDirection))) {
        ClickedEvent.Invoke();
    }

    if (isDisabled) {
        ImGui::EndDisabled();
    }
}
