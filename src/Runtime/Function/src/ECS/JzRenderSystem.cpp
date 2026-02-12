/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Function/ECS/JzRenderSystem.h"

#include <algorithm>
#include <array>
#include <cstddef>

#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Runtime/Function/ECS/JzAssetComponents.h"
#include "JzRE/Runtime/Function/ECS/JzCameraComponents.h"
#include "JzRE/Runtime/Function/ECS/JzLightComponents.h"
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
                RenderToTargetFiltered(world, target, entry.camera, entry.visibility, entry.features);
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

    // Main scene output renders gameplay entities only.
    RenderEntitiesFiltered(world, JzRenderVisibility::Untagged);
}

void JzRenderSystem::CleanupResources()
{
    m_gridVertexCount = 0;
    m_gridVAO.reset();
    m_gridVertexBuffer.reset();
    m_axisVAO.reset();
    m_axisVertexBuffer.reset();
    m_skyboxScreenTriangleVAO.reset();
    m_skyboxScreenTriangleBuffer.reset();
    m_axisPipeline.reset();
    m_skyboxPipeline.reset();
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
    String outputName;
    auto it = std::find_if(m_views.begin(), m_views.end(),
                           [handle](const auto &pair) { return pair.first == handle; });
    if (it != m_views.end()) {
        outputName = it->second.outputName;
        m_views.erase(it);
    }

    if (!outputName.empty()) {
        m_outputCache.Remove(outputName);
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

void JzRenderSystem::UpdateViewFeatures(JzRenderSystem::ViewHandle handle, JzRenderViewFeatures features)
{
    auto it = std::find_if(m_views.begin(), m_views.end(),
                           [handle](const auto &pair) { return pair.first == handle; });
    if (it != m_views.end()) {
        it->second.features = features;
    }
}

void JzRenderSystem::RenderToTargetFiltered(JzWorld &world, JzRenderTarget &target,
                                            JzEntity camera, JzRenderVisibility visibility,
                                            JzRenderViewFeatures features)
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
    RenderEntitiesFiltered(world, visibility);

    // Render helper passes enabled for this view.
    if (HasFeature(features, JzRenderViewFeatures::Skybox)) {
        RenderSkybox(world, viewMatrix, projectionMatrix);
    }
    if (HasFeature(features, JzRenderViewFeatures::Grid)) {
        RenderGrid(viewMatrix, projectionMatrix);
    }
    if (HasFeature(features, JzRenderViewFeatures::Axis)) {
        RenderAxis(viewMatrix, projectionMatrix);
    }

    // Unbind framebuffer
    device.BindFramebuffer(nullptr);
}

void JzRenderSystem::RenderEntitiesFiltered(JzWorld &world, JzRenderVisibility visibility)
{
    auto &device       = JzServiceContainer::Get<JzDevice>();
    auto &assetManager = JzServiceContainer::Get<JzAssetManager>();

    const Bool allowEditorOnly  = HasVisibility(visibility, JzRenderVisibility::EditorOnly);
    const Bool allowPreviewOnly = HasVisibility(visibility, JzRenderVisibility::PreviewOnly);
    const Bool allowUntagged    = HasVisibility(visibility, JzRenderVisibility::Untagged);

    // Get all renderable entities
    auto views = world.View<JzTransformComponent, JzMeshAssetComponent, JzMaterialAssetComponent, JzAssetReadyTag>();

    for (auto entity : views) {
        // Tag-based filtering:
        // - JzEditorOnlyTag: render if tag mask includes EditorOnly
        // - JzPreviewOnlyTag: render if tag mask includes PreviewOnly
        // - Untagged (game object): render if tag mask includes Untagged

        Bool hasEditorTag  = world.HasComponent<JzEditorOnlyTag>(entity);
        Bool hasPreviewTag = world.HasComponent<JzPreviewOnlyTag>(entity);

        // Skip editor-only entities if not including editor
        if (hasEditorTag && !allowEditorOnly) {
            continue;
        }

        // Skip preview-only entities if not including preview
        if (hasPreviewTag && !allowPreviewOnly) {
            continue;
        }

        // Skip untagged entities if not allowed
        if (!hasEditorTag && !hasPreviewTag && !allowUntagged) {
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

Bool JzRenderSystem::CreateEditorHelperResources()
{
    const Bool hasPipelines = (m_skyboxPipeline != nullptr && m_axisPipeline != nullptr);
    const Bool hasGeometry =
        (m_skyboxScreenTriangleVAO != nullptr && m_skyboxScreenTriangleBuffer != nullptr &&
         m_axisVAO != nullptr && m_axisVertexBuffer != nullptr &&
         m_gridVAO != nullptr && m_gridVertexBuffer != nullptr && m_gridVertexCount > 0);
    if (hasPipelines && hasGeometry) {
        return true;
    }

    if (!JzServiceContainer::Has<JzAssetManager>() || !JzServiceContainer::Has<JzDevice>()) {
        return false;
    }

    auto &assetManager = JzServiceContainer::Get<JzAssetManager>();
    auto &device       = JzServiceContainer::Get<JzDevice>();

    struct JzLineVertex {
        F32 px, py, pz;
        F32 r, g, b;
    };

    const auto loadPipeline = [&assetManager](const String &primaryPath, const String &fallbackPath) {
        auto tryLoad = [&assetManager](const String &path) -> std::shared_ptr<JzRHIPipeline> {
            const auto shaderHandle = assetManager.LoadSync<JzShaderAsset>(path);
            auto      *shaderAsset  = assetManager.Get(shaderHandle);
            if (!shaderAsset || !shaderAsset->IsCompiled()) {
                return nullptr;
            }

            auto variant = shaderAsset->GetMainVariant();
            if (!variant || !variant->IsValid()) {
                return nullptr;
            }
            return variant->GetPipeline();
        };

        auto pipeline = tryLoad(primaryPath);
        if (!pipeline && !fallbackPath.empty()) {
            pipeline = tryLoad(fallbackPath);
        }
        return pipeline;
    };

    if (!m_skyboxPipeline) {
        m_skyboxPipeline = loadPipeline("shaders/editor_skybox", "resources/shaders/editor_skybox");
    }

    if (!m_axisPipeline) {
        m_axisPipeline = loadPipeline("shaders/editor_axis", "resources/shaders/editor_axis");
    }

    if (!m_skyboxScreenTriangleBuffer || !m_skyboxScreenTriangleVAO) {
        constexpr std::array<F32, 6> kTriangleVertices = {
            -1.0f, -1.0f,
            -1.0f, 3.0f,
            3.0f, -1.0f
        };

        JzGPUBufferObjectDesc vbDesc;
        vbDesc.type      = JzEGPUBufferObjectType::Vertex;
        vbDesc.usage     = JzEGPUBufferObjectUsage::StaticDraw;
        vbDesc.size      = kTriangleVertices.size() * sizeof(F32);
        vbDesc.data      = kTriangleVertices.data();
        vbDesc.debugName = "EditorSkyboxScreenTriangleVB";
        m_skyboxScreenTriangleBuffer = device.CreateBuffer(vbDesc);

        m_skyboxScreenTriangleVAO = device.CreateVertexArray("EditorSkyboxScreenTriangleVAO");
        if (m_skyboxScreenTriangleBuffer && m_skyboxScreenTriangleVAO) {
            m_skyboxScreenTriangleVAO->BindVertexBuffer(m_skyboxScreenTriangleBuffer, 0);
            m_skyboxScreenTriangleVAO->SetVertexAttribute(0, 2, static_cast<U32>(2 * sizeof(F32)), 0);
        }
    }

    if (!m_axisVertexBuffer || !m_axisVAO) {
        constexpr std::array<JzLineVertex, 6> kAxisVertices = {{
            {0.0f, 0.0f, 0.0f, 1.0f, 0.2f, 0.2f},
            {1.5f, 0.0f, 0.0f, 1.0f, 0.2f, 0.2f},
            {0.0f, 0.0f, 0.0f, 0.2f, 1.0f, 0.2f},
            {0.0f, 1.5f, 0.0f, 0.2f, 1.0f, 0.2f},
            {0.0f, 0.0f, 0.0f, 0.2f, 0.5f, 1.0f},
            {0.0f, 0.0f, 1.5f, 0.2f, 0.5f, 1.0f}
        }};

        JzGPUBufferObjectDesc vbDesc;
        vbDesc.type      = JzEGPUBufferObjectType::Vertex;
        vbDesc.usage     = JzEGPUBufferObjectUsage::StaticDraw;
        vbDesc.size      = kAxisVertices.size() * sizeof(JzLineVertex);
        vbDesc.data      = kAxisVertices.data();
        vbDesc.debugName = "EditorAxisVB";
        m_axisVertexBuffer = device.CreateBuffer(vbDesc);

        m_axisVAO = device.CreateVertexArray("EditorAxisVAO");
        if (m_axisVertexBuffer && m_axisVAO) {
            m_axisVAO->BindVertexBuffer(m_axisVertexBuffer, 0);
            m_axisVAO->SetVertexAttribute(0, 3, static_cast<U32>(sizeof(JzLineVertex)), 0);
            m_axisVAO->SetVertexAttribute(1, 3, static_cast<U32>(sizeof(JzLineVertex)),
                                          static_cast<U32>(offsetof(JzLineVertex, r)));
        }
    }

    if (!m_gridVertexBuffer || !m_gridVAO || m_gridVertexCount == 0) {
        constexpr I32 kHalfLineCount = 20;
        constexpr F32 kGridSpacing   = 1.0f;
        constexpr F32 kGridY         = -0.001f;
        constexpr F32 kGridExtent    = kHalfLineCount * kGridSpacing;

        std::vector<JzLineVertex> gridVertices;
        gridVertices.reserve(static_cast<size_t>((kHalfLineCount * 2 + 1) * 4));

        for (I32 i = -kHalfLineCount; i <= kHalfLineCount; ++i) {
            const F32 offset = static_cast<F32>(i) * kGridSpacing;
            const Bool majorLine = (i == 0) || (i % 5 == 0);
            const F32  c         = majorLine ? 0.36f : 0.24f;

            gridVertices.push_back({-kGridExtent, kGridY, offset, c, c, c});
            gridVertices.push_back({kGridExtent, kGridY, offset, c, c, c});
            gridVertices.push_back({offset, kGridY, -kGridExtent, c, c, c});
            gridVertices.push_back({offset, kGridY, kGridExtent, c, c, c});
        }

        JzGPUBufferObjectDesc vbDesc;
        vbDesc.type      = JzEGPUBufferObjectType::Vertex;
        vbDesc.usage     = JzEGPUBufferObjectUsage::StaticDraw;
        vbDesc.size      = gridVertices.size() * sizeof(JzLineVertex);
        vbDesc.data      = gridVertices.data();
        vbDesc.debugName = "EditorGridVB";
        m_gridVertexBuffer = device.CreateBuffer(vbDesc);

        m_gridVAO = device.CreateVertexArray("EditorGridVAO");
        if (m_gridVertexBuffer && m_gridVAO) {
            m_gridVAO->BindVertexBuffer(m_gridVertexBuffer, 0);
            m_gridVAO->SetVertexAttribute(0, 3, static_cast<U32>(sizeof(JzLineVertex)), 0);
            m_gridVAO->SetVertexAttribute(1, 3, static_cast<U32>(sizeof(JzLineVertex)),
                                          static_cast<U32>(offsetof(JzLineVertex, r)));
            m_gridVertexCount = static_cast<U32>(gridVertices.size());
        }
    }

    return true;
}

void JzRenderSystem::RenderSkybox(JzWorld &world, const JzMat4 &viewMatrix, const JzMat4 &projectionMatrix)
{
    if (!CreateEditorHelperResources() || !m_skyboxPipeline || !m_skyboxScreenTriangleVAO) {
        return;
    }

    auto &device = JzServiceContainer::Get<JzDevice>();

    JzVec3 sunDirection(0.3f, -1.0f, -0.5f);
    auto   lightView = world.View<JzDirectionalLightComponent>();
    if (!lightView.empty()) {
        sunDirection = world.GetComponent<JzDirectionalLightComponent>(lightView.front()).direction;
    }
    if (sunDirection.Length() > 0.0001f) {
        sunDirection.Normalize();
    }

    device.BindPipeline(m_skyboxPipeline);
    device.BindVertexArray(m_skyboxScreenTriangleVAO);

    m_skyboxPipeline->SetUniform("view", viewMatrix);
    m_skyboxPipeline->SetUniform("projection", projectionMatrix);
    m_skyboxPipeline->SetUniform("topColor", JzVec3(0.19f, 0.42f, 0.78f));
    m_skyboxPipeline->SetUniform("horizonColor", JzVec3(0.62f, 0.73f, 0.90f));
    m_skyboxPipeline->SetUniform("groundColor", JzVec3(0.20f, 0.21f, 0.24f));
    m_skyboxPipeline->SetUniform("sunDirection", sunDirection);
    m_skyboxPipeline->SetUniform("sunColor", JzVec3(1.0f, 0.95f, 0.80f));
    m_skyboxPipeline->SetUniform("sunSize", 0.04f);
    m_skyboxPipeline->SetUniform("exposure", 1.0f);

    JzDrawParams drawParams;
    drawParams.primitiveType = JzEPrimitiveType::Triangles;
    drawParams.vertexCount   = 3;
    drawParams.instanceCount = 1;
    drawParams.firstVertex   = 0;
    drawParams.firstInstance = 0;
    device.Draw(drawParams);
}

void JzRenderSystem::RenderAxis(const JzMat4 &viewMatrix, const JzMat4 &projectionMatrix)
{
    if (!CreateEditorHelperResources() || !m_axisPipeline || !m_axisVAO) {
        return;
    }

    auto &device = JzServiceContainer::Get<JzDevice>();
    device.BindPipeline(m_axisPipeline);
    device.BindVertexArray(m_axisVAO);

    const JzMat4 model = JzMat4x4::Identity();
    m_axisPipeline->SetUniform("model", model);
    m_axisPipeline->SetUniform("view", viewMatrix);
    m_axisPipeline->SetUniform("projection", projectionMatrix);

    JzDrawParams drawParams;
    drawParams.primitiveType = JzEPrimitiveType::Lines;
    drawParams.vertexCount   = 6;
    drawParams.instanceCount = 1;
    drawParams.firstVertex   = 0;
    drawParams.firstInstance = 0;
    device.Draw(drawParams);
}

void JzRenderSystem::RenderGrid(const JzMat4 &viewMatrix, const JzMat4 &projectionMatrix)
{
    if (!CreateEditorHelperResources() || !m_axisPipeline || !m_gridVAO || m_gridVertexCount == 0) {
        return;
    }

    auto &device = JzServiceContainer::Get<JzDevice>();
    device.BindPipeline(m_axisPipeline);
    device.BindVertexArray(m_gridVAO);

    const JzMat4 model = JzMat4x4::Identity();
    m_axisPipeline->SetUniform("model", model);
    m_axisPipeline->SetUniform("view", viewMatrix);
    m_axisPipeline->SetUniform("projection", projectionMatrix);

    JzDrawParams drawParams;
    drawParams.primitiveType = JzEPrimitiveType::Lines;
    drawParams.vertexCount   = m_gridVertexCount;
    drawParams.instanceCount = 1;
    drawParams.firstVertex   = 0;
    drawParams.firstInstance = 0;
    device.Draw(drawParams);
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
