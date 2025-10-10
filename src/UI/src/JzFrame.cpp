#include "JzRE/UI/JzFrame.h"
#include "JzRE/UI/JzConverter.h"
#include <imgui.h>

JzRE::JzFrame::JzFrame() { }

void JzRE::JzFrame::_Draw_Impl()
{
    ImGui::Image((ImTextureID)(uintptr_t)frameTextureId,
                 JzConverter::ToImVec2(frameSize),
                 ImVec2(0.f, 0.f),
                 ImVec2(1.f, 1.f));
}
