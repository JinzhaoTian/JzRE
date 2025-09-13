/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRHIRenderer.h"
#include "JzContext.h"

JzRE::JzRHIRenderer::JzRHIRenderer() { }

JzRE::JzRHIRenderer::~JzRHIRenderer()
{
    CleanupResources();
}

void JzRE::JzRHIRenderer::RenderScene(JzRE::JzScene *scene)
{
    if (!scene) {
        return;
    }

    if (m_frameSizeChanged) {
        CreateFramebuffer();
        m_frameSizeChanged = false;
    }

    if (!m_isInitialized) {
        CreateDefaultPipeline();
        m_isInitialized = true;
    }

    // 由上层（如 View）控制 BeginFrame/BindFramebuffer/SetViewport/Clear
    auto scenePtr = std::shared_ptr<JzRE::JzScene>(scene, [](JzRE::JzScene *) { });

    if (m_useCommandList) {
        RenderWithCommandList(scenePtr);
    } else {
        RenderImmediate(scenePtr);
    }
}

void JzRE::JzRHIRenderer::SetUseCommandList(JzRE::Bool useCommandList)
{
    m_useCommandList = useCommandList;
}

JzRE::Bool JzRE::JzRHIRenderer::IsUsingCommandList() const
{
    return m_useCommandList;
}

void JzRE::JzRHIRenderer::SetThreadCount(JzRE::U32 threadCount)
{
    auto &queue = JzRE_CONTEXT().GetCommandQueue();
    queue.SetThreadCount(threadCount);
}

JzRE::U32 JzRE::JzRHIRenderer::GetThreadCount() const
{
    auto &queue = JzRE_CONTEXT().GetCommandQueue();
    return queue.GetThreadCount();
}

JzRE::Bool JzRE::JzRHIRenderer::CreateFramebuffer()
{
    auto &device = JzRE_DEVICE();

    // create default framebuffer
    m_framebuffer = device.CreateFramebuffer("RendererDefaultFB");
    if (!m_framebuffer) {
        return false;
    }

    // create default color texture
    JzTextureDesc colorDesc;
    colorDesc.type      = JzETextureType::Texture2D;
    colorDesc.format    = JzETextureFormat::RGBA8;
    colorDesc.width     = static_cast<U32>(m_frameSize.x());
    colorDesc.height    = static_cast<U32>(m_frameSize.y());
    colorDesc.debugName = "RendererColor";
    m_colorTexture      = device.CreateTexture(colorDesc);

    // bind color texture to framebuffer
    if (m_colorTexture) {
        m_framebuffer->AttachColorTexture(m_colorTexture, 0);
    }

    // create default depth texture
    JzTextureDesc depthDesc;
    depthDesc.type      = JzETextureType::Texture2D;
    depthDesc.format    = JzETextureFormat::Depth24;
    depthDesc.width     = static_cast<U32>(m_frameSize.x());
    depthDesc.height    = static_cast<U32>(m_frameSize.y());
    depthDesc.debugName = "RendererDepth";
    m_depthTexture      = device.CreateTexture(depthDesc);

    // bind depth texture to framebuffer
    if (m_depthTexture) {
        m_framebuffer->AttachDepthTexture(m_depthTexture);
    }

    return true;
}

JzRE::Bool JzRE::JzRHIRenderer::CreateDefaultPipeline()
{
    auto &device = JzRE_DEVICE();

    const char *vsSrc = R"(
        #version 330 core

        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aNormal;
        layout (location = 2) in vec2 aTexCoords;

        out vec3 FragPos;
        out vec3 Normal;
        out vec2 TexCoords;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        void main()
        {
            FragPos = vec3(model * vec4(aPos, 1.0));
            Normal = mat3(transpose(inverse(model))) * aNormal;  
            TexCoords = aTexCoords;
            
            gl_Position = projection * view * vec4(FragPos, 1.0);
        }
    )";

    const char *fsSrc = R"( 
        #version 330 core

        in vec3 FragPos;
        in vec3 Normal;
        in vec2 TexCoords;

        out vec4 FragColor;

        void main()
        {
            FragColor = vec4(0.1, 1.0, 0.1, 1.0);
        }
    )";

    JzShaderDesc vsDesc{};
    vsDesc.type       = JzEShaderType::Vertex;
    vsDesc.source     = vsSrc;
    vsDesc.entryPoint = "main";
    vsDesc.debugName  = "RendererVS";

    JzShaderDesc fsDesc{};
    fsDesc.type       = JzEShaderType::Fragment;
    fsDesc.source     = fsSrc;
    fsDesc.entryPoint = "main";
    fsDesc.debugName  = "RendererFS";

    JzPipelineDesc pipeDesc{};
    pipeDesc.shaders               = {vsDesc, fsDesc};
    pipeDesc.renderState.depthTest = true;
    pipeDesc.renderState.cullMode  = JzECullMode::Back;
    pipeDesc.debugName             = "RendererDefaultPipeline";

    m_defaultPipeline = device.CreatePipeline(pipeDesc);
    return m_defaultPipeline != nullptr;
}

void JzRE::JzRHIRenderer::CleanupResources()
{
    m_defaultPipeline.reset();
    m_depthTexture.reset();
    m_colorTexture.reset();
    m_framebuffer.reset();
    m_isInitialized = false;
}

void JzRE::JzRHIRenderer::RenderImmediate(std::shared_ptr<JzRE::JzScene> scene)
{
    if (!scene) {
        return;
    }

    auto &device = JzRE_DEVICE();

    // bind frame buffer
    device.BindFramebuffer(m_framebuffer);

    // bind pipeline
    device.BindPipeline(m_defaultPipeline);

    JzViewport viewport;
    viewport.x        = 0.0f;
    viewport.y        = 0.0f;
    viewport.width    = static_cast<F32>(m_frameSize.x());
    viewport.height   = static_cast<F32>(m_frameSize.y());
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    device.SetViewport(viewport);

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
    device.Clear(clearParams);

    JzMat4 modelMatrix = JzMat4x4::Identity();
    m_defaultPipeline->SetUniform("model", modelMatrix);
    m_defaultPipeline->SetUniform("view", modelMatrix);
    m_defaultPipeline->SetUniform("projection", modelMatrix);

    // render scene
    for (const auto &model : scene->GetModels()) {
        if (model) {
            model->Draw(m_defaultPipeline);
        }
    }

    device.BindFramebuffer(nullptr);
}

void JzRE::JzRHIRenderer::RenderWithCommandList(std::shared_ptr<JzRE::JzScene> scene)
{
    if (!scene) {
        return;
    }

    auto &device = JzRE_DEVICE();

    auto cmd = device.CreateCommandList("RendererCmdList");
    if (!cmd) {
        return;
    }

    cmd->Begin();

    if (m_defaultPipeline) {
        cmd->BindPipeline(m_defaultPipeline);
    }

    // 遍历场景模型，录制绘制命令
    for (const auto &model : scene->GetModels()) {
        if (model) {
            // For command list rendering, we need to record mesh draw commands
            for (const auto &mesh : model->GetMeshes()) {
                if (mesh.GetVertexArray() && mesh.GetIndexCount() > 0) {
                    // Bind vertex array
                    cmd->BindVertexArray(mesh.GetVertexArray());

                    // Bind textures
                    for (U32 i = 0; i < mesh.textures.size(); i++) {
                        cmd->BindTexture(mesh.textures[i], i);
                    }

                    // Record draw indexed command
                    JzDrawIndexedParams drawParams{};
                    drawParams.primitiveType = JzEPrimitiveType::Triangles;
                    drawParams.indexCount    = mesh.GetIndexCount();
                    drawParams.instanceCount = 1;
                    drawParams.firstIndex    = 0;
                    drawParams.vertexOffset  = 0;
                    drawParams.firstInstance = 0;

                    cmd->DrawIndexed(drawParams);
                }
            }
        }
    }

    cmd->End();
    device.ExecuteCommandList(cmd);
}

void JzRE::JzRHIRenderer::SetupViewport()
{
    auto &device = JzRE_DEVICE();

    if (m_colorTexture) {
        JzViewport vp{};
        vp.x        = 0.0f;
        vp.y        = 0.0f;
        vp.width    = static_cast<F32>(m_colorTexture->GetWidth());
        vp.height   = static_cast<F32>(m_colorTexture->GetHeight());
        vp.minDepth = 0.0f;
        vp.maxDepth = 1.0f;
        device.SetViewport(vp);
    }
}

void JzRE::JzRHIRenderer::ClearBuffers()
{
    auto &device = JzRE_DEVICE();

    JzClearParams clearParams{};
    clearParams.clearColor   = true;
    clearParams.clearDepth   = true;
    clearParams.clearStencil = false;
    clearParams.colorR       = 0.1f;
    clearParams.colorG       = 0.1f;
    clearParams.colorB       = 0.1f;
    clearParams.colorA       = 1.0f;
    clearParams.depth        = 1.0f;

    device.Clear(clearParams);
}

std::shared_ptr<JzRE::JzRHIFramebuffer> JzRE::JzRHIRenderer::GetFramebuffer() const
{
    return m_framebuffer;
}

std::shared_ptr<JzRE::JzRHIPipeline> JzRE::JzRHIRenderer::GetDefaultPipeline() const
{
    return m_defaultPipeline;
}

JzRE::Bool JzRE::JzRHIRenderer::IsInitialized() const
{
    return m_isInitialized;
}

JzRE::Bool JzRE::JzRHIRenderer::Initialize()
{
    if (m_isInitialized) {
        return true;
    }

    Bool success  = true;
    success      &= CreateFramebuffer();
    success      &= CreateDefaultPipeline();

    m_isInitialized = success;
    return success;
}

void JzRE::JzRHIRenderer::BeginFrame()
{
    auto &device = JzRE_DEVICE();
    device.BeginFrame();
}

void JzRE::JzRHIRenderer::EndFrame()
{
    auto &device = JzRE_DEVICE();
    device.EndFrame();
}

void JzRE::JzRHIRenderer::BindFramebuffer(std::shared_ptr<JzRE::JzRHIFramebuffer> framebuffer)
{
    auto &device = JzRE_DEVICE();

    if (framebuffer) {
        device.BindFramebuffer(framebuffer);
    } else if (m_framebuffer) {
        device.BindFramebuffer(m_framebuffer);
    }
}

void JzRE::JzRHIRenderer::SetRenderState(const JzRE::JzRenderState &state)
{
    auto &device = JzRE_DEVICE();
    device.SetRenderState(state);
}

void JzRE::JzRHIRenderer::SetFrameSize(JzRE::JzIVec2 p_size)
{
    m_frameSize        = p_size;
    m_frameSizeChanged = true;
}

JzRE::JzIVec2 JzRE::JzRHIRenderer::GetCurrentFrameSize() const
{
    return m_frameSize;
}

std::shared_ptr<JzRE::JzRHITexture> JzRE::JzRHIRenderer::GetCurrentTexture()
{
    return m_colorTexture;
}