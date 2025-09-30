/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/UI/JzTexturePreview.h"
#include <imgui.h>
#include "JzRE/UI/JzConverter.h"

JzRE::JzTexturePreview::JzTexturePreview(std::shared_ptr<JzGPUTextureObject> texture) :
    m_texture(texture),
    m_textureSize({80.0f, 80.0f}) { }

void JzRE::JzTexturePreview::Execute(JzRE::JzEPluginExecutionContext context)
{
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();

        if (m_texture) {
            ImGui::Image((ImTextureID)(uintptr_t)m_texture->GetTextureID(),
                         JzConverter::ToImVec2(m_textureSize),
                         ImVec2(0.f, 1.f),
                         ImVec2(1.f, 0.f));
        } else {
            ImGui::Dummy(JzConverter::ToImVec2(m_textureSize));
        }

        ImGui::EndTooltip();
    }
}