
#include "JzImageButton.h"
#include <imgui.h>
#include "JzRETypes.h"
#include "JzConverter.h"

JzRE::JzImageButton::JzImageButton(std::shared_ptr<JzRHITexture> p_texture, const JzVec2 &p_size) :
    m_texture(p_texture),
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
                           (ImTextureID)(uintptr_t)m_texture->GetTextureID(),
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
