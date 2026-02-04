/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Editor/Panels/JzView.h"

#include <imgui.h>

#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Runtime/Function/ECS/JzRenderSystem.h"
#include "JzRE/Runtime/Function/Rendering/JzRenderOutput.h"

namespace JzRE {

JzView::JzView(const String &name, Bool is_opened) :
    JzPanelWindow(name, is_opened),
    m_name(name)
{
    m_frame    = &CreateWidget<JzFrame>();
    scrollable = false;

    // Register render target if view is opened
    if (is_opened) {
        RegisterRenderTarget();
    }
}

JzView::~JzView()
{
    UnregisterRenderTarget();
}

void JzView::Update(F32 deltaTime)
{
    // Base implementation - override in subclasses
}

JzEntity JzView::GetCameraEntity()
{
    // Default: use main camera
    return INVALID_ENTITY;
}

String JzView::GetPassName() const
{
    return m_name + "Pass";
}

String JzView::GetOutputName() const
{
    return m_name + "_Color";
}

void JzView::_Draw_Impl()
{
    UpdateFrameTexture();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    JzPanelWindow::_Draw_Impl();
    ImGui::PopStyleVar();
}

JzIVec2 JzView::GetSafeSize() const
{
    constexpr float kTitleBarHeight = 20.0f;
    const auto     &size            = GetSize();
    return {
        static_cast<I32>(size.x),
        static_cast<I32>(std::max(0.0f, size.y - kTitleBarHeight))};
}

void JzView::RegisterRenderTarget()
{
    if (m_viewHandle != JzRenderSystem::INVALID_VIEW_HANDLE) {
        return;
    }

    if (!JzServiceContainer::Has<JzRenderSystem>()) {
        return;
    }

    auto &renderSystem = JzServiceContainer::Get<JzRenderSystem>();

    JzRenderSystem::JzRenderViewDesc desc;
    desc.name           = m_name;
    desc.passName       = GetPassName();
    desc.outputName     = GetOutputName();
    desc.camera         = GetCameraEntity();
    desc.includeEditor  = IncludeEditorOnly();
    desc.includePreview = IncludePreviewOnly();
    desc.shouldRender   = [this]() { return IsOpened() && IsVisible(); };
    desc.getDesiredSize = [this]() { return GetSafeSize(); };

    m_viewHandle = renderSystem.RegisterView(std::move(desc));
}

void JzView::UnregisterRenderTarget()
{
    if (m_viewHandle == JzRenderSystem::INVALID_VIEW_HANDLE) {
        return;
    }

    if (!JzServiceContainer::Has<JzRenderSystem>()) {
        return;
    }

    auto &renderSystem = JzServiceContainer::Get<JzRenderSystem>();
    renderSystem.UnregisterView(m_viewHandle);
    m_viewHandle = JzRenderSystem::INVALID_VIEW_HANDLE;
}

void JzView::UpdateFrameTexture()
{
    auto size = GetSafeSize();
    if (size.x <= 0 || size.y <= 0) {
        return;
    }

    if (!JzServiceContainer::Has<JzRenderSystem>()) {
        return;
    }

    auto &renderSystem = JzServiceContainer::Get<JzRenderSystem>();
    auto *output       = renderSystem.GetRenderOutput(GetOutputName());
    if (!output || !output->IsValid()) {
        return;
    }

    m_frame->frameTextureId = output->GetTextureID();
    m_frame->frameSize      = JzVec2(static_cast<F32>(size.x), static_cast<F32>(size.y));
}

} // namespace JzRE
