/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/UI/JzIcon.h"
#include <imgui.h>
#include "JzRE/UI/JzConverter.h"

JzRE::JzIcon::JzIcon(std::shared_ptr<JzRHITexture> texture, const JzVec2 &size) :
    m_texture(texture),
    m_textureSize(size) { }

void JzRE::JzIcon::_Draw_Impl()
{
    ImGui::Image((ImTextureID)(uintptr_t)m_texture->GetTextureID(),
                 JzConverter::ToImVec2(m_textureSize),
                 ImVec2(0.f, 1.f),
                 ImVec2(1.f, 0.f));
}