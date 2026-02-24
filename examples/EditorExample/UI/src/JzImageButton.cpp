/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#include "JzRE/Editor/UI/JzImageButton.h"
#include <imgui.h>
#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Editor/UI/JzConverter.h"
#include "JzRE/Editor/UI/JzImGuiTextureBridge.h"

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
                           JzImGuiTextureBridge::Resolve(m_texture),
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
