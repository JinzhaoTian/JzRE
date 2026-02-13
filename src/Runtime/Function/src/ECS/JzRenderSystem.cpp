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

JzRenderSystem::JzRenderSystem() :
    m_mainOutput(std::make_shared<JzRenderOutput>("MainSceneOutput")) { }

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
        EnsureMainOutput();
        m_frameSizeChanged = false;
    }

    auto geometryPipeline = ResolveGeometryPipeline();
    m_isInitialized       = geometryPipeline != nullptr;

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

    JzRenderGraphContribution geometryContribution;
    geometryContribution.name            = "Geometry";
    geometryContribution.requiredFeature = JzRenderTargetFeatures::None;
    geometryContribution.scope           = JzRenderGraphContributionScope::All;
    geometryContribution.clearTarget     = true;
    geometryContribution.execute =
        [this, geometryPipeline](const JzRenderGraphContributionContext &context) {
            DrawVisibleEntities(context.world, context.visibility, geometryPipeline);
        };

    if (EnsureMainOutput() && m_mainOutput) {
        JzRGTexture mainColor = m_renderGraph.CreateTexture(
            {m_frameSize, JzETextureResourceFormat::RGBA8, false, "MainScene_Color"});
        JzRGTexture mainDepth = m_renderGraph.CreateTexture(
            {m_frameSize, JzETextureResourceFormat::Depth24, false, "MainScene_Depth"});
        m_renderGraph.BindTexture(mainColor, m_mainOutput->GetColorTexture());
        m_renderGraph.BindTexture(mainDepth, m_mainOutput->GetDepthTexture());
        m_renderGraph.BindRenderTarget(mainColor, mainDepth, m_mainOutput->GetFramebuffer());

        m_renderGraph.AddPass({
            "MainScenePass",
            nullptr,
            [this, mainColor, mainDepth](JzRGBuilder &builder) {
                builder.Write(mainColor, JzRGUsage::Write);
                builder.Write(mainDepth, JzRGUsage::Write);
                builder.SetRenderTarget(mainColor, mainDepth);
                builder.SetViewport(m_frameSize);
            },
            [this, &world, geometryPipeline, geometryContribution](const JzRGPassContext &passContext) {
                ExecuteContribution(world, passContext, INVALID_ENTITY,
                                    JzRenderVisibility::MainScene, JzRenderTargetFeatures::None,
                                    geometryPipeline, geometryContribution);
            },
        });

        for (const auto &contribution : m_graphContributions) {
            const auto contributionName = contribution.name;
            if (contributionName.empty()) {
                continue;
            }
            if (!HasContributionScope(contribution.scope, JzRenderGraphContributionScope::MainScene)) {
                continue;
            }

            m_renderGraph.AddPass({
                "MainScene_" + contributionName + "_ContributionPass",
                [this, requiredFeature = contribution.requiredFeature,
                 contributionEnabled = contribution.enabledExecute]() {
                    if (!IsContributionEnabled(JzRenderTargetFeatures::None, requiredFeature)) {
                        return false;
                    }
                    if (contributionEnabled && !contributionEnabled()) {
                        return false;
                    }
                    return true;
                },
                [this, mainColor, mainDepth](JzRGBuilder &builder) {
                    builder.Write(mainColor, JzRGUsage::Write);
                    builder.Write(mainDepth, JzRGUsage::Write);
                    builder.SetRenderTarget(mainColor, mainDepth);
                    builder.SetViewport(m_frameSize);
                },
                [this, &world, contribution, geometryPipeline](const JzRGPassContext &passContext) {
                    ExecuteContribution(world, passContext, INVALID_ENTITY,
                                        JzRenderVisibility::MainScene,
                                        JzRenderTargetFeatures::None, geometryPipeline,
                                        contribution);
                },
            });
        }
    }

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
        m_renderGraph.BindRenderTarget(targetColor, targetDepth, output.GetFramebuffer());

        m_renderGraph.AddPass({
            desc.name + "_GeometryPass",
            desc.shouldRender ? desc.shouldRender : nullptr,
            [desiredSize, targetColor, targetDepth](JzRGBuilder &builder) {
                builder.Write(targetColor, JzRGUsage::Write);
                builder.Write(targetDepth, JzRGUsage::Write);
                builder.SetRenderTarget(targetColor, targetDepth);
                builder.SetViewport(desiredSize);
            },
            [this, &world, camera = desc.camera, visibility = desc.visibility,
             features = desc.features, geometryPipeline,
             geometryContribution](const JzRGPassContext &passContext) {
                ExecuteContribution(world, passContext, camera, visibility, features,
                                    geometryPipeline, geometryContribution);
            },
        });

        for (const auto &contribution : m_graphContributions) {
            const auto contributionName = contribution.name;
            if (contributionName.empty()) {
                continue;
            }
            if (!HasContributionScope(contribution.scope,
                                      JzRenderGraphContributionScope::RegisteredTarget)) {
                continue;
            }

            m_renderGraph.AddPass({
                desc.name + "_" + contributionName + "_ContributionPass",
                [this,
                 shouldRender        = desc.shouldRender,
                 targetFeatures      = desc.features,
                 requiredFeature     = contribution.requiredFeature,
                 contributionEnabled = contribution.enabledExecute]() {
                    if (shouldRender && !shouldRender()) {
                        return false;
                    }
                    if (!IsContributionEnabled(targetFeatures, requiredFeature)) {
                        return false;
                    }
                    if (contributionEnabled && !contributionEnabled()) {
                        return false;
                    }
                    return true;
                },
                [desiredSize, targetColor, targetDepth](JzRGBuilder &builder) {
                    builder.Write(targetColor, JzRGUsage::Write);
                    builder.Write(targetDepth, JzRGUsage::Write);
                    builder.SetRenderTarget(targetColor, targetDepth);
                    builder.SetViewport(desiredSize);
                },
                [this, &world, camera = desc.camera,
                 visibility = desc.visibility, features = desc.features, contribution,
                 geometryPipeline](const JzRGPassContext &passContext) {
                    ExecuteContribution(world, passContext, camera, visibility, features,
                                        geometryPipeline, contribution);
                },
            });
        }
    }

    m_renderGraph.Compile();
    static Bool s_dumpedGraph = false;
    if (!s_dumpedGraph) {
        m_renderGraph.DumpGraph("docs/architecture/render_graph_dump.md");
        s_dumpedGraph = true;
    }

    m_renderGraph.Execute(device);

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
    if (!m_mainOutput) {
        return nullptr;
    }
    return m_mainOutput->GetFramebuffer();
}

std::shared_ptr<JzGPUTextureObject> JzRenderSystem::GetColorTexture() const
{
    if (!m_mainOutput) {
        return nullptr;
    }
    return m_mainOutput->GetColorTexture();
}

std::shared_ptr<JzGPUTextureObject> JzRenderSystem::GetDepthTexture() const
{
    if (!m_mainOutput) {
        return nullptr;
    }
    return m_mainOutput->GetDepthTexture();
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
    if (!m_mainOutput || !m_mainOutput->IsValid()) {
        return;
    }

    auto &device = JzServiceContainer::Get<JzDevice>();
    device.BlitFramebufferToScreen(m_mainOutput->GetFramebuffer(), static_cast<U32>(m_frameSize.x),
                                   static_cast<U32>(m_frameSize.y), screenWidth, screenHeight);
}

Bool JzRenderSystem::IsInitialized() const
{
    return m_isInitialized;
}

void JzRenderSystem::RegisterGraphContribution(JzRenderGraphContribution contribution)
{
    if (contribution.name.empty() || !contribution.execute) {
        return;
    }

    auto iter = std::find_if(m_graphContributions.begin(), m_graphContributions.end(),
                             [&contribution](const JzRenderGraphContribution &entry) {
                                 return entry.name == contribution.name;
                             });
    if (iter != m_graphContributions.end()) {
        *iter = std::move(contribution);
    } else {
        m_graphContributions.push_back(std::move(contribution));
    }
}

void JzRenderSystem::ClearGraphContributions()
{
    m_graphContributions.clear();
}

Bool JzRenderSystem::EnsureMainOutput()
{
    if (!m_mainOutput) {
        m_mainOutput = std::make_shared<JzRenderOutput>("MainSceneOutput");
    }

    if (!m_mainOutput || m_frameSize.x <= 0 || m_frameSize.y <= 0) {
        return false;
    }

    m_mainOutput->EnsureSize(m_frameSize);
    return m_mainOutput->IsValid();
}

std::shared_ptr<JzRHIPipeline> JzRenderSystem::ResolveGeometryPipeline() const
{
    auto &assetManager = JzServiceContainer::Get<JzAssetManager>();
    auto  handle       = assetManager.LoadSync<JzShaderAsset>("shaders/standard");
    auto *shaderAsset  = assetManager.Get(handle);
    if (!shaderAsset || !shaderAsset->IsCompiled()) {
        return nullptr;
    }

    auto mainVariant = shaderAsset->GetMainVariant();
    if (!mainVariant || !mainVariant->IsValid()) {
        return nullptr;
    }

    return mainVariant->GetPipeline();
}

void JzRenderSystem::ExecuteContribution(
    JzWorld &world, const JzRGPassContext &passContext, JzEntity camera,
    JzRenderVisibility visibility, JzRenderTargetFeatures targetFeatures,
    std::shared_ptr<JzRHIPipeline> geometryPipeline, const JzRenderGraphContribution &contribution)
{
    if (!passContext.framebuffer || !contribution.execute) {
        return;
    }
    if (passContext.viewport.x <= 0 || passContext.viewport.y <= 0) {
        return;
    }
    if (!IsContributionEnabled(targetFeatures, contribution.requiredFeature)) {
        return;
    }
    if (contribution.enabledExecute && !contribution.enabledExecute()) {
        return;
    }

    JzMat4 viewMatrix       = JzMat4x4::Identity();
    JzMat4 projectionMatrix = JzMat4x4::Identity();
    JzVec3 clearColor(0.1f, 0.1f, 0.1f);
    ResolveCameraFrameData(world, camera, viewMatrix, projectionMatrix, clearColor);

    if (contribution.clearTarget) {
        if (!geometryPipeline) {
            return;
        }
        BeginRenderTargetPass(passContext, viewMatrix, projectionMatrix, clearColor,
                              std::move(geometryPipeline));
    } else {
        BeginContributionTargetPass(passContext);
    }

    JzRenderGraphContributionContext context{
        world,
        camera,
        visibility,
        targetFeatures,
        passContext.viewport,
        viewMatrix,
        projectionMatrix,
        &passContext};
    contribution.execute(context);
}

void JzRenderSystem::ResolveCameraFrameData(
    JzWorld &world, JzEntity preferredCamera, JzMat4 &viewMatrix,
    JzMat4 &projectionMatrix, JzVec3 &clearColor) const
{
    viewMatrix       = JzMat4x4::Identity();
    projectionMatrix = JzMat4x4::Identity();
    clearColor       = JzVec3(0.1f, 0.1f, 0.1f);

    if (IsValidEntity(preferredCamera) && world.HasComponent<JzCameraComponent>(preferredCamera)) {
        const auto &camera = world.GetComponent<JzCameraComponent>(preferredCamera);
        viewMatrix         = camera.viewMatrix;
        projectionMatrix   = camera.projectionMatrix;
        clearColor         = camera.clearColor;
        return;
    }

    auto cameraView = world.View<JzCameraComponent>();
    for (auto entity : cameraView) {
        const auto &camera = world.GetComponent<JzCameraComponent>(entity);
        if (camera.isMainCamera) {
            viewMatrix       = camera.viewMatrix;
            projectionMatrix = camera.projectionMatrix;
            clearColor       = camera.clearColor;
            return;
        }
    }

    if (!cameraView.empty()) {
        const auto &camera = world.GetComponent<JzCameraComponent>(cameraView.front());
        viewMatrix         = camera.viewMatrix;
        projectionMatrix   = camera.projectionMatrix;
        clearColor         = camera.clearColor;
    }
}

void JzRenderSystem::BeginRenderTargetPass(
    const JzRGPassContext &passContext,
    const JzMat4 &viewMatrix, const JzMat4 &projectionMatrix, const JzVec3 &clearColor,
    std::shared_ptr<JzRHIPipeline> pipeline)
{
    if (!pipeline) {
        return;
    }

    auto &device = passContext.device;
    device.BindFramebuffer(passContext.framebuffer);
    device.BindPipeline(pipeline);

    JzViewport viewport;
    viewport.x        = 0.0f;
    viewport.y        = 0.0f;
    viewport.width    = static_cast<F32>(passContext.viewport.x);
    viewport.height   = static_cast<F32>(passContext.viewport.y);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    device.SetViewport(viewport);

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
    pipeline->SetUniform("model", modelMatrix);
    pipeline->SetUniform("view", viewMatrix);
    pipeline->SetUniform("projection", projectionMatrix);
}

void JzRenderSystem::BeginContributionTargetPass(
    const JzRGPassContext &passContext)
{
    auto &device = passContext.device;
    device.BindFramebuffer(passContext.framebuffer);

    JzViewport viewport;
    viewport.x        = 0.0f;
    viewport.y        = 0.0f;
    viewport.width    = static_cast<F32>(passContext.viewport.x);
    viewport.height   = static_cast<F32>(passContext.viewport.y);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    device.SetViewport(viewport);
}

void JzRenderSystem::CleanupResources()
{
    m_graphContributions.clear();
    m_mainOutput.reset();
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
    record.handle = handle;
    record.desc   = std::move(desc);
    record.output = std::make_shared<JzRenderOutput>(record.desc.name + "_Output");
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

void JzRenderSystem::UpdateRenderTargetFeatures(JzRenderTargetHandle   handle,
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
                                                  JzRenderVisibility   visibility)
{
    auto it = std::find_if(m_renderTargets.begin(), m_renderTargets.end(),
                           [handle](const JzRenderTarget &record) {
                               return record.handle == handle;
                           });
    if (it != m_renderTargets.end()) {
        it->desc.visibility = visibility;
    }
}

void JzRenderSystem::DrawVisibleEntities(JzWorld &world, JzRenderVisibility visibility,
                                         std::shared_ptr<JzRHIPipeline> pipeline)
{
    if (!pipeline) {
        return;
    }

    auto &device       = JzServiceContainer::Get<JzDevice>();
    auto &assetManager = JzServiceContainer::Get<JzAssetManager>();

    auto views = world.View<JzTransformComponent, JzMeshAssetComponent, JzMaterialAssetComponent,
                            JzAssetReadyTag>();

    for (auto entity : views) {
        if (!IsEntityVisible(world, entity, visibility)) {
            continue;
        }
        DrawEntity(world, entity, assetManager, device, pipeline);
    }
}

void JzRenderSystem::DrawEntity(JzWorld &world, JzEntity entity, JzAssetManager &assetManager,
                                JzDevice &device, std::shared_ptr<JzRHIPipeline> pipeline)
{
    if (!pipeline) {
        return;
    }

    auto &transform = world.GetComponent<JzTransformComponent>(entity);
    auto &meshComp  = world.GetComponent<JzMeshAssetComponent>(entity);
    auto &matComp   = world.GetComponent<JzMaterialAssetComponent>(entity);

    auto *mesh = assetManager.Get(meshComp.meshHandle);
    if (!mesh) {
        return;
    }

    auto vertexArray = mesh->GetVertexArray();
    if (!vertexArray) {
        return;
    }

    pipeline->SetUniform("model", transform.GetWorldMatrix());
    pipeline->SetUniform("material.ambient", matComp.ambientColor);
    pipeline->SetUniform("material.diffuse", matComp.diffuseColor);
    pipeline->SetUniform("material.specular", matComp.specularColor);
    pipeline->SetUniform("material.shininess", matComp.shininess);

    JzMaterial *material          = assetManager.Get(matComp.materialHandle);
    Bool        hasDiffuseTexture = material && material->HasDiffuseTexture();
    pipeline->SetUniform("hasDiffuseTexture", hasDiffuseTexture);
    if (hasDiffuseTexture) {
        device.BindTexture(material->GetDiffuseTexture(), 0);
        pipeline->SetUniform("diffuseTexture", 0);
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

Bool JzRenderSystem::IsEntityVisible(JzWorld &world, JzEntity entity,
                                     JzRenderVisibility visibility) const
{
    const Bool allowOverlay   = HasVisibility(visibility, JzRenderVisibility::Overlay);
    const Bool allowIsolated  = HasVisibility(visibility, JzRenderVisibility::Isolated);
    const Bool allowMainScene = HasVisibility(visibility, JzRenderVisibility::MainScene);

    const Bool hasOverlayTag  = world.HasComponent<JzOverlayRenderTag>(entity);
    const Bool hasIsolatedTag = world.HasComponent<JzIsolatedRenderTag>(entity);

    if (hasOverlayTag) {
        return allowOverlay;
    }
    if (hasIsolatedTag) {
        return allowIsolated;
    }

    return allowMainScene;
}

Bool JzRenderSystem::IsContributionEnabled(JzRenderTargetFeatures targetFeatures,
                                           JzRenderTargetFeatures requiredFeature) const
{
    if (requiredFeature == JzRenderTargetFeatures::None) {
        return true;
    }
    return HasFeature(targetFeatures, requiredFeature);
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
