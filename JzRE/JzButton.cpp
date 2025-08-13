#include "JzButton.h"

JzRE::JzButton::JzButton(const JzRE::String &label, JzRE::Bool disabled) :
    label(label), disabled(disabled)
{
    auto &style = ImGui::GetStyle();
}

void JzRE::JzButton::_Draw_Impl()
{
    auto &style = ImGui::GetStyle();

    auto defaultIdleColor    = style.Colors[ImGuiCol_Button];
    auto defaultHoveredColor = style.Colors[ImGuiCol_ButtonHovered];
    auto defaultClickedColor = style.Colors[ImGuiCol_ButtonActive];
    auto defaultTextColor    = style.Colors[ImGuiCol_Text];

    const bool isDisabled = disabled;

    if (isDisabled) {
        ImGui::BeginDisabled();
    }

    ImVec2 size;
    if (ImGui::Button((label + m_widgetID).c_str(), size)) {
        ClickedEvent.Invoke();
    }

    if (isDisabled) {
        ImGui::EndDisabled();
    }
}