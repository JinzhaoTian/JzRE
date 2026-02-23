/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Editor/UI/JzIcon.h"
#include <imgui.h>
#include "JzRE/Editor/UI/JzConverter.h"
#include "JzRE/Editor/UI/JzImGuiTextureBridge.h"

JzRE::JzIcon::JzIcon(std::shared_ptr<JzGPUTextureObject> iconTexture, const JzVec2 &iconSize) :
    m_iconTexture(iconTexture),
    m_iconSize(iconSize) { }

void JzRE::JzIcon::_Draw_Impl()
{
    ImGui::Image(JzImGuiTextureBridge::Resolve(m_iconTexture),
                 JzConverter::ToImVec2(m_iconSize),
                 ImVec2(0.f, 0.f),
                 ImVec2(1.f, 1.f));
}
