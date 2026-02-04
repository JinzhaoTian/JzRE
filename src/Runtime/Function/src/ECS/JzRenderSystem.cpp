/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Function/ECS/JzRenderSystem.h"

#include <algorithm>

#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Runtime/Function/ECS/JzAssetComponents.h"
#include "JzRE/Runtime/Function/ECS/JzCameraComponents.h"
#include "JzRE/Runtime/Function/ECS/JzRenderComponents.h"
#include "JzRE/Runtime/Function/ECS/JzTransformComponents.h"
#include "JzRE/Runtime/Function/ECS/JzWindowComponents.h"
#include "JzRE/Runtime/Function/Rendering/JzRenderTarget.h"
#include "JzRE/Runtime/Platform/RHI/JzGraphicsContext.h"
#include "JzRE/Runtime/Platform/RHI/JzDevice.h"
#include "JzRE/Runtime/Resource/JzShaderAsset.h"
#include "JzRE/Runtime/Resource/JzAssetManager.h"
#include "JzRE/Runtime/Resource/JzMesh.h"
#include "JzRE/Runtime/Resource/JzMaterial.h"

namespace JzRE {

JzRenderSystem::JzRenderSystem() { }

void JzRenderSystem::OnInit(JzWorld &world)
{
    // Resources will be created lazily in Update
}

void JzRenderSystem::Update(JzWorld &world, F32 delta)
{
    // Update frame logic and blitting
    auto windowView = world.View<JzWindowStateComponent>();
    Bool shouldBlit = false;

    if (!windowView.empty()) {
        const auto &windowState = world.GetComponent<JzWindowStateComponent>(windowView.front());
        if (m_frameSize != windowState.framebufferSize) {
            m_frameSize        = windowState.framebufferSize;
            m_frameSizeChanged = true;
        }
        shouldBlit = windowState.visible;
    }

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
    m_renderGraph.SetTransitionCallback(
        [this](const JzRGPassDesc &passDesc, const std::vector<JzRGTransition> &transitions) {
            ApplyRenderGraphTransitions(passDesc, transitions);
        });

    auto &device = JzServiceContainer::Get<JzDevice>();
    m_renderGraph.SetTextureAllocator([&device](const JzRGTextureDesc &desc) {
        JzGPUTextureObjectDesc texDesc;
        texDesc.type      = JzETextureResourceType::Texture2D;
        texDesc.format    = desc.format;
        texDesc.width     = static_cast<U32>(desc.size.x);
        texDesc.height    = static_cast<U32>(desc.size.y);
        texDesc.debugName = desc.name;
        return device.CreateTexture(texDesc);
    });
    m_renderGraph.SetBufferAllocator([&device](const JzRGBufferDesc &desc) {
        JzGPUBufferObjectDesc bufDesc;
        bufDesc.type      = desc.type;
        bufDesc.usage     = desc.usage;
        bufDesc.size      = desc.size;
        bufDesc.data      = nullptr;
        bufDesc.debugName = desc.name;
        return device.CreateBuffer(bufDesc);
    });

    m_renderGraph.Reset();

    // Main scene logical outputs
    JzRGTexture mainColor = m_renderGraph.CreateTexture(
        {m_frameSize, JzETextureResourceFormat::RGBA8, false, "MainScene_Color"});
    JzRGTexture mainDepth = m_renderGraph.CreateTexture(
        {m_frameSize, JzETextureResourceFormat::Depth24, false, "MainScene_Depth"});
    m_renderGraph.BindTexture(mainColor, m_colorTexture);
    m_renderGraph.BindTexture(mainDepth, m_depthTexture);

    m_renderGraph.AddPass({
        "MainScenePass",
        nullptr,
        [this, mainColor, mainDepth](JzRGBuilder &builder) {
            builder.Write(mainColor, JzRGUsage::Write);
            builder.Write(mainDepth, JzRGUsage::Write);
            builder.SetRenderTarget(mainColor, mainDepth);
            builder.SetViewport(m_frameSize);
        },
        [this, &world]() {
            SetupViewportAndClear(world);
            RenderEntities(world);

            auto &deviceRef = JzServiceContainer::Get<JzDevice>();
            deviceRef.BindFramebuffer(nullptr);
        },
    });

    // Build view target passes (Phase 2: still driven by registry)
    for (auto &[handle, entry] : m_views) {
        if (!entry.outputName.empty()) {
            auto targetIter = m_targets.find(handle);
            if (targetIter != m_targets.end()) {
                m_renderGraph.RegisterOutput(entry.outputName, targetIter->second.get());
                m_outputCache.Update(entry.outputName, targetIter->second);
            }
        }

        auto targetIter = m_targets.find(handle);
        if (targetIter == m_targets.end()) {
            continue;
        }

        auto   &target      = *targetIter->second;
        JzIVec2 desiredSize = target.GetSize();
        if (entry.getDesiredSize) {
            desiredSize = entry.getDesiredSize();
            target.EnsureSize(desiredSize);
        }

        if (!target.IsValid()) {
            continue;
        }

        const String colorName = entry.outputName.empty() ? entry.name + "_Color" : entry.outputName;
        const String depthName = colorName + "_Depth";
        JzRGTexture  viewColor = m_renderGraph.CreateTexture(
            {desiredSize, JzETextureResourceFormat::RGBA8, false, colorName});
        JzRGTexture viewDepth = m_renderGraph.CreateTexture(
            {desiredSize, JzETextureResourceFormat::Depth24, false, depthName});

        m_renderGraph.BindTexture(viewColor, target.GetColorTexture());
        m_renderGraph.BindTexture(viewDepth, target.GetDepthTexture());

        m_renderGraph.AddPass({
            entry.passName.empty() ? entry.name : entry.passName,
            entry.shouldRender ? entry.shouldRender : nullptr,
            [desiredSize, viewColor, viewDepth](JzRGBuilder &builder) {
                builder.Write(viewColor, JzRGUsage::Write);
                builder.Write(viewDepth, JzRGUsage::Write);
                builder.SetRenderTarget(viewColor, viewDepth);
                builder.SetViewport(desiredSize);
            },
            [this, &world, &entry, handle]() {
                auto targetIter = m_targets.find(handle);
                if (targetIter == m_targets.end()) {
                    return;
                }

                auto &target = *targetIter->second;
                RenderToTargetFiltered(world, target, entry.camera, entry.includeEditor,
                                       entry.includePreview);
            },
        });
    }

    m_renderGraph.Compile();
    static Bool s_dumpedGraph = false;
    if (!s_dumpedGraph) {
        m_renderGraph.DumpGraph("docs/architecture/render_graph_dump.md");
        s_dumpedGraph = true;
    }
    m_renderGraph.Execute();

    // Blit to screen if requested
    if (shouldBlit) {
        BlitToScreen(static_cast<U32>(m_frameSize.x), static_cast<U32>(m_frameSize.y));
    }
}

void JzRenderSystem::OnShutdown(JzWorld &world)
{
    CleanupResources();
}

JzIVec2 JzRenderSystem::GetCurrentFrameSize() const
{
    return m_frameSize;
}

std::shared_ptr<JzGPUFramebufferObject> JzRenderSystem::GetFramebuffer() const
{
    return m_framebuffer;
}

std::shared_ptr<JzGPUTextureObject> JzRenderSystem::GetColorTexture() const
{
    return m_colorTexture;
}

std::shared_ptr<JzGPUTextureObject> JzRenderSystem::GetDepthTexture() const
{
    return m_depthTexture;
}

std::shared_ptr<JzRHIPipeline> JzRenderSystem::GetDefaultPipeline() const
{
    return m_defaultPipeline;
}

JzRenderOutput *JzRenderSystem::GetRenderOutput(ViewHandle handle) const
{
    auto iter = m_targets.find(handle);
    if (iter == m_targets.end()) {
        return nullptr;
    }

    return iter->second.get();
}

JzRenderOutput *JzRenderSystem::GetRenderOutput(const String &name) const
{
    if (auto *cached = m_outputCache.Get(name)) {
        return cached;
    }
    return m_renderGraph.GetOutput(name);
}

void JzRenderSystem::BeginFrame()
{
    if (!JzServiceContainer::Has<JzGraphicsContext>()) {
        return;
    }

    auto &context = JzServiceContainer::Get<JzGraphicsContext>();
    if (!context.IsInitialized()) {
        return;
    }

    context.BeginFrame();
}

void JzRenderSystem::EndFrame()
{
    if (!JzServiceContainer::Has<JzGraphicsContext>()) {
        return;
    }

    auto &context = JzServiceContainer::Get<JzGraphicsContext>();
    if (!context.IsInitialized()) {
        return;
    }

    context.EndFrame();
}

void JzRenderSystem::BlitToScreen(U32 screenWidth, U32 screenHeight)
{
    if (!m_framebuffer) {
        return;
    }

    auto &device = JzServiceContainer::Get<JzDevice>();
    device.BlitFramebufferToScreen(m_framebuffer, static_cast<U32>(m_frameSize.x),
                                   static_cast<U32>(m_frameSize.y), screenWidth, screenHeight);
}

Bool JzRenderSystem::IsInitialized() const
{
    return m_isInitialized;
}

Bool JzRenderSystem::CreateFramebuffer()
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

Bool JzRenderSystem::CreateDefaultPipeline()
{
    auto &assetManager = JzServiceContainer::Get<JzAssetManager>();

    // Load standard shader using JzShaderAsset
    // JzShaderAsset handles both vertex and fragment shaders, compilation, and variant management
    auto handle = assetManager.LoadSync<JzShaderAsset>("shaders/standard");

    JzShaderAsset *shaderAsset = assetManager.Get(handle);
    if (!shaderAsset) {
        return false;
    }

    // Check if shader compiled successfully
    if (!shaderAsset->IsCompiled()) {
        // Log compile error
        return false;
    }

    // Get the main variant's pipeline directly
    auto mainVariant = shaderAsset->GetMainVariant();
    if (!mainVariant || !mainVariant->IsValid()) {
        return false;
    }

    m_defaultPipeline = mainVariant->GetPipeline();
    return m_defaultPipeline != nullptr;
}

void JzRenderSystem::SetupViewportAndClear(JzWorld &world)
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

    auto cameraView = world.View<JzCameraComponent>();
    for (auto entity : cameraView) {
        const auto &camera = world.GetComponent<JzCameraComponent>(entity);
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

void JzRenderSystem::RenderEntities(JzWorld &world)
{
    auto &device = JzServiceContainer::Get<JzDevice>();

    // Get camera matrices
    JzMat4 viewMatrix       = JzMat4x4::Identity();
    JzMat4 projectionMatrix = JzMat4x4::Identity();

    auto cameraView = world.View<JzCameraComponent>();
    for (auto camEntity : cameraView) {
        const auto &camera = world.GetComponent<JzCameraComponent>(camEntity);
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

        // Bind diffuse texture if available
        // Get texture directly from material (hasDiffuseTexture flag is set during model loading)
        JzMaterial *mat               = assetManager.Get(matComp.materialHandle);
        bool        hasDiffuseTexture = mat && mat->HasDiffuseTexture();
        m_defaultPipeline->SetUniform("hasDiffuseTexture", hasDiffuseTexture);

        if (hasDiffuseTexture) {
            device.BindTexture(mat->GetDiffuseTexture(), 0);
            m_defaultPipeline->SetUniform("diffuseTexture", 0);
        }

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

void JzRenderSystem::CleanupResources()
{
    m_defaultPipeline.reset();
    m_depthTexture.reset();
    m_colorTexture.reset();
    m_framebuffer.reset();
    m_targets.clear();
    m_outputCache.Clear();
    m_isInitialized = false;
}

// ==================== View Registration ====================

JzRenderSystem::ViewHandle JzRenderSystem::RegisterView(JzRenderViewDesc desc)
{
    const auto handle = m_nextViewHandle++;
    const auto name   = desc.name.empty() ? "RenderTarget" : desc.name;

    if (desc.passName.empty()) {
        desc.passName = name + "Pass";
    }
    if (desc.outputName.empty()) {
        desc.outputName = name + "_Color";
    }

    m_views.emplace_back(handle, std::move(desc));
    m_targets.emplace(handle, std::make_shared<JzRenderTarget>(name + "_RT"));
    return handle;
}

void JzRenderSystem::UnregisterView(JzRenderSystem::ViewHandle handle)
{
    auto it = std::find_if(m_views.begin(), m_views.end(),
                           [handle](const auto &pair) { return pair.first == handle; });
    if (it != m_views.end()) {
        m_views.erase(it);
    }

    for (auto &entry : m_views) {
        if (entry.first == handle) {
            if (!entry.second.outputName.empty()) {
                m_outputCache.Remove(entry.second.outputName);
            }
            break;
        }
    }
    m_targets.erase(handle);
}

void JzRenderSystem::UpdateViewCamera(JzRenderSystem::ViewHandle handle, JzEntity camera)
{
    auto it = std::find_if(m_views.begin(), m_views.end(),
                           [handle](const auto &pair) { return pair.first == handle; });
    if (it != m_views.end()) {
        it->second.camera = camera;
    }
}

void JzRenderSystem::RenderToTargetFiltered(JzWorld &world, JzRenderTarget &target,
                                            JzEntity camera, Bool includeEditor, Bool includePreview)
{
    // Ensure pipeline is initialized
    if (!m_isInitialized) {
        CreateDefaultPipeline();
        m_isInitialized = true;
    }

    if (!m_defaultPipeline) {
        return;
    }

    auto &device     = JzServiceContainer::Get<JzDevice>();
    auto  targetSize = target.GetSize();

    // Bind render target's framebuffer
    device.BindFramebuffer(target.GetFramebuffer());

    // Bind pipeline
    device.BindPipeline(m_defaultPipeline);

    // Set viewport to render target size
    JzViewport viewport;
    viewport.x        = 0.0f;
    viewport.y        = 0.0f;
    viewport.width    = static_cast<F32>(targetSize.x);
    viewport.height   = static_cast<F32>(targetSize.y);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    device.SetViewport(viewport);

    // Get camera matrices - use specified camera or find main camera
    JzMat4 viewMatrix       = JzMat4x4::Identity();
    JzMat4 projectionMatrix = JzMat4x4::Identity();
    JzVec3 clearColor(0.1f, 0.1f, 0.1f);

    auto cameraView = world.View<JzCameraComponent>();

    if (IsValidEntity(camera) && world.HasComponent<JzCameraComponent>(camera)) {
        // Use specified camera
        const auto &cam  = world.GetComponent<JzCameraComponent>(camera);
        viewMatrix       = cam.viewMatrix;
        projectionMatrix = cam.projectionMatrix;
        clearColor       = cam.clearColor;
    } else {
        // Fall back to main camera
        for (auto entity : cameraView) {
            const auto &cam = world.GetComponent<JzCameraComponent>(entity);
            if (cam.isMainCamera) {
                viewMatrix       = cam.viewMatrix;
                projectionMatrix = cam.projectionMatrix;
                clearColor       = cam.clearColor;
                break;
            }
        }
    }

    // Clear the framebuffer
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

    // Set common uniforms
    JzMat4 modelMatrix = JzMat4x4::Identity();
    m_defaultPipeline->SetUniform("model", modelMatrix);
    m_defaultPipeline->SetUniform("view", viewMatrix);
    m_defaultPipeline->SetUniform("projection", projectionMatrix);

    // Render entities with filtering
    RenderEntitiesFiltered(world, includeEditor, includePreview);

    // Unbind framebuffer
    device.BindFramebuffer(nullptr);
}

void JzRenderSystem::RenderEntitiesFiltered(JzWorld &world, Bool includeEditor, Bool includePreview)
{
    auto &device       = JzServiceContainer::Get<JzDevice>();
    auto &assetManager = JzServiceContainer::Get<JzAssetManager>();

    // Get all renderable entities
    auto views = world.View<JzTransformComponent, JzMeshAssetComponent, JzMaterialAssetComponent, JzAssetReadyTag>();

    for (auto entity : views) {
        // Tag-based filtering:
        // - JzEditorOnlyTag: only render if includeEditor is true
        // - JzPreviewOnlyTag: only render if includePreview is true
        // - No tag (game object): always render unless it's preview-only view

        Bool hasEditorTag  = world.HasComponent<JzEditorOnlyTag>(entity);
        Bool hasPreviewTag = world.HasComponent<JzPreviewOnlyTag>(entity);

        // Skip editor-only entities if not including editor
        if (hasEditorTag && !includeEditor) {
            continue;
        }

        // Skip preview-only entities if not including preview
        if (hasPreviewTag && !includePreview) {
            continue;
        }

        // For preview-only views, skip game objects (entities without preview tag)
        // Preview view should only show preview entities
        if (includePreview && !includeEditor && !hasPreviewTag) {
            continue;
        }

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
        m_defaultPipeline->SetUniform("material.ambient", matComp.ambientColor);
        m_defaultPipeline->SetUniform("material.diffuse", matComp.diffuseColor);
        m_defaultPipeline->SetUniform("material.specular", matComp.specularColor);
        m_defaultPipeline->SetUniform("material.shininess", matComp.shininess);

        // Bind diffuse texture if available
        JzMaterial *mat               = assetManager.Get(matComp.materialHandle);
        bool        hasDiffuseTexture = mat && mat->HasDiffuseTexture();
        m_defaultPipeline->SetUniform("hasDiffuseTexture", hasDiffuseTexture);

        if (hasDiffuseTexture) {
            device.BindTexture(mat->GetDiffuseTexture(), 0);
            m_defaultPipeline->SetUniform("diffuseTexture", 0);
        }

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

void JzRenderSystem::ApplyRenderGraphTransitions(
    const JzRGPassDesc &passDesc, const std::vector<JzRGTransition> &transitions)
{
    (void)passDesc;

    if (transitions.empty()) {
        return;
    }

    auto                             &device = JzServiceContainer::Get<JzDevice>();
    std::vector<JzRHIResourceBarrier> barriers;
    barriers.reserve(transitions.size());

    auto toState = [](JzRGUsage usage) {
        switch (usage) {
            case JzRGUsage::Read:
                return JzERHIResourceState::Read;
            case JzRGUsage::Write:
                return JzERHIResourceState::Write;
            case JzRGUsage::ReadWrite:
                return JzERHIResourceState::ReadWrite;
        }
        return JzERHIResourceState::Unknown;
    };

    for (const auto &transition : transitions) {
        if (transition.id == 0) {
            continue;
        }

        if (transition.type == JzRGResourceType::Texture) {
            auto resource = m_renderGraph.GetTextureResource(JzRGTexture{transition.id});
            if (!resource) {
                continue;
            }

            JzRHIResourceBarrier barrier;
            barrier.type     = JzEResourceType::Texture;
            barrier.resource = std::static_pointer_cast<JzGPUResource>(resource);
            barrier.before   = toState(transition.before);
            barrier.after    = toState(transition.after);
            barriers.push_back(std::move(barrier));
        } else {
            auto resource = m_renderGraph.GetBufferResource(JzRGBuffer{transition.id});
            if (!resource) {
                continue;
            }

            JzRHIResourceBarrier barrier;
            barrier.type     = JzEResourceType::Buffer;
            barrier.resource = std::static_pointer_cast<JzGPUResource>(resource);
            barrier.before   = toState(transition.before);
            barrier.after    = toState(transition.after);
            barriers.push_back(std::move(barrier));
        }
    }

    if (!barriers.empty()) {
        device.ResourceBarrier(barriers);
    }
}

} // namespace JzRE
