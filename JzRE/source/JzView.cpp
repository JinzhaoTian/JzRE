#include "JzView.h"

JzRE::JzView::JzView(const JzRE::String &name, JzRE::Bool is_opened) :
    JzRE::JzPanelWindow(name, is_opened)
{
    scrollable = false;
}

void JzRE::JzView::Update(JzRE::F32 deltaTime) { }

void JzRE::JzView::Render()
{
    auto [winWidth, winHeight] = GetSafeSize();
    auto camera                = GetCamera();
    auto scene                 = GetScene();

    if (winWidth > 0 && winHeight > 0 && camera && scene) {
        // m_framebuffer.Resize(winWidth, winHeight);

        InitFrame();

        // OvRendering::Data::FrameDescriptor frameDescriptor;
        // frameDescriptor.renderWidth  = winWidth;
        // frameDescriptor.renderHeight = winHeight;
        // frameDescriptor.camera       = camera;
        // frameDescriptor.outputBuffer = m_framebuffer;

        // m_renderer->BeginFrame(frameDescriptor);

        DrawFrame();

        // m_renderer->EndFrame();

        // EDITOR_CONTEXT(driver)->OnFrameCompleted();
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
    // m_renderer->AddDescriptor<OvCore::Rendering::SceneRenderer::SceneDescriptor>(
    // 	CreateSceneDescriptor()
    // );
}

void JzRE::JzView::DrawFrame()
{
    // m_renderer->DrawFrame();
}

std::pair<JzRE::U16, JzRE::U16> JzRE::JzView::GetSafeSize() const
{
    constexpr float kTitleBarHeight = 25.0f; // <--- this takes into account the imgui window title bar
    // const auto     &size            = GetSize();
    return {
        static_cast<JzRE::U16>(480),
        static_cast<JzRE::U16>(320)};
}