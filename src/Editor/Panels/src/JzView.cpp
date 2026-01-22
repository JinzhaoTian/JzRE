/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include <imgui.h>
#include "JzRE/Editor/Panels/JzView.h"
#include "JzRE/Runtime/Function/ECS/JzEnttRenderSystem.h"
#include "JzRE/Runtime/Core/JzServiceContainer.h"

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

    // TODO
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
        static_cast<JzRE::I32>(size.x),
        static_cast<JzRE::I32>(std::max(0.0f, size.y - kTitleBarHeight))};
}
