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
#include "JzRE/Runtime/Platform/RHI/JzDevice.h"
#include "JzRE/Runtime/Platform/RHI/JzGraphicsContext.h"
#include "JzRE/Runtime/Resource/JzAssetManager.h"
#include "JzRE/Runtime/Resource/JzMaterial.h"
#include "JzRE/Runtime/Resource/JzMesh.h"
#include "JzRE/Runtime/Resource/JzShaderAsset.h"

namespace JzRE {

JzRenderSystem::JzRenderSystem() { }

void JzRenderSystem::OnInit(JzWorld &world)
{
    (void)world;
    // Resources are created lazily in Update().
}

void JzRenderSystem::Update(JzWorld &world, F32 delta)
{
    (void)delta;

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

    if (m_frameSizeChanged) {
        CreateFramebuffer();
        m_frameSizeChanged = false;
    }

    if (!m_isInitialized) {
        CreateDefaultPipeline();
        m_isInitialized = true;
    }

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

    for (auto &record : m_renderTargets) {
        auto &desc = record.desc;
        if (!record.output) {
            continue;
        }

        auto   &output      = *record.output;
        JzIVec2 desiredSize = output.GetSize();
        if (desc.getDesiredSize) {
            desiredSize = desc.getDesiredSize();
            output.EnsureSize(desiredSize);
        }

        if (!output.IsValid()) {
            continue;
        }

        const String colorName   = desc.name + "_Color";
        const String depthName   = colorName + "_Depth";
        JzRGTexture  targetColor = m_renderGraph.CreateTexture(
            {desiredSize, JzETextureResourceFormat::RGBA8, false, colorName});
        JzRGTexture targetDepth = m_renderGraph.CreateTexture(
            {desiredSize, JzETextureResourceFormat::Depth24, false, depthName});

        m_renderGraph.BindTexture(targetColor, output.GetColorTexture());
        m_renderGraph.BindTexture(targetDepth, output.GetDepthTexture());

        m_renderGraph.AddPass({
            desc.name + "Pass",
            desc.shouldRender ? desc.shouldRender : nullptr,
            [desiredSize, targetColor, targetDepth](JzRGBuilder &builder) {
                builder.Write(targetColor, JzRGUsage::Write);
                builder.Write(targetDepth, JzRGUsage::Write);
                builder.SetRenderTarget(targetColor, targetDepth);
                builder.SetViewport(desiredSize);
            },
            [this, &world, output = record.output, camera = desc.camera,
             visibility = desc.visibility, features = desc.features]() {
                if (!output) {
                    return;
                }
                RenderToTargetFiltered(world, *output, camera, visibility, features);
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

    if (shouldBlit) {
        BlitToScreen(static_cast<U32>(m_frameSize.x), static_cast<U32>(m_frameSize.y));
    }
}

void JzRenderSystem::OnShutdown(JzWorld &world)
{
    (void)world;
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

JzRenderOutput *JzRenderSystem::GetRenderOutput(JzRenderTargetHandle handle) const
{
    auto iter = std::find_if(m_renderTargets.begin(), m_renderTargets.end(),
                             [handle](const JzRenderTarget &record) {
                                 return record.handle == handle;
                             });
    if (iter == m_renderTargets.end() || !iter->output) {
        return nullptr;
    }

    return iter->output.get();
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

void JzRenderSystem::RegisterRenderPass(JzRenderPass pass)
{
    if (pass.name.empty()) {
        return;
    }

    if (!pass.pipeline || !pass.vertexArray) {
        return;
    }

    if (pass.drawParams.vertexCount == 0) {
        return;
    }

    auto iter = std::find_if(
        m_renderPasses.begin(), m_renderPasses.end(),
        [&pass](const JzRenderPass &entry) { return entry.name == pass.name; });
    if (iter != m_renderPasses.end()) {
        *iter = std::move(pass);
    } else {
        m_renderPasses.push_back(std::move(pass));
    }
}

void JzRenderSystem::ClearRenderPasses()
{
    m_renderPasses.clear();
}

Bool JzRenderSystem::CreateFramebuffer()
{
    auto &device = JzServiceContainer::Get<JzDevice>();

    m_framebuffer = device.CreateFramebuffer("ECSRendererFB");
    if (!m_framebuffer) {
        return false;
    }

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
    auto  handle       = assetManager.LoadSync<JzShaderAsset>("shaders/standard");
    auto *shaderAsset  = assetManager.Get(handle);
    if (!shaderAsset || !shaderAsset->IsCompiled()) {
        return false;
    }

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
    device.BindFramebuffer(m_framebuffer);
    device.BindPipeline(m_defaultPipeline);

    JzViewport viewport;
    viewport.x        = 0.0f;
    viewport.y        = 0.0f;
    viewport.width    = static_cast<F32>(m_frameSize.x);
    viewport.height   = static_cast<F32>(m_frameSize.y);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    device.SetViewport(viewport);

    JzVec3 clearColor(0.1f, 0.1f, 0.1f);
    auto   cameraView = world.View<JzCameraComponent>();
    for (auto entity : cameraView) {
        const auto &camera = world.GetComponent<JzCameraComponent>(entity);
        if (camera.isMainCamera) {
            clearColor = camera.clearColor;
            break;
        }
    }

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

    JzMat4 modelMatrix = JzMat4x4::Identity();
    m_defaultPipeline->SetUniform("model", modelMatrix);
    m_defaultPipeline->SetUniform("view", viewMatrix);
    m_defaultPipeline->SetUniform("projection", projectionMatrix);

    RenderEntitiesFiltered(world, JzRenderVisibility::MainScene);
}

void JzRenderSystem::CleanupResources()
{
    m_renderPasses.clear();
    m_defaultPipeline.reset();
    m_depthTexture.reset();
    m_colorTexture.reset();
    m_framebuffer.reset();
    m_renderTargets.clear();
    m_nextRenderTargetHandle = 1;
    m_isInitialized          = false;
}

JzRenderTargetHandle JzRenderSystem::RegisterRenderTarget(JzRenderTargetDesc desc)
{
    const auto handle           = m_nextRenderTargetHandle++;
    auto       renderTargetName = desc.name.empty() ? "RenderTarget" : desc.name;
    const auto isNameUsed       = [this](const String &candidate) {
        return std::any_of(m_renderTargets.begin(), m_renderTargets.end(),
                                 [&candidate](const JzRenderTarget &record) {
                               return record.desc.name == candidate;
                           });
    };
    if (isNameUsed(renderTargetName)) {
        renderTargetName += "_" + std::to_string(handle);
    }
    desc.name = renderTargetName;

    JzRenderTarget record;
    record.handle     = handle;
    record.desc       = std::move(desc);
    record.output     = std::make_shared<JzRenderOutput>(record.desc.name + "_Output");
    m_renderTargets.push_back(std::move(record));
    return handle;
}

void JzRenderSystem::UnregisterRenderTarget(JzRenderTargetHandle handle)
{
    auto it = std::find_if(m_renderTargets.begin(), m_renderTargets.end(),
                           [handle](const JzRenderTarget &record) {
                               return record.handle == handle;
                           });
    if (it != m_renderTargets.end()) {
        m_renderTargets.erase(it);
    }
}

void JzRenderSystem::UpdateRenderTargetCamera(JzRenderTargetHandle handle, JzEntity camera)
{
    auto it = std::find_if(m_renderTargets.begin(), m_renderTargets.end(),
                           [handle](const JzRenderTarget &record) {
                               return record.handle == handle;
                           });
    if (it != m_renderTargets.end()) {
        it->desc.camera = camera;
    }
}

void JzRenderSystem::UpdateRenderTargetFeatures(JzRenderTargetHandle  handle,
                                                JzRenderTargetFeatures features)
{
    auto it = std::find_if(m_renderTargets.begin(), m_renderTargets.end(),
                           [handle](const JzRenderTarget &record) {
                               return record.handle == handle;
                           });
    if (it != m_renderTargets.end()) {
        it->desc.features = features;
    }
}

void JzRenderSystem::UpdateRenderTargetVisibility(JzRenderTargetHandle handle,
                                                  JzRenderVisibility visibility)
{
    auto it = std::find_if(m_renderTargets.begin(), m_renderTargets.end(),
                           [handle](const JzRenderTarget &record) {
                               return record.handle == handle;
                           });
    if (it != m_renderTargets.end()) {
        it->desc.visibility = visibility;
    }
}

void JzRenderSystem::RenderToTargetFiltered(JzWorld &world, JzRenderOutput &output,
                                            JzEntity camera, JzRenderVisibility visibility,
                                            JzRenderTargetFeatures features)
{
    if (!m_isInitialized) {
        CreateDefaultPipeline();
        m_isInitialized = true;
    }
    if (!m_defaultPipeline) {
        return;
    }

    auto &device     = JzServiceContainer::Get<JzDevice>();
    auto  targetSize = output.GetSize();

    device.BindFramebuffer(output.GetFramebuffer());
    device.BindPipeline(m_defaultPipeline);

    JzViewport viewport;
    viewport.x        = 0.0f;
    viewport.y        = 0.0f;
    viewport.width    = static_cast<F32>(targetSize.x);
    viewport.height   = static_cast<F32>(targetSize.y);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    device.SetViewport(viewport);

    JzMat4 viewMatrix       = JzMat4x4::Identity();
    JzMat4 projectionMatrix = JzMat4x4::Identity();
    JzVec3 clearColor(0.1f, 0.1f, 0.1f);

    auto cameraView = world.View<JzCameraComponent>();
    if (IsValidEntity(camera) && world.HasComponent<JzCameraComponent>(camera)) {
        const auto &cam  = world.GetComponent<JzCameraComponent>(camera);
        viewMatrix       = cam.viewMatrix;
        projectionMatrix = cam.projectionMatrix;
        clearColor       = cam.clearColor;
    } else {
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

    JzMat4 modelMatrix = JzMat4x4::Identity();
    m_defaultPipeline->SetUniform("model", modelMatrix);
    m_defaultPipeline->SetUniform("view", viewMatrix);
    m_defaultPipeline->SetUniform("projection", projectionMatrix);

    RenderEntitiesFiltered(world, visibility);
    ExecuteRenderPasses(world, features, viewMatrix, projectionMatrix);

    device.BindFramebuffer(nullptr);
}

void JzRenderSystem::RenderEntitiesFiltered(JzWorld &world, JzRenderVisibility visibility)
{
    auto &device       = JzServiceContainer::Get<JzDevice>();
    auto &assetManager = JzServiceContainer::Get<JzAssetManager>();

    const Bool allowOverlay   = HasVisibility(visibility, JzRenderVisibility::Overlay);
    const Bool allowIsolated  = HasVisibility(visibility, JzRenderVisibility::Isolated);
    const Bool allowMainScene = HasVisibility(visibility, JzRenderVisibility::MainScene);

    auto views = world.View<JzTransformComponent, JzMeshAssetComponent, JzMaterialAssetComponent,
                            JzAssetReadyTag>();

    for (auto entity : views) {
        Bool hasOverlayTag  = world.HasComponent<JzOverlayRenderTag>(entity);
        Bool hasIsolatedTag = world.HasComponent<JzIsolatedRenderTag>(entity);
        if (hasOverlayTag && !allowOverlay) {
            continue;
        }
        if (hasIsolatedTag && !allowIsolated) {
            continue;
        }
        if (!hasOverlayTag && !hasIsolatedTag && !allowMainScene) {
            continue;
        }

        auto &transform = world.GetComponent<JzTransformComponent>(entity);
        auto &meshComp  = world.GetComponent<JzMeshAssetComponent>(entity);
        auto &matComp   = world.GetComponent<JzMaterialAssetComponent>(entity);

        auto *mesh = assetManager.Get(meshComp.meshHandle);
        if (!mesh) continue;

        auto vertexArray = mesh->GetVertexArray();
        if (!vertexArray) continue;

        m_defaultPipeline->SetUniform("model", transform.GetWorldMatrix());
        m_defaultPipeline->SetUniform("material.ambient", matComp.ambientColor);
        m_defaultPipeline->SetUniform("material.diffuse", matComp.diffuseColor);
        m_defaultPipeline->SetUniform("material.specular", matComp.specularColor);
        m_defaultPipeline->SetUniform("material.shininess", matComp.shininess);

        JzMaterial *material          = assetManager.Get(matComp.materialHandle);
        Bool        hasDiffuseTexture = material && material->HasDiffuseTexture();
        m_defaultPipeline->SetUniform("hasDiffuseTexture", hasDiffuseTexture);
        if (hasDiffuseTexture) {
            device.BindTexture(material->GetDiffuseTexture(), 0);
            m_defaultPipeline->SetUniform("diffuseTexture", 0);
        }

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

void JzRenderSystem::ExecuteRenderPasses(JzWorld &world, JzRenderTargetFeatures features,
                                         const JzMat4 &viewMatrix,
                                         const JzMat4 &projectionMatrix)
{
    if (m_renderPasses.empty()) {
        return;
    }

    auto &device = JzServiceContainer::Get<JzDevice>();
    for (auto &pass : m_renderPasses) {
        if (!HasFeature(features, pass.feature)) {
            continue;
        }
        if (!pass.pipeline || !pass.vertexArray || pass.drawParams.vertexCount == 0) {
            continue;
        }

        device.BindPipeline(pass.pipeline);
        device.BindVertexArray(pass.vertexArray);
        if (pass.setupPass) {
            pass.setupPass(pass.pipeline, world, viewMatrix, projectionMatrix);
        }
        device.Draw(pass.drawParams);
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
