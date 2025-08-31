#include "JzImage.h"

JzRE::JzImage::JzImage(JzRE::U32 p_textureId, const JzRE::JzVec2 &p_size) :
    textureId{p_textureId},
    size(p_size) { }

void JzRE::JzImage::_Draw_Impl()
{
    ImGui::Image(textureId, JzConverter::ToImVec2(size), ImVec2(0.f, 1.f), ImVec2(1.f, 0.f));
}
