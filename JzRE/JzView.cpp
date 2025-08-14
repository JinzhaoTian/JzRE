#include "JzView.h"

JzRE::JzView::JzView(const JzRE::String &name, JzRE::Bool is_opened) :
    JzRE::JzPanelWindow(name, is_opened),
    m_cameraController(m_camera)
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

    m_image = &CreateWidget<JzImage>(m_texture->GetHandle(), JzVec2{0.f, 0.f});

    scrollable = false;
}

void JzRE::JzView::Update(JzRE::F32 deltaTime)
{
    m_cameraController.HandleInputs(deltaTime);

    // TODO
}

void JzRE::JzView::Render()
{
    auto [winWidth, winHeight] = GetSafeSize();
    auto camera                = GetCamera();
    auto scene                 = GetScene();

    if (winWidth > 0 && winHeight > 0 && camera && scene) {
        auto device = JzRE_DEVICE();

        // 开始帧（RHI）
        device->BeginFrame();

        // 尺寸变更则重建离屏纹理并重新挂载到帧缓冲（RHI）
        if (!m_texture || m_texture->GetWidth() != winWidth || m_texture->GetHeight() != winHeight) {
            JzTextureDesc textureDesc;
            textureDesc.type      = JzETextureType::Texture2D;
            textureDesc.format    = JzETextureFormat::RGBA8;
            textureDesc.width     = winWidth;
            textureDesc.height    = winHeight;
            textureDesc.mipLevels = 1;
            textureDesc.debugName = "ViewColor";
            m_texture             = device->CreateTexture(textureDesc);

            if (m_framebuffer) {
                m_framebuffer->AttachColorTexture(m_texture);
            }

            // Update image widget with new texture handle
            if (m_image && m_texture) {
                m_image->textureId = m_texture->GetHandle();
            }
        }

        // 绑定离屏帧缓冲并设置视口（RHI）
        device->BindFramebuffer(m_framebuffer);

        JzViewport viewport;
        viewport.x        = 0.0f;
        viewport.y        = 0.0f;
        viewport.width    = static_cast<F32>(winWidth);
        viewport.height   = static_cast<F32>(winHeight);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        device->SetViewport(viewport);

        // 清屏（RHI）
        JzClearParams clearParams;
        clearParams.clearColor   = true;
        clearParams.clearDepth   = true;
        clearParams.clearStencil = false;
        clearParams.colorR       = 1.0f;
        clearParams.colorG       = 0.1f;
        clearParams.colorB       = 0.1f;
        clearParams.colorA       = 1.0f;
        clearParams.depth        = 1.0f;
        clearParams.stencil      = 0;
        device->Clear(clearParams);

        // 场景渲染（RHI）
        m_renderer->RenderScene(scene);

        // 解绑离屏帧缓冲（回到默认帧缓冲）（RHI）
        device->BindFramebuffer(nullptr);

        // 结束帧（RHI）
        device->EndFrame();

        // 更新展示用控件尺寸和纹理（非图形 API 调用）
        if (m_image && m_texture) {
            m_image->size      = JzVec2{static_cast<F32>(winWidth), static_cast<F32>(winHeight)};
            m_image->textureId = m_texture->GetHandle();
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

std::pair<JzRE::U16, JzRE::U16> JzRE::JzView::GetSafeSize() const
{
    constexpr float kTitleBarHeight = 20.0f; // <--- this takes into account the imgui window title bar
    const auto     &size            = GetSize();
    return {
        static_cast<JzRE::U16>(size.x()),
        static_cast<JzRE::U16>(std::max(0.0f, size.y() - kTitleBarHeight))};
}

JzRE::JzCamera *JzRE::JzView::GetCamera()
{
    return &m_camera;
}

JzRE::JzScene *JzRE::JzView::GetScene()
{
    auto &sceneManager = EDITOR_CONTEXT(sceneManager);
    return sceneManager.GetCurrentScene();
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
