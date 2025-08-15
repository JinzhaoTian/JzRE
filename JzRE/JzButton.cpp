#include "JzButton.h"

JzRE::JzButton::JzButton(const JzRE::String &label, JzRE::Bool disabled) :
    label(label),
    disabled(disabled)
{
    auto &style = ImGui::GetStyle();

    idleBackgroundColor    = JzConverter::ToJzVec4(style.Colors[ImGuiCol_Button]);
    hoveredBackgroundColor = JzConverter::ToJzVec4(style.Colors[ImGuiCol_ButtonHovered]);
    clickedBackgroundColor = JzConverter::ToJzVec4(style.Colors[ImGuiCol_ButtonActive]);
    textColor              = JzConverter::ToJzVec4(style.Colors[ImGuiCol_Text]);
}

void JzRE::JzButton::_Draw_Impl()
{
    auto &style = ImGui::GetStyle();

    auto defaultIdleColor    = style.Colors[ImGuiCol_Button];
    auto defaultHoveredColor = style.Colors[ImGuiCol_ButtonHovered];
    auto defaultClickedColor = style.Colors[ImGuiCol_ButtonActive];
    auto defaultTextColor    = style.Colors[ImGuiCol_Text];

    style.Colors[ImGuiCol_Button]        = JzConverter::ToImVec4(idleBackgroundColor);
    style.Colors[ImGuiCol_ButtonHovered] = JzConverter::ToImVec4(hoveredBackgroundColor);
    style.Colors[ImGuiCol_ButtonActive]  = JzConverter::ToImVec4(clickedBackgroundColor);
    style.Colors[ImGuiCol_Text]          = JzConverter::ToImVec4(textColor);

    const bool isDisabled = disabled;

    if (isDisabled) {
        ImGui::BeginDisabled();
    }

    if (ImGui::Button((label + m_widgetID).c_str(), JzConverter::ToImVec2(size))) {
        ClickedEvent.Invoke();
    }

    if (isDisabled) {
        ImGui::EndDisabled();
    }

    style.Colors[ImGuiCol_Button]        = defaultIdleColor;
    style.Colors[ImGuiCol_ButtonHovered] = defaultHoveredColor;
    style.Colors[ImGuiCol_ButtonActive]  = defaultClickedColor;
    style.Colors[ImGuiCol_Text]          = defaultTextColor;
}