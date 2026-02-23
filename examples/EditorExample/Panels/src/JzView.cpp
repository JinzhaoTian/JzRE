/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Editor/Panels/JzView.h"

#include <algorithm>
#include <imgui.h>

#include "JzRE/Runtime/Core/JzLogger.h"
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

    // Register render target at construction (including initially-closed panels).
    RegisterRenderTarget();
}

JzView::~JzView()
{
    UnregisterRenderTarget();
}

void JzView::Update(F32 deltaTime)
{
    (void)deltaTime;

    if (m_renderTargetHandle == INVALID_RENDER_TARGET_HANDLE || !JzServiceContainer::Has<JzRenderSystem>()) {
        return;
    }

    auto &renderSystem = JzServiceContainer::Get<JzRenderSystem>();
    renderSystem.UpdateRenderTargetVisibility(m_renderTargetHandle, GetVisibility());
    renderSystem.UpdateRenderTargetFeatures(m_renderTargetHandle, GetRenderFeatures());
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
    if (m_renderTargetHandle != INVALID_RENDER_TARGET_HANDLE) {
        return;
    }

    if (!JzServiceContainer::Has<JzRenderSystem>()) {
        JzRE_LOG_ERROR("JzView::RegisterRenderTarget() called before JzRenderSystem is available (panel={})", m_name);
        return;
    }

    auto &renderSystem = JzServiceContainer::Get<JzRenderSystem>();

    JzRenderTargetDesc desc;
    desc.name           = m_name;
    desc.camera         = GetCameraEntity();
    desc.visibility     = GetVisibility();
    desc.features       = GetRenderFeatures();
    desc.shouldRender   = [this]() { return IsOpened() && IsVisible(); };
    desc.getDesiredSize = [this]() { return GetSafeSize(); };

    m_renderTargetHandle = renderSystem.RegisterRenderTarget(std::move(desc));
}

void JzView::UnregisterRenderTarget()
{
    if (m_renderTargetHandle == INVALID_RENDER_TARGET_HANDLE) {
        return;
    }

    if (!JzServiceContainer::Has<JzRenderSystem>()) {
        return;
    }

    auto &renderSystem = JzServiceContainer::Get<JzRenderSystem>();
    renderSystem.UnregisterRenderTarget(m_renderTargetHandle);
    m_renderTargetHandle = INVALID_RENDER_TARGET_HANDLE;
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
    auto *output       = renderSystem.GetRenderOutput(m_renderTargetHandle);
    if (!output || !output->IsValid()) {
        return;
    }

    m_frame->frameTextureId = output->GetTextureID();
    m_frame->frameTexture   = output->GetColorTexture();
    m_frame->frameSize      = JzVec2(static_cast<F32>(size.x), static_cast<F32>(size.y));
}

} // namespace JzRE
