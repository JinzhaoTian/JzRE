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
    // auto camera                = GetCamera();
    // auto scene                 = GetScene();

    if (winWidth > 0 && winHeight > 0 /* && camera && scene*/) {
        auto device = JzRE_DEVICE();

        // Create vertex buffer
        F32 vertices[] = {
            -0.5f, -0.5f, 0.0f, // bottom left
            0.5f, -0.5f, 0.0f,  // bottom right
            0.0f, 0.5f, 0.0f    // top
        };

        JzBufferDesc vertexBufferDesc;
        vertexBufferDesc.type      = JzEBufferType::Vertex;
        vertexBufferDesc.usage     = JzEBufferUsage::StaticDraw;
        vertexBufferDesc.size      = sizeof(vertices);
        vertexBufferDesc.data      = vertices;
        vertexBufferDesc.debugName = "TriangleVertexBuffer";

        auto vertexBuffer = device->CreateBuffer(vertexBufferDesc);
        std::cout << "Create vertex buffer: " << vertexBuffer->GetDebugName() << std::endl;

        // Create index buffer
        U32 indices[] = {0, 1, 2};

        JzBufferDesc indexBufferDesc;
        indexBufferDesc.type      = JzEBufferType::Index;
        indexBufferDesc.usage     = JzEBufferUsage::StaticDraw;
        indexBufferDesc.size      = sizeof(indices);
        indexBufferDesc.data      = indices;
        indexBufferDesc.debugName = "TriangleIndexBuffer";

        auto indexBuffer = device->CreateBuffer(indexBufferDesc);
        std::cout << "Create index buffer: " << indexBuffer->GetDebugName() << std::endl;

        // Create texture
        JzTextureDesc textureDesc;
        textureDesc.type      = JzETextureType::Texture2D;
        textureDesc.format    = JzETextureFormat::RGBA8;
        textureDesc.width     = 256;
        textureDesc.height    = 256;
        textureDesc.debugName = "TestTexture";

        auto texture = device->CreateTexture(textureDesc);
        std::cout << "Create texture: " << texture->GetDebugName()
                  << " (" << texture->GetWidth() << "x" << texture->GetHeight() << ")" << std::endl;

        // Create shader
        JzShaderDesc vertexShaderDesc;
        vertexShaderDesc.type      = JzEShaderType::Vertex;
        vertexShaderDesc.source    = R"(
            #version 330 core
            layout (location = 0) in vec3 aPos;
            void main() {
                gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
            }
        )";
        vertexShaderDesc.debugName = "BasicVertexShader";

        JzShaderDesc fragmentShaderDesc;
        fragmentShaderDesc.type      = JzEShaderType::Fragment;
        fragmentShaderDesc.source    = R"(
            #version 330 core
            out vec4 FragColor;
            void main() {
                FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
            }
        )";
        fragmentShaderDesc.debugName = "BasicFragmentShader";

        auto vertexShader   = device->CreateShader(vertexShaderDesc);
        auto fragmentShader = device->CreateShader(fragmentShaderDesc);
        std::cout << "Create shader: " << vertexShader->GetDebugName()
                  << " & " << fragmentShader->GetDebugName() << std::endl;

        // Create pipeline
        JzPipelineDesc pipelineDesc;
        pipelineDesc.shaders               = {vertexShaderDesc, fragmentShaderDesc};
        pipelineDesc.renderState.depthTest = true;
        pipelineDesc.renderState.cullMode  = JzECullMode::Back;
        pipelineDesc.debugName             = "BasicPipeline";

        auto pipeline = device->CreatePipeline(pipelineDesc);
        std::cout << "Create pipeline: " << pipeline->GetDebugName() << std::endl;

        // Create vertex array object
        auto vertexArray = device->CreateVertexArray("TriangleVAO");
        vertexArray->BindVertexBuffer(vertexBuffer);
        vertexArray->BindIndexBuffer(indexBuffer);
        vertexArray->SetVertexAttribute(0, 3, 3 * sizeof(F32), 0);
        std::cout << "Create vertex array object: " << vertexArray->GetDebugName() << std::endl;

        // 3. Immediate Rendering Mode Example
        std::cout << "\n=== Immediate Rendering Mode Example ===" << std::endl;

        device->BeginFrame();

        // Clear screen
        JzClearParams clearParams;
        clearParams.clearColor = true;
        clearParams.clearDepth = true;
        clearParams.colorR     = 0.2f;
        clearParams.colorG     = 0.3f;
        clearParams.colorB     = 0.3f;
        clearParams.colorA     = 1.0f;
        device->Clear(clearParams);

        // Set viewport
        JzViewport viewport;
        viewport.x      = 0.0f;
        viewport.y      = 0.0f;
        viewport.width  = winWidth;
        viewport.height = winHeight;
        device->SetViewport(viewport);

        // Bind resources and draw
        device->BindPipeline(pipeline);
        device->BindVertexArray(vertexArray);
        device->BindTexture(texture, 0);

        JzDrawIndexedParams drawParams;
        drawParams.indexCount    = 3;
        drawParams.primitiveType = JzEPrimitiveType::Triangles;
        device->DrawIndexed(drawParams);

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