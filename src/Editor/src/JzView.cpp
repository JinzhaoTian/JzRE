/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Editor/JzView.h"
#include <imgui.h>
#include "JzRE/Editor/JzContext.h"

JzRE::JzView::JzView(const JzRE::String &name, JzRE::Bool is_opened) :
    JzRE::JzPanelWindow(name, is_opened),
    m_cameraController(m_camera)
{
    m_image = &CreateWidget<JzImage>(0, JzVec2(0.f, 0.f));

    m_renderer = std::make_unique<JzRHIRenderer>();

    scrollable = false;
}

void JzRE::JzView::Update(JzRE::F32 deltaTime)
{
    m_cameraController.HandleInputs(deltaTime);

    // TODO
}

void JzRE::JzView::Render()
{
    if (!m_renderer->IsInitialized()) {
        m_renderer->Initialize();
    }

    auto winSize = GetSafeSize();
    auto scene   = GetScene();
    if (winSize.x() > 0 && winSize.y() > 0 && scene) {
        auto currentFrameSize = m_renderer->GetCurrentFrameSize();

        if (currentFrameSize != winSize) {
            m_renderer->SetFrameSize(winSize);
        }

        m_renderer->BeginFrame();
        m_renderer->RenderScene(scene);
        m_renderer->EndFrame();

        if (m_image) {
            auto currentTexture = m_renderer->GetCurrentTexture();
            m_image->size       = JzVec2(static_cast<F32>(winSize.x()), static_cast<F32>(winSize.y()));
            m_image->textureId  = (GLuint)(uintptr_t)currentTexture->GetTextureID();
        }
    }
}

void JzRE::JzView::_Draw_Impl()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    JzPanelWindow::_Draw_Impl();
    ImGui::PopStyleVar();
}

void JzRE::JzView::InitFrame()
{
    // TODO
}

void JzRE::JzView::DrawFrame()
{
    // TODO
}

JzRE::JzIVec2 JzRE::JzView::GetSafeSize() const
{
    constexpr float kTitleBarHeight = 20.0f; // <--- this takes into account the imgui window title bar
    const auto     &size            = GetSize();
    return {
        static_cast<JzRE::I32>(size.x()),
        static_cast<JzRE::I32>(std::max(0.0f, size.y() - kTitleBarHeight))};
}

JzRE::JzCamera *JzRE::JzView::GetCamera()
{
    return &m_camera;
}

JzRE::JzScene *JzRE::JzView::GetScene()
{
    return JzContext::GetInstance().GetSceneManager().GetCurrentScene();
}

void JzRE::JzView::ResetCameraTransform()
{
    // TODO
    // m_camera.transform->SetWorldPosition(kDefaultCameraPosition);
    // m_camera.transform->SetWorldRotation(kDefaultCameraRotation);
}

JzRE::JzCameraController &JzRE::JzView::GetCameraController()
{
    return m_cameraController;
}
