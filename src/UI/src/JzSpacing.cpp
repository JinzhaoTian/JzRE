/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/UI/JzSpacing.h"
#include <imgui.h>

JzRE::JzSpacing::JzSpacing(JzRE::U16 spaces) :
    spaces(spaces) { }

void JzRE::JzSpacing::_Draw_Impl()
{
    for (U16 i = 0; i < spaces; ++i) {
        ImGui::Spacing();

        if (i + 1 < spaces)
            ImGui::SameLine();
    }
}