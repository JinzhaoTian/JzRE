/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Editor/JzRHIRenderer.h"
#include "JzRE/RHI/JzDevice.h"
#include "JzRE/Core/JzServiceContainer.h"

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
    // TODO
}

JzRE::U32 JzRE::JzRHIRenderer::GetThreadCount() const
{
    return 1;
}

JzRE::Bool JzRE::JzRHIRenderer::CreateFramebuffer()
{
    auto &device = JzServiceContainer::Get<JzDevice>();

    // create default framebuffer
    m_framebuffer = device.CreateFramebuffer("RendererDefaultFB");
    if (!m_framebuffer) {
        return false;
    }

    // create default color texture
    JzGPUTextureObjectDesc colorDesc;
    colorDesc.type      = JzETextureResourceType::Texture2D;
    colorDesc.format    = JzETextureResourceFormat::RGBA8;
    colorDesc.width     = static_cast<U32>(m_frameSize.x());
    colorDesc.height    = static_cast<U32>(m_frameSize.y());
    colorDesc.debugName = "RendererColor";
    m_colorTexture      = device.CreateTexture(colorDesc);

    // bind color texture to framebuffer
    if (m_colorTexture) {
        m_framebuffer->AttachColorTexture(m_colorTexture, 0);
    }

    // create default depth texture
    JzGPUTextureObjectDesc depthDesc;
    depthDesc.type      = JzETextureResourceType::Texture2D;
    depthDesc.format    = JzETextureResourceFormat::Depth24;
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
    auto &device = JzServiceContainer::Get<JzDevice>();

    // Simple vertex shader with MVP transform
    const char *vsSrc = R"(
        #version 330 core

        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aNormal;
        layout (location = 2) in vec2 aTexCoords;

        out vec3 vNormal;
        out vec3 vWorldPos;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        void main()
        {
            vec4 worldPos = model * vec4(aPos, 1.0);
            vWorldPos = worldPos.xyz;
            vNormal = mat3(model) * aNormal;
            gl_Position = projection * view * worldPos;
        }
    )";

    // Simple fragment shader with basic diffuse lighting
    const char *fsSrc = R"(
        #version 330 core

        in vec3 vNormal;
        in vec3 vWorldPos;

        out vec4 FragColor;

        void main()
        {
            // Light direction (from above-front)
            vec3 lightDir = normalize(vec3(0.3, 1.0, 0.5));
            
            // Normalize the normal
            vec3 normal = normalize(vNormal);
            
            // Basic diffuse lighting
            float diff = max(dot(normal, lightDir), 0.0);
            
            // Ambient + diffuse
            vec3 ambient = vec3(0.2);
            vec3 diffuse = vec3(0.8) * diff;
            
            vec3 color = ambient + diffuse;
            FragColor = vec4(color, 1.0);
        }
    )";

    JzShaderProgramDesc vsDesc{};
    vsDesc.type       = JzEShaderProgramType::Vertex;
    vsDesc.source     = vsSrc;
    vsDesc.entryPoint = "main";
    vsDesc.debugName  = "RendererVS";

    JzShaderProgramDesc fsDesc{};
    fsDesc.type       = JzEShaderProgramType::Fragment;
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

    auto &device = JzServiceContainer::Get<JzDevice>();

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

    // Clear the screen (RHI)
    JzClearParams clearParams;
    clearParams.clearColor   = true;
    clearParams.clearDepth   = true;
    clearParams.clearStencil = false;
    clearParams.colorR       = 0.1f;
    clearParams.colorG       = 0.1f;
    clearParams.colorB       = 0.1f;
    clearParams.colorA       = 1.0f;
    clearParams.depth        = 1.0f;
    clearParams.stencil      = 0;
    device.Clear(clearParams);

    // Set up matrices
    JzMat4 modelMatrix = JzMat4x4::Identity();

    // Camera setup for Cornell Box:
    // The model is roughly at center (0, 2.5, -3), extends from about -3 to 2.5 in X, -0.2 to 5.3 in Y, -0.2 to -5.8 in Z
    // Camera positioned to view the scene from the front
    F32 aspect = m_frameSize.x() > 0 && m_frameSize.y() > 0 ? static_cast<F32>(m_frameSize.x()) / static_cast<F32>(m_frameSize.y()) : 1.0f;

    // Camera at (0, 2.5, 8) looking at (0, 2.5, -3) - positioned in front of the Cornell Box
    JzVec3 cameraPos    = JzVec3(0.0f, 2.5f, 8.0f);
    JzVec3 cameraTarget = JzVec3(0.0f, 2.5f, -3.0f);
    JzVec3 cameraUp     = JzVec3(0.0f, 1.0f, 0.0f);

    JzMat4 viewMatrix = JzMat4x4::LookAt(cameraPos, cameraTarget, cameraUp);
    JzMat4 projMatrix = JzMat4x4::Perspective(45.0f * 3.14159f / 180.0f, aspect, 0.1f, 100.0f);

    m_defaultPipeline->SetUniform("model", modelMatrix);
    m_defaultPipeline->SetUniform("view", viewMatrix);
    m_defaultPipeline->SetUniform("projection", projMatrix);

    // Render scene models
    for (const auto &model : scene->GetModels()) {
        if (!model) continue;

        // Iterate through all meshes in the model
        for (const auto &mesh : model->GetMeshes()) {
            if (!mesh) continue;

            auto vao = mesh->GetVertexArray();
            if (!vao) continue;

            // Bind vertex array
            device.BindVertexArray(vao);

            // Draw indexed
            JzDrawIndexedParams drawParams;
            drawParams.primitiveType = JzEPrimitiveType::Triangles;
            drawParams.indexCount    = mesh->GetIndexCount();
            drawParams.instanceCount = 1;
            drawParams.firstIndex    = 0;
            drawParams.vertexOffset  = 0;
            drawParams.firstInstance = 0;

            device.DrawIndexed(drawParams);
        }
    }

    device.BindFramebuffer(nullptr);
}

void JzRE::JzRHIRenderer::RenderWithCommandList(std::shared_ptr<JzRE::JzScene> scene)
{
    if (!scene) {
        return;
    }

    auto &device = JzServiceContainer::Get<JzDevice>();

    auto cmd = device.CreateCommandList("RendererCmdList");
    if (!cmd) {
        return;
    }

    cmd->Begin();

    if (m_defaultPipeline) {
        cmd->BindPipeline(m_defaultPipeline);
    }

    // // 遍历场景模型，录制绘制命令
    // for (const auto &model : scene->GetModels()) {
    //     if (model) {
    //         // For command list rendering, we need to record mesh draw commands
    //         for (const auto &mesh : model->GetMeshes()) {
    //             if (mesh.GetVertexArray() && mesh.GetIndexCount() > 0) {
    //                 // Bind vertex array
    //                 cmd->BindVertexArray(mesh.GetVertexArray());

    //                 // Bind textures
    //                 for (U32 i = 0; i < mesh.textures.size(); i++) {
    //                     cmd->BindTexture(mesh.textures[i], i);
    //                 }

    //                 // Record draw indexed command
    //                 JzDrawIndexedParams drawParams{};
    //                 drawParams.primitiveType = JzEPrimitiveType::Triangles;
    //                 drawParams.indexCount    = mesh.GetIndexCount();
    //                 drawParams.instanceCount = 1;
    //                 drawParams.firstIndex    = 0;
    //                 drawParams.vertexOffset  = 0;
    //                 drawParams.firstInstance = 0;

    //                 cmd->DrawIndexed(drawParams);
    //             }
    //         }
    //     }
    // }

    cmd->End();
    device.ExecuteCommandList(cmd);
}

void JzRE::JzRHIRenderer::SetupViewport()
{
    auto &device = JzServiceContainer::Get<JzDevice>();

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
    auto &device = JzServiceContainer::Get<JzDevice>();

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

std::shared_ptr<JzRE::JzGPUFramebufferObject> JzRE::JzRHIRenderer::GetFramebuffer() const
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
    auto &device = JzServiceContainer::Get<JzDevice>();
    device.BeginFrame();
}

void JzRE::JzRHIRenderer::EndFrame()
{
    auto &device = JzServiceContainer::Get<JzDevice>();
    device.EndFrame();
}

void JzRE::JzRHIRenderer::BindFramebuffer(std::shared_ptr<JzRE::JzGPUFramebufferObject> framebuffer)
{
    auto &device = JzServiceContainer::Get<JzDevice>();

    if (framebuffer) {
        device.BindFramebuffer(framebuffer);
    } else if (m_framebuffer) {
        device.BindFramebuffer(m_framebuffer);
    }
}

void JzRE::JzRHIRenderer::SetRenderState(const JzRE::JzRenderState &state)
{
    auto &device = JzServiceContainer::Get<JzDevice>();
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

std::shared_ptr<JzRE::JzGPUTextureObject> JzRE::JzRHIRenderer::GetCurrentTexture()
{
    return m_colorTexture;
}