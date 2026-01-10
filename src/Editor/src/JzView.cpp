/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include <imgui.h>
#include "JzRE/Editor/JzView.h"
#include "JzRE/Editor/JzRHIRenderer.h"
#include "JzRE/Core/JzServiceContainer.h"

JzRE::JzView::JzView(const JzRE::String &name, JzRE::Bool is_opened) :
    JzRE::JzPanelWindow(name, is_opened)
{
    m_frame = &CreateWidget<JzFrame>();

    scrollable = false;
}

void JzRE::JzView::Update(JzRE::F32 deltaTime)
{
    // TODO
}

void JzRE::JzView::Render()
{
    auto winSize = GetSafeSize();
    if (winSize.x() > 0 && winSize.y() > 0) {
        auto &renderer = JzServiceContainer::Get<JzRHIRenderer>();

        // Update frame size if necessary
        if (winSize.x() != renderer.GetCurrentFrameSize().x() || winSize.y() != renderer.GetCurrentFrameSize().y()) {
            renderer.SetFrameSize(winSize);
        }

        if (m_frame) {
            m_frame->frameSize = JzVec2(static_cast<F32>(winSize.x()), static_cast<F32>(winSize.y()));

            // Get the render texture from the renderer
            auto texture = renderer.GetCurrentTexture();
            if (texture) {
                m_frame->frameTextureId = texture->GetTextureID();
            }
        }
    }
}

void JzRE::JzView::_Draw_Impl()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    JzPanelWindow::_Draw_Impl();
    ImGui::PopStyleVar();
}

JzRE::JzIVec2 JzRE::JzView::GetSafeSize() const
{
    constexpr float kTitleBarHeight = 20.0f; // <--- this takes into account the imgui window title bar
    const auto     &size            = GetSize();
    return {
        static_cast<JzRE::I32>(size.x()),
        static_cast<JzRE::I32>(std::max(0.0f, size.y() - kTitleBarHeight))};
}
