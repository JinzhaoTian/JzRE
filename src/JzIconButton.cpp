/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzIconButton.h"
#include <cstdint>
#include <imgui.h>
#include "JzConverter.h"

JzRE::JzIconButton::JzIconButton(std::shared_ptr<JzRE::JzRHITexture> texture) :
    m_texture(texture) { }

void JzRE::JzIconButton::_Draw_Impl()
{
    const Bool isDisabled = disabled;

    if (isDisabled) {
        ImGui::BeginDisabled();
    }

    ImGui::PushID(this);

    ImGui::PushStyleColor(ImGuiCol_Button, JzConverter::ToImVec4(idleBackgroundColor));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, JzConverter::ToImVec4(hoveredBackgroundColor));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, JzConverter::ToImVec4(clickedBackgroundColor));

    if (ImGui::Button(("##IconButton" + m_widgetID).c_str(), JzConverter::ToImVec2(buttonSize))) {
        ClickedEvent.Invoke();
    }

    const Bool isHovered = ImGui::IsItemHovered();

    const ImVec2 buttonMin  = ImGui::GetItemRectMin();
    const ImVec2 iconMinPos = ImVec2(buttonMin.x + (buttonSize.x() - iconSize.x()) * 0.5f,
                                     buttonMin.y + (buttonSize.y() - iconSize.y()) * 0.5f);
    const ImVec2 iconMaxPos = ImVec2(iconMinPos.x + iconSize.x(),
                                     iconMinPos.y + iconSize.y());

    if (m_texture) {
        JzVec4 finalIconColor = iconColor;

        if (isHovered) {
            finalIconColor = hoveredIconColor;
        }

        ImGui::GetWindowDrawList()->AddImage(
            (ImTextureID)(uintptr_t)(m_texture->GetTextureID()),
            iconMinPos,
            iconMaxPos,
            ImVec2(0.f, 0.f),
            ImVec2(1.f, 1.f),
            ImColor(JzConverter::ToImVec4(finalIconColor)));
    }

    ImGui::PopStyleColor(3);

    ImGui::PopID();

    if (isDisabled) {
        ImGui::EndDisabled();
    }
}
