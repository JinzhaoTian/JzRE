#include "JzRE/Editor/UI/JzFrame.h"
#include "JzRE/Editor/UI/JzConverter.h"
#include "JzRE/Editor/UI/JzImGuiTextureBridge.h"
#include <imgui.h>

JzRE::JzFrame::JzFrame() { }

void JzRE::JzFrame::_Draw_Impl()
{
    ImTextureID textureId{};
    if (frameTexture) {
        textureId = JzImGuiTextureBridge::Resolve(frameTexture);
    } else {
        textureId = JzImGuiTextureBridge::ResolveRaw(frameTextureId);
    }

    if (!textureId) {
        ImGui::Dummy(JzConverter::ToImVec2(frameSize));
        return;
    }

    ImGui::Image(textureId,
                 JzConverter::ToImVec2(frameSize),
                 ImVec2(0.f, 0.f),
                 ImVec2(1.f, 1.f));
}
