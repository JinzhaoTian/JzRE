/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Function/ECS/JzEnttRenderSystem.h"

#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Runtime/Function/ECS/JzEnttCameraSystem.h"
#include "JzRE/Runtime/Function/ECS/JzEnttLightSystem.h"
#include "JzRE/Runtime/Function/ECS/JzComponent.h"
#include "JzRE/Runtime/Platform/JzDevice.h"
#include "JzRE/Runtime/Resource/JzMaterial.h"
#include "JzRE/Runtime/Resource/JzMesh.h"

namespace JzRE {

JzEnttRenderSystem::JzEnttRenderSystem() { }

void JzEnttRenderSystem::OnInit(JzEnttWorld &world)
{
    // Resources will be created lazily in Update
}

void JzEnttRenderSystem::Update(JzEnttWorld &world, F32 delta)
{
    // Create/recreate framebuffer if size changed
    if (m_frameSizeChanged) {
        CreateFramebuffer();
        m_frameSizeChanged = false;
    }

    // Create pipeline if not initialized
    if (!m_isInitialized) {
        CreateDefaultPipeline();
        m_isInitialized = true;
    }

    // Setup viewport and clear
    SetupViewportAndClear();

    // Render all entities
    RenderEntities(world);

    // Unbind framebuffer
    auto &device = JzServiceContainer::Get<JzDevice>();
    device.BindFramebuffer(nullptr);
}

void JzEnttRenderSystem::OnShutdown(JzEnttWorld &world)
{
    CleanupResources();
}

void JzEnttRenderSystem::SetCameraSystem(std::shared_ptr<JzEnttCameraSystem> cameraSystem)
{
    m_cameraSystem = std::move(cameraSystem);
}

void JzEnttRenderSystem::SetLightSystem(std::shared_ptr<JzEnttLightSystem> lightSystem)
{
    m_lightSystem = std::move(lightSystem);
}

void JzEnttRenderSystem::SetFrameSize(JzIVec2 size)
{
    if (m_frameSize != size) {
        m_frameSize        = size;
        m_frameSizeChanged = true;
    }
}

JzIVec2 JzEnttRenderSystem::GetCurrentFrameSize() const
{
    return m_frameSize;
}

std::shared_ptr<JzGPUFramebufferObject> JzEnttRenderSystem::GetFramebuffer() const
{
    return m_framebuffer;
}

std::shared_ptr<JzGPUTextureObject> JzEnttRenderSystem::GetColorTexture() const
{
    return m_colorTexture;
}

std::shared_ptr<JzGPUTextureObject> JzEnttRenderSystem::GetDepthTexture() const
{
    return m_depthTexture;
}

std::shared_ptr<JzRHIPipeline> JzEnttRenderSystem::GetDefaultPipeline() const
{
    return m_defaultPipeline;
}

void JzEnttRenderSystem::BeginFrame()
{
    auto &device = JzServiceContainer::Get<JzDevice>();
    device.BeginFrame();
}

void JzEnttRenderSystem::EndFrame()
{
    auto &device = JzServiceContainer::Get<JzDevice>();
    device.EndFrame();
}

void JzEnttRenderSystem::BlitToScreen(U32 screenWidth, U32 screenHeight)
{
    if (!m_framebuffer) {
        return;
    }

    auto &device = JzServiceContainer::Get<JzDevice>();
    device.BlitFramebufferToScreen(m_framebuffer, static_cast<U32>(m_frameSize.x),
                                   static_cast<U32>(m_frameSize.y), screenWidth, screenHeight);
}

Bool JzEnttRenderSystem::IsInitialized() const
{
    return m_isInitialized;
}

Bool JzEnttRenderSystem::CreateFramebuffer()
{
    auto &device = JzServiceContainer::Get<JzDevice>();

    // Create framebuffer
    m_framebuffer = device.CreateFramebuffer("ECSRendererFB");
    if (!m_framebuffer) {
        return false;
    }

    // Create color texture
    JzGPUTextureObjectDesc colorDesc;
    colorDesc.type      = JzETextureResourceType::Texture2D;
    colorDesc.format    = JzETextureResourceFormat::RGBA8;
    colorDesc.width     = static_cast<U32>(m_frameSize.x);
    colorDesc.height    = static_cast<U32>(m_frameSize.y);
    colorDesc.debugName = "ECSRendererColor";
    m_colorTexture      = device.CreateTexture(colorDesc);

    if (m_colorTexture) {
        m_framebuffer->AttachColorTexture(m_colorTexture, 0);
    }

    // Create depth texture
    JzGPUTextureObjectDesc depthDesc;
    depthDesc.type      = JzETextureResourceType::Texture2D;
    depthDesc.format    = JzETextureResourceFormat::Depth24;
    depthDesc.width     = static_cast<U32>(m_frameSize.x);
    depthDesc.height    = static_cast<U32>(m_frameSize.y);
    depthDesc.debugName = "ECSRendererDepth";
    m_depthTexture      = device.CreateTexture(depthDesc);

    if (m_depthTexture) {
        m_framebuffer->AttachDepthTexture(m_depthTexture);
    }

    return true;
}

Bool JzEnttRenderSystem::CreateDefaultPipeline()
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

        // Light properties
        uniform vec3 uLightDir;
        uniform vec3 uLightColor;

        // Camera position for specular
        uniform vec3 uCameraPos;

        void main()
        {
            // Normalize the normal
            vec3 normal = normalize(vNormal);

            // Ambient
            vec3 ambient = uAmbientColor * 0.3;

            // Diffuse
            float diff = max(dot(normal, uLightDir), 0.0);
            vec3 diffuse = uDiffuseColor * diff * uLightColor;

            // Specular (Blinn-Phong)
            vec3 viewDir = normalize(uCameraPos - vWorldPos);
            vec3 halfwayDir = normalize(uLightDir + viewDir);
            float spec = pow(max(dot(normal, halfwayDir), 0.0), uShininess);
            vec3 specular = uSpecularColor * spec * uLightColor * 0.5;

            vec3 color = ambient + diffuse + specular;
            FragColor = vec4(color, 1.0);
        }
    )";

    JzShaderProgramDesc vsDesc{};
    vsDesc.type       = JzEShaderProgramType::Vertex;
    vsDesc.source     = vsSrc;
    vsDesc.entryPoint = "main";
    vsDesc.debugName  = "ECSRendererVS";

    JzShaderProgramDesc fsDesc{};
    fsDesc.type       = JzEShaderProgramType::Fragment;
    fsDesc.source     = fsSrc;
    fsDesc.entryPoint = "main";
    fsDesc.debugName  = "ECSRendererFS";

    JzPipelineDesc pipeDesc{};
    pipeDesc.shaders               = {vsDesc, fsDesc};
    pipeDesc.renderState.depthTest = true;
    pipeDesc.renderState.cullMode  = JzECullMode::Back;
    pipeDesc.debugName             = "ECSRendererDefaultPipeline";

    m_defaultPipeline = device.CreatePipeline(pipeDesc);
    return m_defaultPipeline != nullptr;
}

void JzEnttRenderSystem::SetupViewportAndClear()
{
    auto &device = JzServiceContainer::Get<JzDevice>();

    // Bind framebuffer
    device.BindFramebuffer(m_framebuffer);

    // Bind pipeline
    device.BindPipeline(m_defaultPipeline);

    // Set viewport
    JzViewport viewport;
    viewport.x        = 0.0f;
    viewport.y        = 0.0f;
    viewport.width    = static_cast<F32>(m_frameSize.x);
    viewport.height   = static_cast<F32>(m_frameSize.y);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    device.SetViewport(viewport);

    // Get clear color from camera system
    JzVec3 clearColor(0.1f, 0.1f, 0.1f);
    if (m_cameraSystem) {
        clearColor = m_cameraSystem->GetClearColor();
    }

    // Clear the screen
    JzClearParams clearParams;
    clearParams.clearColor   = true;
    clearParams.clearDepth   = true;
    clearParams.clearStencil = false;
    clearParams.colorR       = clearColor.x;
    clearParams.colorG       = clearColor.y;
    clearParams.colorB       = clearColor.z;
    clearParams.colorA       = 1.0f;
    clearParams.depth        = 1.0f;
    clearParams.stencil      = 0;
    device.Clear(clearParams);
}

void JzEnttRenderSystem::RenderEntities(JzEnttWorld &world)
{
    auto &device = JzServiceContainer::Get<JzDevice>();

    // Get camera matrices
    JzMat4 viewMatrix       = JzMat4x4::Identity();
    JzMat4 projectionMatrix = JzMat4x4::Identity();
    JzVec3 cameraPos(0.0f, 0.0f, 10.0f);

    if (m_cameraSystem) {
        viewMatrix       = m_cameraSystem->GetViewMatrix();
        projectionMatrix = m_cameraSystem->GetProjectionMatrix();
        cameraPos        = m_cameraSystem->GetCameraPosition();
    }

    // Get light data
    JzVec3 lightDir(0.3f, 1.0f, 0.5f);
    JzVec3 lightColor(1.0f, 1.0f, 1.0f);

    if (m_lightSystem) {
        lightDir   = m_lightSystem->GetPrimaryLightDirection();
        lightColor = m_lightSystem->GetPrimaryLightColor();
    }

    // Set common uniforms
    JzMat4 modelMatrix = JzMat4x4::Identity();
    m_defaultPipeline->SetUniform("model", modelMatrix);
    m_defaultPipeline->SetUniform("view", viewMatrix);
    m_defaultPipeline->SetUniform("projection", projectionMatrix);
    m_defaultPipeline->SetUniform("uCameraPos", cameraPos);
    m_defaultPipeline->SetUniform("uLightDir", lightDir.Normalized());
    m_defaultPipeline->SetUniform("uLightColor", lightColor);

    // Default material colors
    JzVec3 defaultAmbient(0.2f, 0.2f, 0.2f);
    JzVec3 defaultDiffuse(0.8f, 0.8f, 0.8f);
    JzVec3 defaultSpecular(0.5f, 0.5f, 0.5f);
    F32    defaultShininess = 32.0f;

    // Render all entities with Transform + Mesh + Material
    auto view = world.View<JzTransformComponent, JzMeshComponent, JzMaterialComponent>();

    for (auto entity : view) {
        auto &transform = world.GetComponent<JzTransformComponent>(entity);
        auto &meshComp  = world.GetComponent<JzMeshComponent>(entity);
        auto &matComp   = world.GetComponent<JzMaterialComponent>(entity);

        // Get resources
        auto mesh     = std::static_pointer_cast<JzMesh>(meshComp.mesh);
        auto material = std::static_pointer_cast<JzMaterial>(matComp.material);

        // Validate resources
        if (!mesh || mesh->GetState() != JzEResourceState::Loaded) {
            continue;
        }

        auto vertexArray = mesh->GetVertexArray();
        if (!vertexArray) {
            continue;
        }

        // Set model matrix from transform
        // TODO: Compute full model matrix from position, rotation, scale
        JzMat4 entityModelMatrix = JzMat4x4::Identity();
        // For now just use translation
        entityModelMatrix(0, 3) = transform.position.x;
        entityModelMatrix(1, 3) = transform.position.y;
        entityModelMatrix(2, 3) = transform.position.z;
        m_defaultPipeline->SetUniform("model", entityModelMatrix);

        // Set material uniforms
        if (material && material->GetState() == JzEResourceState::Loaded) {
            const auto &props = material->GetProperties();
            m_defaultPipeline->SetUniform("uAmbientColor", props.ambientColor);
            m_defaultPipeline->SetUniform("uDiffuseColor", props.diffuseColor);
            m_defaultPipeline->SetUniform("uSpecularColor", props.specularColor);
            m_defaultPipeline->SetUniform("uShininess", props.shininess);
        } else {
            m_defaultPipeline->SetUniform("uAmbientColor", defaultAmbient);
            m_defaultPipeline->SetUniform("uDiffuseColor", defaultDiffuse);
            m_defaultPipeline->SetUniform("uSpecularColor", defaultSpecular);
            m_defaultPipeline->SetUniform("uShininess", defaultShininess);
        }

        // Bind vertex array and draw
        device.BindVertexArray(vertexArray);

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

void JzEnttRenderSystem::CleanupResources()
{
    m_defaultPipeline.reset();
    m_depthTexture.reset();
    m_colorTexture.reset();
    m_framebuffer.reset();
    m_isInitialized = false;
}

} // namespace JzRE
