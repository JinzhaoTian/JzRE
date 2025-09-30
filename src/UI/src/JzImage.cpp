/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/UI/JzImage.h"
#include "JzRE/UI/JzConverter.h"

JzRE::JzImage::JzImage(std::shared_ptr<JzRE::JzGPUTextureObject> imageTexture, const JzRE::JzVec2 &imageSize) :
    imageTexture{imageTexture},
    imageSize(imageSize) { }

void JzRE::JzImage::_Draw_Impl()
{
    ImGui::Image((ImTextureID)(uintptr_t)imageTexture->GetTextureID(),
                 JzConverter::ToImVec2(imageSize),
                 ImVec2(0.f, 0.f),
                 ImVec2(1.f, 1.f));
}
