/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/UI/JzIconButton.h"
#include <imgui.h>
#include "JzRE/UI/JzConverter.h"

JzRE::JzIconButton::JzIconButton(std::shared_ptr<JzRE::JzRHITexture> iconTexture) :
    m_iconTexture(iconTexture),
    buttonSize({20.0f, 20.0f}),
    buttonIdleColor("#2A2A2A"),
    buttonHoveredColor("#414243"),
    buttonClickedColor("#c2c2c2"),
    iconSize({12.0f, 12.0f}),
    iconIdleColor("#f3f3f3"),
    iconHoveredColor("#f3f3f3") { }

void JzRE::JzIconButton::_Draw_Impl()
{
    const Bool isDisabled = disabled;

    if (isDisabled) {
        ImGui::BeginDisabled();
    }

    ImGui::PushID(this);

    ImGui::PushStyleColor(ImGuiCol_Button, JzConverter::HexToImVec4(buttonIdleColor));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, JzConverter::HexToImVec4(buttonHoveredColor));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, JzConverter::HexToImVec4(buttonClickedColor));

    if (ImGui::Button(("##IconButton" + m_widgetID).c_str(), JzConverter::ToImVec2(buttonSize))) {
        ClickedEvent.Invoke();
    }

    const Bool isHovered = ImGui::IsItemHovered();

    const ImVec2 buttonMin  = ImGui::GetItemRectMin();
    const ImVec2 iconMinPos = ImVec2(buttonMin.x + (buttonSize.x() - iconSize.x()) * 0.5f,
                                     buttonMin.y + (buttonSize.y() - iconSize.y()) * 0.5f);
    const ImVec2 iconMaxPos = ImVec2(iconMinPos.x + iconSize.x(),
                                     iconMinPos.y + iconSize.y());

    if (m_iconTexture) {
        String finalIconColor = iconIdleColor;

        if (isHovered) {
            finalIconColor = iconHoveredColor;
        }

        ImGui::GetWindowDrawList()->AddImage(
            (ImTextureID)(uintptr_t)(m_iconTexture->GetTextureID()),
            iconMinPos,
            iconMaxPos,
            ImVec2(0.f, 0.f),
            ImVec2(1.f, 1.f),
            ImColor(JzConverter::HexToImVec4(finalIconColor)));
    }

    ImGui::PopStyleColor(3);

    ImGui::PopID();

    if (isDisabled) {
        ImGui::EndDisabled();
    }
}
