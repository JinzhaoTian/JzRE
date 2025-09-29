/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/UI/JzIcon.h"
#include <imgui.h>
#include "JzRE/UI/JzConverter.h"

JzRE::JzIcon::JzIcon(std::shared_ptr<JzRHITexture> iconTexture, const JzVec2 &iconSize) :
    m_iconTexture(iconTexture),
    m_iconSize(iconSize) { }

void JzRE::JzIcon::_Draw_Impl()
{
    ImGui::Image((ImTextureID)(uintptr_t)m_iconTexture->GetTextureID(),
                 JzConverter::ToImVec2(m_iconSize),
                 ImVec2(0.f, 0.f),
                 ImVec2(1.f, 1.f));
}