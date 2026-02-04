/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include <imgui.h>
#include "JzRE/Editor/Panels/JzView.h"
#include "JzRE/Runtime/Function/ECS/JzRenderSystem.h"
#include "JzRE/Runtime/Function/ECS/JzWorld.h"
#include "JzRE/Runtime/Core/JzServiceContainer.h"

JzRE::JzView::JzView(const JzRE::String &name, JzRE::Bool is_opened) :
    JzRE::JzPanelWindow(name, is_opened)
{
    m_frame        = &CreateWidget<JzFrame>();
    m_renderTarget = std::make_unique<JzRenderTarget>(name + "_RT");

    scrollable = false;
}

void JzRE::JzView::Update(JzRE::F32 deltaTime)
{
    // Base implementation - override in subclasses
}

void JzRE::JzView::Render()
{
    auto viewSize = GetSafeSize();

    // Skip if invalid size
    if (viewSize.x <= 0 || viewSize.y <= 0) {
        return;
    }

    // Ensure render target matches view size
    m_renderTarget->EnsureSize(viewSize);

    // Check if services are available
    if (!JzServiceContainer::Has<JzRenderSystem>() || !JzServiceContainer::Has<JzWorld>()) {
        return;
    }

    auto &renderSystem = JzServiceContainer::Get<JzRenderSystem>();
    auto &world        = JzServiceContainer::Get<JzWorld>();

    // Render scene to this view's render target
    renderSystem.RenderToTarget(world, *m_renderTarget, GetCameraEntity());

    // Update frame widget with rendered texture
    m_frame->frameTextureId = m_renderTarget->GetTextureID();
    m_frame->frameSize      = JzVec2(static_cast<F32>(viewSize.x), static_cast<F32>(viewSize.y));
}

JzRE::JzEntity JzRE::JzView::GetCameraEntity()
{
    // Default: use main camera
    return INVALID_ENTITY;
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
