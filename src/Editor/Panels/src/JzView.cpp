/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Editor/Panels/JzView.h"

#include <imgui.h>

#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Runtime/Function/ECS/JzRenderSystem.h"

namespace JzRE {

JzView::JzView(const String &name, Bool is_opened) :
    JzPanelWindow(name, is_opened),
    m_name(name)
{
    m_frame        = &CreateWidget<JzFrame>();
    m_renderTarget = std::make_unique<JzRenderTarget>(name + "_RT");

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
    if (m_registryHandle != JzRenderTargetRegistry::INVALID_HANDLE) {
        return;
    }

    if (!JzServiceContainer::Has<JzRenderSystem>()) {
        return;
    }

    auto &renderSystem = JzServiceContainer::Get<JzRenderSystem>();

    JzRenderTargetEntry entry;
    entry.target         = m_renderTarget.get();
    entry.camera         = GetCameraEntity();
    entry.includeEditor  = IncludeEditorOnly();
    entry.includePreview = IncludePreviewOnly();
    entry.shouldRender   = [this]() { return IsOpened() && IsVisible(); };
    entry.getDesiredSize = [this]() { return GetSafeSize(); };
    entry.name           = m_name;

    m_registryHandle = renderSystem.RegisterTarget(std::move(entry));
}

void JzView::UnregisterRenderTarget()
{
    if (m_registryHandle == JzRenderTargetRegistry::INVALID_HANDLE) {
        return;
    }

    if (!JzServiceContainer::Has<JzRenderSystem>()) {
        return;
    }

    auto &renderSystem = JzServiceContainer::Get<JzRenderSystem>();
    renderSystem.UnregisterTarget(m_registryHandle);
    m_registryHandle = JzRenderTargetRegistry::INVALID_HANDLE;
}

void JzView::UpdateFrameTexture()
{
    auto size = GetSafeSize();
    if (size.x > 0 && size.y > 0 && m_renderTarget && m_renderTarget->IsValid()) {
        m_frame->frameTextureId = m_renderTarget->GetTextureID();
        m_frame->frameSize      = JzVec2(static_cast<F32>(size.x), static_cast<F32>(size.y));
    }
}

} // namespace JzRE
