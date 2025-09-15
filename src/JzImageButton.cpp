
#include "JzImageButton.h"
#include <imgui.h>
#include "JzConverter.h"

JzRE::JzImageButton::JzImageButton(JzRE::U32 p_textureId, const JzRE::JzVec2 &p_size) :
    textureId(p_textureId),
    size(p_size),
    backgroundColor({0, 0, 0, 0}),
    textureColor({1, 1, 1, 1}) { }

void JzRE::JzImageButton::_Draw_Impl()
{
    const Bool isDisabled = disabled;

    if (isDisabled) {
        ImGui::BeginDisabled();
    }

    if (ImGui::ImageButton(m_widgetID.c_str(),
                           textureId,
                           JzConverter::ToImVec2(size),
                           ImVec2(0.f, 1.f),
                           ImVec2(1.f, 0.f),
                           JzConverter::ToImVec4(backgroundColor),
                           JzConverter::ToImVec4(textureColor))) {
        ClickedEvent.Invoke();
    }

    if (isDisabled) {
        ImGui::EndDisabled();
    }
}
