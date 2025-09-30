
#include "JzRE/UI/JzImageButton.h"
#include <imgui.h>
#include "JzRE/Core/JzRETypes.h"
#include "JzRE/UI/JzConverter.h"

JzRE::JzImageButton::JzImageButton(std::shared_ptr<JzGPUTextureObject> texture, const JzVec2 &size) :
    m_texture(texture),
    buttonSize(size),
    buttonIdleColor("#000000"),
    buttonTextureColor("#f3f3f3") { }

void JzRE::JzImageButton::_Draw_Impl()
{
    const Bool isDisabled = disabled;

    if (isDisabled) {
        ImGui::BeginDisabled();
    }

    if (ImGui::ImageButton(m_widgetID.c_str(),
                           (ImTextureID)(uintptr_t)m_texture->GetTextureID(),
                           JzConverter::ToImVec2(buttonSize),
                           ImVec2(0.f, 1.f),
                           ImVec2(1.f, 0.f),
                           JzConverter::HexToImVec4(buttonIdleColor),
                           JzConverter::HexToImVec4(buttonTextureColor))) {
        ClickedEvent.Invoke();
    }

    if (isDisabled) {
        ImGui::EndDisabled();
    }
}
