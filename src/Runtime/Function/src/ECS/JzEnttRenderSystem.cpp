/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Function/ECS/JzEnttRenderSystem.h"

#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Runtime/Function/ECS/JzEnttComponents.h"
#include "JzRE/Runtime/Function/ECS/JzAssetComponents.h"
#include "JzRE/Runtime/Platform/JzDevice.h"
#include "JzRE/Runtime/Resource/JzShaderManager.h"
#include "JzRE/Runtime/Resource/JzAssetManager.h"
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
    SetupViewportAndClear(world);

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
    auto &shaderManager = JzServiceContainer::Get<JzShaderManager>();

    // Get default standard shader variant from ShaderManager
    auto variant = shaderManager.GetStandardShader();

    if (!variant || !variant->IsValid()) {
        return false;
    }

    m_defaultPipeline = variant->GetPipeline();
    return m_defaultPipeline != nullptr;
}

void JzEnttRenderSystem::SetupViewportAndClear(JzEnttWorld &world)
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

    auto cameraView = world.View<JzEnttCameraComponent>();
    for (auto entity : cameraView) {
        const auto &camera = world.GetComponent<JzEnttCameraComponent>(entity);
        if (camera.isMainCamera) {
            clearColor = camera.clearColor;
            break;
        }
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

    auto cameraView = world.View<JzEnttCameraComponent>();
    for (auto camEntity : cameraView) {
        const auto &camera = world.GetComponent<JzEnttCameraComponent>(camEntity);
        if (camera.isMainCamera) {
            viewMatrix       = camera.viewMatrix;
            projectionMatrix = camera.projectionMatrix;
            break;
        }
    }

    // Set common uniforms for example shader
    JzMat4 modelMatrix = JzMat4x4::Identity();
    m_defaultPipeline->SetUniform("model", modelMatrix);
    m_defaultPipeline->SetUniform("view", viewMatrix);
    m_defaultPipeline->SetUniform("projection", projectionMatrix);

    // Render entities with Asset Components (JzMeshAssetComponent + JzMaterialAssetComponent)
    // We only render entities that are marked as ready (JzAssetReadyTag)
    auto &assetManager = JzServiceContainer::Get<JzAssetManager>();
    auto  views        = world.View<JzTransformComponent, JzMeshAssetComponent, JzMaterialAssetComponent, JzAssetReadyTag>();

    for (auto entity : views) {
        auto &transform = world.GetComponent<JzTransformComponent>(entity);
        auto &meshComp  = world.GetComponent<JzMeshAssetComponent>(entity);
        auto &matComp   = world.GetComponent<JzMaterialAssetComponent>(entity);

        // Get Mesh Asset
        JzMesh *mesh = assetManager.Get(meshComp.meshHandle);
        if (!mesh) continue;

        auto vertexArray = mesh->GetVertexArray();
        if (!vertexArray) continue;

        // Get world matrix
        const JzMat4 &entityModelMatrix = transform.GetWorldMatrix();
        m_defaultPipeline->SetUniform("model", entityModelMatrix);

        // Set Material Uniforms
        // Note: We use the cached values in the component which are updated by JzAssetLoadingSystem
        m_defaultPipeline->SetUniform("material.ambient", matComp.ambientColor);
        m_defaultPipeline->SetUniform("material.diffuse", matComp.diffuseColor);
        m_defaultPipeline->SetUniform("material.specular", matComp.specularColor);
        m_defaultPipeline->SetUniform("material.shininess", matComp.shininess);

        // Bind and Draw
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
