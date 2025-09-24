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

    const auto cursor_pos = ImGui::GetCursorPos();

    // calculate the centered position for the icon
    ImGui::SetCursorPos(ImVec2(cursor_pos.x + (buttonSize.x() - iconSize.x()) * 0.5f,
                               cursor_pos.y + (buttonSize.y() - iconSize.y()) * 0.5f));

    if (m_texture) {
        ImGui::Image((ImTextureID)(uintptr_t)(m_texture->GetTextureID()),
                     JzConverter::ToImVec2(iconSize),
                     ImVec2(0.f, 0.f),
                     ImVec2(1.f, 1.f),
                     JzConverter::ToImVec4(iconColor),
                     ImVec4(0, 0, 0, 0));
    } else {
        ImGui::Dummy(JzConverter::ToImVec2(iconSize));
    }

    ImGui::SetCursorPos(cursor_pos);

    ImGui::PushStyleColor(ImGuiCol_Button, JzConverter::ToImVec4(idleBackgroundColor));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, JzConverter::ToImVec4(hoveredBackgroundColor));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, JzConverter::ToImVec4(clickedBackgroundColor));

    if (ImGui::Button("", JzConverter::ToImVec2(buttonSize))) {
        ClickedEvent.Invoke();
    }

    ImGui::PopStyleColor(3);

    ImGui::PopID();

    if (isDisabled) {
        ImGui::EndDisabled();
    }
}
