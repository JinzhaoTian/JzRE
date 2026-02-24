/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Editor/JzREEditor.h"

#include <array>
#include <cstddef>
#include <vector>

#include "JzRE/Runtime/Core/JzLogger.h"
#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Runtime/Function/ECS/JzLightComponents.h"
#include "JzRE/Runtime/Platform/RHI/JzDevice.h"
#include "JzRE/Runtime/Resource/JzShader.h"

namespace {

JzRE::JzRERuntimeSettings CreateSettingsFromPath(JzRE::JzERHIType             rhiType,
                                                 const std::filesystem::path &openPath)
{
    JzRE::JzRERuntimeSettings settings;
    settings.windowTitle     = "JzRE";
    settings.windowSize      = {1280, 720};
    settings.windowDecorated = false;
    settings.rhiType         = rhiType;

    // Check if path is a project file or directory
    if (!openPath.empty()) {
        auto projectFile = JzRE::JzREEditor::FindProjectFile(openPath);
        if (!projectFile.empty()) {
            settings.projectFile = projectFile;
        }
    }

    return settings;
}

std::shared_ptr<JzRE::JzRHIPipeline> LoadEditorContributionPipeline(
    JzRE::JzAssetSystem &assetSystem, const JzRE::String &primaryPath, const JzRE::String &fallbackPath)
{
    const auto tryLoad = [&assetSystem](const JzRE::String &path) -> std::shared_ptr<JzRE::JzRHIPipeline> {
        const auto handle      = assetSystem.LoadSync<JzRE::JzShader>(path);
        auto      *shader = assetSystem.Get(handle);
        if (!shader || !shader->IsCompiled()) {
            return nullptr;
        }

        auto pipeline = shader->GetMainVariant();
        if (!pipeline) {
            return nullptr;
        }
        return pipeline;
    };

    auto pipeline = tryLoad(primaryPath);
    if (!pipeline && !fallbackPath.empty()) {
        pipeline = tryLoad(fallbackPath);
    }
    return pipeline;
}

} // anonymous namespace

struct JzRE::JzREEditor::JzEditorRenderContributionResources {
    std::shared_ptr<JzRHIPipeline>          skyboxPipeline;
    std::shared_ptr<JzRHIPipeline>          linePipeline;
    std::shared_ptr<JzGPUBufferObject>      skyboxVertexBuffer;
    std::shared_ptr<JzGPUVertexArrayObject> skyboxVAO;
    std::shared_ptr<JzGPUBufferObject>      axisVertexBuffer;
    std::shared_ptr<JzGPUVertexArrayObject> axisVAO;
    std::shared_ptr<JzGPUBufferObject>      gridVertexBuffer;
    std::shared_ptr<JzGPUVertexArrayObject> gridVAO;
    U32                                     gridVertexCount = 0;
};

std::filesystem::path JzRE::JzREEditor::FindProjectFile(const std::filesystem::path &path)
{
    if (path.empty()) {
        return {};
    }

    // If path is a file with .jzreproject extension, use it directly
    if (std::filesystem::is_regular_file(path)) {
        if (path.extension() == JzProjectManager::GetProjectFileExtension()) {
            return path;
        }
        return {};
    }

    // If path is a directory, search for .jzreproject file
    if (std::filesystem::is_directory(path)) {
        for (const auto &entry : std::filesystem::directory_iterator(path)) {
            if (entry.is_regular_file() && entry.path().extension() == JzProjectManager::GetProjectFileExtension()) {
                return entry.path();
            }
        }
    }

    return {};
}

JzRE::JzREEditor::JzREEditor(JzERHIType rhiType, const std::filesystem::path &openPath) :
    JzRERuntime(CreateSettingsFromPath(rhiType, openPath)),
    m_openPath(openPath)
{
    // Create editor UI with runtime reference
    m_editorUI = std::make_unique<JzEditorUI>(*this);
}

JzRE::JzREEditor::~JzREEditor()
{
    ReleaseEditorRenderContributions();

    // Clean up editor UI before base class destructor runs
    m_editorUI.reset();
}

JzRE::JzEditorUI &JzRE::JzREEditor::GetEditorUI()
{
    return *m_editorUI;
}

void JzRE::JzREEditor::OnStart()
{
    JzRERuntime::OnStart();

    InitializeEditorRenderContributions();
}

void JzRE::JzREEditor::OnUpdate(F32 deltaTime)
{
    // Update editor logic (shortcuts, mode updates, panel updates)
    // This is called before UpdateSystems() in the main loop
    m_editorUI->Update(deltaTime);
}

void JzRE::JzREEditor::OnRender(F32 deltaTime)
{
    // Render editor views and UI
    // This is called after UpdateSystems() has run (camera, light, render preparation)
    // Rendering order:
    //   1. Game scene (rendered by RenderSystem during UpdateSystems)
    //   2. SceneView (captures game scene to framebuffer)
    //   3. ImGui UI (rendered on top)
    m_editorUI->Render(deltaTime);
}

void JzRE::JzREEditor::OnStop()
{
    ReleaseEditorRenderContributions();
    JzRERuntime::OnStop();
}

void JzRE::JzREEditor::InitializeEditorRenderContributions()
{
    ReleaseEditorRenderContributions();

    if (!m_renderSystem || !m_assetSystem || !JzServiceContainer::Has<JzDevice>()) {
        return;
    }

    auto &device      = JzServiceContainer::Get<JzDevice>();
    auto &assetSystem = *m_assetSystem;

    m_editorRenderContributionResources = std::make_unique<JzEditorRenderContributionResources>();
    auto &resources                     = *m_editorRenderContributionResources;

    resources.skyboxPipeline = LoadEditorContributionPipeline(
        assetSystem, "shaders/editor_skybox.jzshader", "examples/EditorExample/resources/shaders/editor_skybox");
    resources.linePipeline = LoadEditorContributionPipeline(
        assetSystem, "shaders/editor_axis.jzshader", "examples/EditorExample/resources/shaders/editor_axis");

    if (!resources.skyboxPipeline || !resources.linePipeline) {
        JzRE_LOG_WARN("JzREEditor: Editor contribution shaders are not fully available, contribution rendering may be incomplete.");
    }

    constexpr std::array<F32, 6> kSkyboxTriangleVertices = {
        -1.0f, -1.0f,
        -1.0f, 3.0f,
        3.0f, -1.0f};

    {
        JzGPUBufferObjectDesc vbDesc;
        vbDesc.type                  = JzEGPUBufferObjectType::Vertex;
        vbDesc.usage                 = JzEGPUBufferObjectUsage::StaticDraw;
        vbDesc.size                  = kSkyboxTriangleVertices.size() * sizeof(F32);
        vbDesc.data                  = kSkyboxTriangleVertices.data();
        vbDesc.debugName             = "EditorSkyboxScreenTriangleVB";
        resources.skyboxVertexBuffer = device.CreateBuffer(vbDesc);

        resources.skyboxVAO = device.CreateVertexArray("EditorSkyboxScreenTriangleVAO");
        if (resources.skyboxVertexBuffer && resources.skyboxVAO) {
            resources.skyboxVAO->BindVertexBuffer(resources.skyboxVertexBuffer, 0);
            resources.skyboxVAO->SetVertexAttribute(0, 2, static_cast<U32>(2 * sizeof(F32)), 0);
        }
    }

    struct JzLineVertex {
        F32 px, py, pz;
        F32 r, g, b;
    };

    constexpr std::array<JzLineVertex, 6> kAxisVertices = {{{0.0f, 0.0f, 0.0f, 1.0f, 0.2f, 0.2f},
                                                            {1.5f, 0.0f, 0.0f, 1.0f, 0.2f, 0.2f},
                                                            {0.0f, 0.0f, 0.0f, 0.2f, 1.0f, 0.2f},
                                                            {0.0f, 1.5f, 0.0f, 0.2f, 1.0f, 0.2f},
                                                            {0.0f, 0.0f, 0.0f, 0.2f, 0.5f, 1.0f},
                                                            {0.0f, 0.0f, 1.5f, 0.2f, 0.5f, 1.0f}}};

    {
        JzGPUBufferObjectDesc vbDesc;
        vbDesc.type                = JzEGPUBufferObjectType::Vertex;
        vbDesc.usage               = JzEGPUBufferObjectUsage::StaticDraw;
        vbDesc.size                = kAxisVertices.size() * sizeof(JzLineVertex);
        vbDesc.data                = kAxisVertices.data();
        vbDesc.debugName           = "EditorAxisVB";
        resources.axisVertexBuffer = device.CreateBuffer(vbDesc);

        resources.axisVAO = device.CreateVertexArray("EditorAxisVAO");
        if (resources.axisVertexBuffer && resources.axisVAO) {
            resources.axisVAO->BindVertexBuffer(resources.axisVertexBuffer, 0);
            resources.axisVAO->SetVertexAttribute(0, 3, static_cast<U32>(sizeof(JzLineVertex)), 0);
            resources.axisVAO->SetVertexAttribute(1, 3, static_cast<U32>(sizeof(JzLineVertex)),
                                                  static_cast<U32>(offsetof(JzLineVertex, r)));
        }
    }

    constexpr I32 kHalfLineCount = 20;
    constexpr F32 kGridSpacing   = 1.0f;
    constexpr F32 kGridY         = -0.001f;
    constexpr F32 kGridExtent    = kHalfLineCount * kGridSpacing;

    std::vector<JzLineVertex> gridVertices;
    gridVertices.reserve(static_cast<size_t>((kHalfLineCount * 2 + 1) * 4));

    for (I32 i = -kHalfLineCount; i <= kHalfLineCount; ++i) {
        const F32  offset    = static_cast<F32>(i) * kGridSpacing;
        const Bool majorLine = (i == 0) || (i % 5 == 0);
        const F32  c         = majorLine ? 0.36f : 0.24f;

        gridVertices.push_back({-kGridExtent, kGridY, offset, c, c, c});
        gridVertices.push_back({kGridExtent, kGridY, offset, c, c, c});
        gridVertices.push_back({offset, kGridY, -kGridExtent, c, c, c});
        gridVertices.push_back({offset, kGridY, kGridExtent, c, c, c});
    }

    {
        JzGPUBufferObjectDesc vbDesc;
        vbDesc.type                = JzEGPUBufferObjectType::Vertex;
        vbDesc.usage               = JzEGPUBufferObjectUsage::StaticDraw;
        vbDesc.size                = gridVertices.size() * sizeof(JzLineVertex);
        vbDesc.data                = gridVertices.data();
        vbDesc.debugName           = "EditorGridVB";
        resources.gridVertexBuffer = device.CreateBuffer(vbDesc);

        resources.gridVAO = device.CreateVertexArray("EditorGridVAO");
        if (resources.gridVertexBuffer && resources.gridVAO) {
            resources.gridVAO->BindVertexBuffer(resources.gridVertexBuffer, 0);
            resources.gridVAO->SetVertexAttribute(0, 3, static_cast<U32>(sizeof(JzLineVertex)), 0);
            resources.gridVAO->SetVertexAttribute(1, 3, static_cast<U32>(sizeof(JzLineVertex)),
                                                  static_cast<U32>(offsetof(JzLineVertex, r)));
            resources.gridVertexCount = static_cast<U32>(gridVertices.size());
        }
    }

    auto setupLineContribution = [](const std::shared_ptr<JzRHIPipeline>   &pipeline,
                                    const JzRenderGraphContributionContext &context) {
        const JzMat4 model = JzMat4x4::Identity();
        pipeline->SetUniform("model", model);
        pipeline->SetUniform("view", context.viewMatrix);
        pipeline->SetUniform("projection", context.projectionMatrix);
    };

    if (resources.skyboxPipeline && resources.skyboxVAO) {
        JzDrawParams drawParams;
        drawParams.primitiveType = JzEPrimitiveType::Triangles;
        drawParams.vertexCount   = 3;
        drawParams.instanceCount = 1;
        drawParams.firstVertex   = 0;
        drawParams.firstInstance = 0;

        JzRenderGraphContribution skyboxContribution;
        skyboxContribution.name            = "EditorSkyboxContribution";
        skyboxContribution.requiredFeature = JzRenderTargetFeatures::Skybox;
        skyboxContribution.scope           = JzRenderGraphContributionScope::RegisteredTarget;
        skyboxContribution.execute =
            [pipeline = resources.skyboxPipeline, vertexArray = resources.skyboxVAO, drawParams](
                const JzRenderGraphContributionContext &context) {
                if (context.commandList == nullptr) {
                    return;
                }

                auto &commandList = *context.commandList;
                commandList.BindPipeline(pipeline);
                commandList.BindVertexArray(vertexArray);

                JzVec3 sunDirection(0.3f, -1.0f, -0.5f);
                auto   lightView = context.world.View<JzDirectionalLightComponent>();
                if (!lightView.empty()) {
                    sunDirection =
                        context.world.GetComponent<JzDirectionalLightComponent>(lightView.front()).direction;
                }
                if (sunDirection.Length() > 0.0001f) {
                    sunDirection.Normalize();
                }

                pipeline->SetUniform("view", context.viewMatrix);
                pipeline->SetUniform("projection", context.projectionMatrix);
                pipeline->SetUniform("topColor", JzVec3(0.19f, 0.42f, 0.78f));
                pipeline->SetUniform("horizonColor", JzVec3(0.62f, 0.73f, 0.90f));
                pipeline->SetUniform("groundColor", JzVec3(0.20f, 0.21f, 0.24f));
                pipeline->SetUniform("sunDirection", sunDirection);
                pipeline->SetUniform("sunColor", JzVec3(1.0f, 0.95f, 0.80f));
                pipeline->SetUniform("sunSize", 0.04f);
                pipeline->SetUniform("exposure", 1.0f);

                commandList.Draw(drawParams);
            };
        m_renderSystem->RegisterGraphContribution(std::move(skyboxContribution));
    }

    if (resources.linePipeline && resources.axisVAO) {
        JzDrawParams drawParams;
        drawParams.primitiveType = JzEPrimitiveType::Lines;
        drawParams.vertexCount   = static_cast<U32>(kAxisVertices.size());
        drawParams.instanceCount = 1;
        drawParams.firstVertex   = 0;
        drawParams.firstInstance = 0;

        JzRenderGraphContribution axisContribution;
        axisContribution.name            = "EditorAxisContribution";
        axisContribution.requiredFeature = JzRenderTargetFeatures::Axis;
        axisContribution.scope           = JzRenderGraphContributionScope::RegisteredTarget;
        axisContribution.execute =
            [pipeline = resources.linePipeline, vertexArray = resources.axisVAO, drawParams,
             setupLineContribution](const JzRenderGraphContributionContext &context) {
                if (context.commandList == nullptr) {
                    return;
                }

                auto &commandList = *context.commandList;
                commandList.BindPipeline(pipeline);
                commandList.BindVertexArray(vertexArray);
                setupLineContribution(pipeline, context);
                commandList.Draw(drawParams);
            };
        m_renderSystem->RegisterGraphContribution(std::move(axisContribution));
    }

    if (resources.linePipeline && resources.gridVAO && resources.gridVertexCount > 0) {
        JzDrawParams drawParams;
        drawParams.primitiveType = JzEPrimitiveType::Lines;
        drawParams.vertexCount   = resources.gridVertexCount;
        drawParams.instanceCount = 1;
        drawParams.firstVertex   = 0;
        drawParams.firstInstance = 0;

        JzRenderGraphContribution gridContribution;
        gridContribution.name            = "EditorGridContribution";
        gridContribution.requiredFeature = JzRenderTargetFeatures::Grid;
        gridContribution.scope           = JzRenderGraphContributionScope::RegisteredTarget;
        gridContribution.execute =
            [pipeline = resources.linePipeline, vertexArray = resources.gridVAO, drawParams,
             setupLineContribution](const JzRenderGraphContributionContext &context) {
                if (context.commandList == nullptr) {
                    return;
                }

                auto &commandList = *context.commandList;
                commandList.BindPipeline(pipeline);
                commandList.BindVertexArray(vertexArray);
                setupLineContribution(pipeline, context);
                commandList.Draw(drawParams);
            };
        m_renderSystem->RegisterGraphContribution(std::move(gridContribution));
    }
}

void JzRE::JzREEditor::ReleaseEditorRenderContributions()
{
    if (m_renderSystem) {
        m_renderSystem->ClearGraphContributions();
    }
    m_editorRenderContributionResources.reset();
}
