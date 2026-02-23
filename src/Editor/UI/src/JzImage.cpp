/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Editor/UI/JzImage.h"
#include "JzRE/Editor/UI/JzConverter.h"
#include "JzRE/Editor/UI/JzImGuiTextureBridge.h"

JzRE::JzImage::JzImage(std::shared_ptr<JzRE::JzGPUTextureObject> imageTexture, const JzRE::JzVec2 &imageSize) :
    imageTexture{imageTexture},
    imageSize(imageSize) { }

void JzRE::JzImage::_Draw_Impl()
{
    ImGui::Image(JzImGuiTextureBridge::Resolve(imageTexture),
                 JzConverter::ToImVec2(imageSize),
                 ImVec2(0.f, 0.f),
                 ImVec2(1.f, 1.f));
}
