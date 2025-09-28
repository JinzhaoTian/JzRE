/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/UI/JzButton.h"
#include "JzRE/UI/JzConverter.h"

JzRE::JzButton::JzButton(const JzRE::String &label, JzRE::Bool isDisabled) :
    label(label),
    disabled(isDisabled)
{
    auto &style = ImGui::GetStyle();

    buttonIdleColor    = JzConverter::ImVec4ToImHex(style.Colors[ImGuiCol_Button]);
    buttonHoveredColor = JzConverter::ImVec4ToImHex(style.Colors[ImGuiCol_ButtonHovered]);
    buttonClickedColor = JzConverter::ImVec4ToImHex(style.Colors[ImGuiCol_ButtonActive]);
    buttonLabelColor   = JzConverter::ImVec4ToImHex(style.Colors[ImGuiCol_Text]);
}

void JzRE::JzButton::_Draw_Impl()
{
    auto &style = ImGui::GetStyle();

    auto defaultIdleColor    = style.Colors[ImGuiCol_Button];
    auto defaultHoveredColor = style.Colors[ImGuiCol_ButtonHovered];
    auto defaultClickedColor = style.Colors[ImGuiCol_ButtonActive];
    auto defaultTextColor    = style.Colors[ImGuiCol_Text];

    style.Colors[ImGuiCol_Button]        = JzConverter::HexToImVec4(buttonIdleColor);
    style.Colors[ImGuiCol_ButtonHovered] = JzConverter::HexToImVec4(buttonHoveredColor);
    style.Colors[ImGuiCol_ButtonActive]  = JzConverter::HexToImVec4(buttonClickedColor);
    style.Colors[ImGuiCol_Text]          = JzConverter::HexToImVec4(buttonLabelColor);

    const bool isDisabled = disabled;

    if (isDisabled) {
        ImGui::BeginDisabled();
    }

    if (ImGui::Button((label + m_widgetID).c_str(), JzConverter::ToImVec2(buttonSize))) {
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