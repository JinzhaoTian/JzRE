/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include <cmath>
#include "JzRE/Runtime/Function/Rendering/JzRHIRenderer.h"
#include "JzRE/Runtime/Platform/JzDevice.h"
#include "JzRE/Runtime/Core/JzServiceContainer.h"

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

    // Vertex shader with MVP transform
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

    // Fragment shader with material support and basic lighting
    const char *fsSrc = R"(
        #version 330 core

        in vec3 vNormal;
        in vec3 vWorldPos;

        out vec4 FragColor;

        // Material properties
        uniform vec3 uAmbientColor;
        uniform vec3 uDiffuseColor;
        uniform vec3 uSpecularColor;
        uniform float uShininess;

        // Camera position for specular
        uniform vec3 uCameraPos;

        void main()
        {
            // Light direction (from above-front)
            vec3 lightDir = normalize(vec3(0.3, 1.0, 0.5));
            vec3 lightColor = vec3(1.0);
            
            // Normalize the normal
            vec3 normal = normalize(vNormal);
            
            // Ambient
            vec3 ambient = uAmbientColor * 0.3;
            
            // Diffuse
            float diff = max(dot(normal, lightDir), 0.0);
            vec3 diffuse = uDiffuseColor * diff * lightColor;
            
            // Specular (Blinn-Phong)
            vec3 viewDir = normalize(uCameraPos - vWorldPos);
            vec3 halfwayDir = normalize(lightDir + viewDir);
            float spec = pow(max(dot(normal, halfwayDir), 0.0), uShininess);
            vec3 specular = uSpecularColor * spec * lightColor * 0.5;
            
            vec3 color = ambient + diffuse + specular;
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

    // Camera setup - get from scene's main camera
    F32 aspect = m_frameSize.x() > 0 && m_frameSize.y() > 0 ? static_cast<F32>(m_frameSize.x()) / static_cast<F32>(m_frameSize.y()) : 1.0f;

    JzVec3 cameraPos(0.0f, 0.0f, 10.0f);
    JzVec3 cameraTarget(0.0f, 0.0f, 0.0f);
    JzVec3 cameraUp(0.0f, 1.0f, 0.0f);
    F32    fov = 60.0f;

    auto *camera = scene->FindMainCamera();
    if (camera) {
        cameraPos = camera->GetPosition();
        fov       = camera->GetFov();

        // Get rotation (stored as pitch, yaw, roll, unused in x, y, z, w)
        JzVec4 rotation = camera->GetRotation();
        F32    pitch    = rotation.x(); // Rotation around X axis
        F32    yaw      = rotation.y(); // Rotation around Y axis

        // Calculate forward direction from pitch and yaw
        // Forward = (sin(yaw) * cos(pitch), sin(pitch), -cos(yaw) * cos(pitch))
        F32 cosPitch = std::cos(pitch);
        F32 sinPitch = std::sin(pitch);
        F32 cosYaw   = std::cos(yaw);
        F32 sinYaw   = std::sin(yaw);

        JzVec3 forward(sinYaw * cosPitch, sinPitch, -cosYaw * cosPitch);
        cameraTarget = cameraPos + forward;

        // Calculate right vector and correct up vector
        JzVec3 worldUp(0.0f, 1.0f, 0.0f);
        JzVec3 right = forward.Cross(worldUp).Normalized();
        cameraUp     = right.Cross(forward).Normalized();
    }

    JzMat4 viewMatrix = JzMat4x4::LookAt(cameraPos, cameraTarget, cameraUp);
    JzMat4 projMatrix = JzMat4x4::Perspective(fov * 3.14159f / 180.0f, aspect, 0.1f, 100.0f);

    m_defaultPipeline->SetUniform("model", modelMatrix);
    m_defaultPipeline->SetUniform("view", viewMatrix);
    m_defaultPipeline->SetUniform("projection", projMatrix);
    m_defaultPipeline->SetUniform("uCameraPos", cameraPos);

    // Default material colors
    JzVec3 defaultAmbient   = JzVec3(0.2f, 0.2f, 0.2f);
    JzVec3 defaultDiffuse   = JzVec3(0.8f, 0.8f, 0.8f);
    JzVec3 defaultSpecular  = JzVec3(0.5f, 0.5f, 0.5f);
    F32    defaultShininess = 32.0f;

    // Render scene models
    for (const auto &model : scene->GetModels()) {
        if (!model) continue;

        const auto &materials = model->GetMaterials();

        // Iterate through all meshes in the model
        for (const auto &mesh : model->GetMeshes()) {
            if (!mesh) continue;

            auto vao = mesh->GetVertexArray();
            if (!vao) continue;

            // Set material uniforms based on mesh's material index
            I32 matIndex = mesh->GetMaterialIndex();
            if (matIndex >= 0 && matIndex < static_cast<I32>(materials.size())) {
                const auto &mat = materials[matIndex];
                if (mat) {
                    const auto &props = mat->GetProperties();
                    m_defaultPipeline->SetUniform("uAmbientColor", props.ambientColor);
                    m_defaultPipeline->SetUniform("uDiffuseColor", props.diffuseColor);
                    m_defaultPipeline->SetUniform("uSpecularColor", props.specularColor);
                    m_defaultPipeline->SetUniform("uShininess", props.shininess);
                } else {
                    // Default material
                    m_defaultPipeline->SetUniform("uAmbientColor", defaultAmbient);
                    m_defaultPipeline->SetUniform("uDiffuseColor", defaultDiffuse);
                    m_defaultPipeline->SetUniform("uSpecularColor", defaultSpecular);
                    m_defaultPipeline->SetUniform("uShininess", defaultShininess);
                }
            } else {
                // No material, use defaults
                m_defaultPipeline->SetUniform("uAmbientColor", defaultAmbient);
                m_defaultPipeline->SetUniform("uDiffuseColor", defaultDiffuse);
                m_defaultPipeline->SetUniform("uSpecularColor", defaultSpecular);
                m_defaultPipeline->SetUniform("uShininess", defaultShininess);
            }

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

void JzRE::JzRHIRenderer::BlitToScreen(U32 screenWidth, U32 screenHeight)
{
    if (!m_framebuffer) {
        return;
    }

    auto &device = JzServiceContainer::Get<JzDevice>();
    device.BlitFramebufferToScreen(m_framebuffer,
                                   static_cast<U32>(m_frameSize.x()),
                                   static_cast<U32>(m_frameSize.y()),
                                   screenWidth, screenHeight);
}