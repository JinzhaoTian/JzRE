/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/UI/JzContextMenu.h"
#include <imgui.h>

void JzRE::JzContextMenu::Execute(JzEPluginExecutionContext context)
{
    if (context == JzEPluginExecutionContext::PANEL ? ImGui::BeginPopupContextWindow() : ImGui::BeginPopupContextItem()) {
        DrawWidgets();
        ImGui::EndPopup();
    }
}

void JzRE::JzContextMenu::Close()
{
    ImGui::CloseCurrentPopup();
}