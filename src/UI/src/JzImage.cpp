/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/UI/JzImage.h"
#include "JzRE/UI/JzConverter.h"

JzRE::JzImage::JzImage(JzRE::U32 p_textureId, const JzRE::JzVec2 &p_size) :
    textureId{p_textureId},
    size(p_size) { }

void JzRE::JzImage::_Draw_Impl()
{
    ImGui::Image(textureId, JzConverter::ToImVec2(size), ImVec2(0.f, 1.f), ImVec2(1.f, 0.f));
}
