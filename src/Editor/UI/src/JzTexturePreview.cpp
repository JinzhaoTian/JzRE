/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Editor/UI/JzTexturePreview.h"
#include <imgui.h>
#include "JzRE/Editor/UI/JzConverter.h"
#include "JzRE/Editor/UI/JzImGuiTextureBridge.h"

JzRE::JzTexturePreview::JzTexturePreview(std::shared_ptr<JzGPUTextureObject> texture) :
    m_texture(texture),
    m_textureSize({80.0f, 80.0f}) { }

void JzRE::JzTexturePreview::Execute(JzRE::JzEPluginExecutionContext context)
{
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();

        if (m_texture) {
            ImGui::Image(JzImGuiTextureBridge::Resolve(m_texture),
                         JzConverter::ToImVec2(m_textureSize),
                         ImVec2(0.f, 1.f),
                         ImVec2(1.f, 0.f));
        } else {
            ImGui::Dummy(JzConverter::ToImVec2(m_textureSize));
        }

        ImGui::EndTooltip();
    }
}
