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
#include "JzRE/Runtime/Resource/JzShaderAsset.h"

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

std::shared_ptr<JzRE::JzRHIPipeline> LoadHelperPipeline(
    JzRE::JzAssetSystem &assetSystem, const JzRE::String &primaryPath, const JzRE::String &fallbackPath)
{
    const auto tryLoad = [&assetSystem](const JzRE::String &path) -> std::shared_ptr<JzRE::JzRHIPipeline> {
        const auto handle      = assetSystem.LoadSync<JzRE::JzShaderAsset>(path);
        auto      *shaderAsset = assetSystem.Get(handle);
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
}

} // anonymous namespace

struct JzRE::JzREEditor::JzEditorHelperResources {
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
    ReleaseEditorHelperPasses();

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
    InitializeEditorHelperPasses();
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
    ReleaseEditorHelperPasses();
    JzRERuntime::OnStop();
}

void JzRE::JzREEditor::InitializeEditorHelperPasses()
{
    ReleaseEditorHelperPasses();

    if (!m_renderSystem || !m_assetSystem || !JzServiceContainer::Has<JzDevice>()) {
        return;
    }

    auto &device      = JzServiceContainer::Get<JzDevice>();
    auto &assetSystem = *m_assetSystem;

    m_editorHelperResources = std::make_unique<JzEditorHelperResources>();
    auto &resources         = *m_editorHelperResources;

    resources.skyboxPipeline =
        LoadHelperPipeline(assetSystem, "shaders/editor_skybox", "resources/shaders/editor_skybox");
    resources.linePipeline =
        LoadHelperPipeline(assetSystem, "shaders/editor_axis", "resources/shaders/editor_axis");

    if (!resources.skyboxPipeline || !resources.linePipeline) {
        JzRE_LOG_WARN("JzREEditor: Helper shaders are not fully available, helper rendering may be incomplete.");
    }

    constexpr std::array<F32, 6> kSkyboxTriangleVertices = {
        -1.0f, -1.0f,
        -1.0f, 3.0f,
        3.0f, -1.0f
    };

    {
        JzGPUBufferObjectDesc vbDesc;
        vbDesc.type      = JzEGPUBufferObjectType::Vertex;
        vbDesc.usage     = JzEGPUBufferObjectUsage::StaticDraw;
        vbDesc.size      = kSkyboxTriangleVertices.size() * sizeof(F32);
        vbDesc.data      = kSkyboxTriangleVertices.data();
        vbDesc.debugName = "EditorSkyboxScreenTriangleVB";
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

    constexpr std::array<JzLineVertex, 6> kAxisVertices = {{
        {0.0f, 0.0f, 0.0f, 1.0f, 0.2f, 0.2f},
        {1.5f, 0.0f, 0.0f, 1.0f, 0.2f, 0.2f},
        {0.0f, 0.0f, 0.0f, 0.2f, 1.0f, 0.2f},
        {0.0f, 1.5f, 0.0f, 0.2f, 1.0f, 0.2f},
        {0.0f, 0.0f, 0.0f, 0.2f, 0.5f, 1.0f},
        {0.0f, 0.0f, 1.5f, 0.2f, 0.5f, 1.0f}
    }};

    {
        JzGPUBufferObjectDesc vbDesc;
        vbDesc.type      = JzEGPUBufferObjectType::Vertex;
        vbDesc.usage     = JzEGPUBufferObjectUsage::StaticDraw;
        vbDesc.size      = kAxisVertices.size() * sizeof(JzLineVertex);
        vbDesc.data      = kAxisVertices.data();
        vbDesc.debugName = "EditorAxisVB";
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
        vbDesc.type      = JzEGPUBufferObjectType::Vertex;
        vbDesc.usage     = JzEGPUBufferObjectUsage::StaticDraw;
        vbDesc.size      = gridVertices.size() * sizeof(JzLineVertex);
        vbDesc.data      = gridVertices.data();
        vbDesc.debugName = "EditorGridVB";
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

    auto setupLinePass = [](const std::shared_ptr<JzRHIPipeline> &pipeline,
                            JzWorld &, const JzMat4 &viewMatrix, const JzMat4 &projectionMatrix) {
        const JzMat4 model = JzMat4x4::Identity();
        pipeline->SetUniform("model", model);
        pipeline->SetUniform("view", viewMatrix);
        pipeline->SetUniform("projection", projectionMatrix);
    };

    if (resources.skyboxPipeline && resources.skyboxVAO) {
        JzRenderSystem::JzRenderHelperPass skyboxPass;
        skyboxPass.name       = "EditorSkyboxPass";
        skyboxPass.feature    = JzRenderViewFeatures::Skybox;
        skyboxPass.pipeline   = resources.skyboxPipeline;
        skyboxPass.vertexArray = resources.skyboxVAO;
        skyboxPass.drawParams.primitiveType = JzEPrimitiveType::Triangles;
        skyboxPass.drawParams.vertexCount   = 3;
        skyboxPass.drawParams.instanceCount = 1;
        skyboxPass.drawParams.firstVertex   = 0;
        skyboxPass.drawParams.firstInstance = 0;
        skyboxPass.setupPass = [](const std::shared_ptr<JzRHIPipeline> &pipeline,
                                  JzWorld &world, const JzMat4 &viewMatrix, const JzMat4 &projectionMatrix) {
            JzVec3 sunDirection(0.3f, -1.0f, -0.5f);
            auto   lightView = world.View<JzDirectionalLightComponent>();
            if (!lightView.empty()) {
                sunDirection = world.GetComponent<JzDirectionalLightComponent>(lightView.front()).direction;
            }
            if (sunDirection.Length() > 0.0001f) {
                sunDirection.Normalize();
            }

            pipeline->SetUniform("view", viewMatrix);
            pipeline->SetUniform("projection", projectionMatrix);
            pipeline->SetUniform("topColor", JzVec3(0.19f, 0.42f, 0.78f));
            pipeline->SetUniform("horizonColor", JzVec3(0.62f, 0.73f, 0.90f));
            pipeline->SetUniform("groundColor", JzVec3(0.20f, 0.21f, 0.24f));
            pipeline->SetUniform("sunDirection", sunDirection);
            pipeline->SetUniform("sunColor", JzVec3(1.0f, 0.95f, 0.80f));
            pipeline->SetUniform("sunSize", 0.04f);
            pipeline->SetUniform("exposure", 1.0f);
        };
        m_renderSystem->RegisterHelperPass(std::move(skyboxPass));
    }

    if (resources.linePipeline && resources.axisVAO) {
        JzRenderSystem::JzRenderHelperPass axisPass;
        axisPass.name        = "EditorAxisPass";
        axisPass.feature     = JzRenderViewFeatures::Axis;
        axisPass.pipeline    = resources.linePipeline;
        axisPass.vertexArray = resources.axisVAO;
        axisPass.drawParams.primitiveType = JzEPrimitiveType::Lines;
        axisPass.drawParams.vertexCount   = static_cast<U32>(kAxisVertices.size());
        axisPass.drawParams.instanceCount = 1;
        axisPass.drawParams.firstVertex   = 0;
        axisPass.drawParams.firstInstance = 0;
        axisPass.setupPass = setupLinePass;
        m_renderSystem->RegisterHelperPass(std::move(axisPass));
    }

    if (resources.linePipeline && resources.gridVAO && resources.gridVertexCount > 0) {
        JzRenderSystem::JzRenderHelperPass gridPass;
        gridPass.name        = "EditorGridPass";
        gridPass.feature     = JzRenderViewFeatures::Grid;
        gridPass.pipeline    = resources.linePipeline;
        gridPass.vertexArray = resources.gridVAO;
        gridPass.drawParams.primitiveType = JzEPrimitiveType::Lines;
        gridPass.drawParams.vertexCount   = resources.gridVertexCount;
        gridPass.drawParams.instanceCount = 1;
        gridPass.drawParams.firstVertex   = 0;
        gridPass.drawParams.firstInstance = 0;
        gridPass.setupPass = setupLinePass;
        m_renderSystem->RegisterHelperPass(std::move(gridPass));
    }
}

void JzRE::JzREEditor::ReleaseEditorHelperPasses()
{
    if (m_renderSystem) {
        m_renderSystem->ClearHelperPasses();
    }
    m_editorHelperResources.reset();
}
