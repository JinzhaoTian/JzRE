#include "JzView.h"

JzRE::JzView::JzView(const JzRE::String &name, JzRE::Bool is_opened) :
    JzRE::JzPanelWindow(name, is_opened)
{
    auto device   = JzRE_DEVICE();
    m_framebuffer = device->CreateFramebuffer();

    // Create texture
    JzTextureDesc textureDesc;
    textureDesc.type      = JzETextureType::Texture2D;
    textureDesc.format    = JzETextureFormat::RGBA8;
    textureDesc.width     = 256;
    textureDesc.height    = 256;
    textureDesc.debugName = "TestScene";
    m_texture             = device->CreateTexture(textureDesc);

    m_framebuffer->AttachColorTexture(m_texture);

    U32 texId = 0; // TODO texture handle

    m_image    = &CreateWidget<JzImage>(texId, JzVec2{0.f, 0.f});
    scrollable = false;
}

void JzRE::JzView::Update(JzRE::F32 deltaTime) { }

void JzRE::JzView::Render()
{
    auto [winWidth, winHeight] = GetSafeSize();
    auto camera                = GetCamera();
    auto scene                 = GetScene();

    if (winWidth > 0 && winHeight > 0 /* && camera && scene*/) {
        auto device = JzRE_DEVICE();

        device->BeginFrame();

        m_renderer->RenderScene(scene); // TODO render scene

        device->EndFrame();
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