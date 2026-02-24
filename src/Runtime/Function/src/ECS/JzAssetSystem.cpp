/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Function/ECS/JzAssetSystem.h"

#include "JzRE/Runtime/Core/JzLogger.h"
#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Runtime/Function/ECS/JzAssetComponents.h"
#include "JzRE/Runtime/Function/ECS/JzTransformComponents.h"
#include "JzRE/Runtime/Function/ECS/JzWorld.h"
#include "JzRE/Runtime/Resource/JzMaterial.h"
#include "JzRE/Runtime/Resource/JzMesh.h"
#include "JzRE/Runtime/Resource/JzModel.h"
#include "JzRE/Runtime/Resource/JzShader.h"

namespace JzRE {

// ==================== Lifecycle ====================

JzAssetSystem::JzAssetSystem() = default;

JzAssetSystem::~JzAssetSystem()
{
    if (m_assetManager) {
        m_assetManager->Shutdown();
    }
}

void JzAssetSystem::OnInit(JzWorld &world)
{
    JzRE_LOG_INFO("JzAssetSystem: Initialized");
}

void JzAssetSystem::Update(JzWorld &world, F32 delta)
{
    if (!m_assetManager || !m_assetManager->IsInitialized()) {
        return;
    }

    // Process async results and LRU eviction
    m_assetManager->Update();

    // Process asset components (absorbed from JzAssetLoadingSystem)
    ProcessMeshAssets(world);
    ProcessMaterialAssets(world);
    ProcessShaders(world);

    // Hot reload check (development mode only)
    if (m_hotReloadEnabled) {
        m_timeSinceLastCheck += delta;

        if (m_forceCheckNextFrame || m_timeSinceLastCheck >= m_hotReloadCheckInterval) {
            CheckForHotReloadUpdates(world);
            m_timeSinceLastCheck  = 0.0f;
            m_forceCheckNextFrame = false;
        }
    }
}

void JzAssetSystem::OnShutdown(JzWorld &world)
{
    if (world.HasContext<JzAssetManager *>()) {
        world.RemoveContext<JzAssetManager *>();
    }

    if (JzServiceContainer::Has<JzAssetManager>()) {
        JzServiceContainer::Remove<JzAssetManager>();
    }

    if (m_assetManager) {
        m_assetManager->Shutdown();
        m_assetManager.reset();
    }

    JzRE_LOG_INFO("JzAssetSystem: Shutdown");
}

// ==================== Initialization ====================

void JzAssetSystem::Initialize(JzWorld &world, const JzAssetManagerConfig &config)
{
    m_assetManager = std::make_unique<JzAssetManager>(config);
    m_assetManager->Initialize();

    // Initialize hot reload from config
    m_hotReloadEnabled = config.enableHotReload;

    // Register JzAssetManager in service container and world context
    // (JzRenderSystem, JzAssetBrowser use it)
    JzServiceContainer::Provide<JzAssetManager>(*m_assetManager);
    world.SetContext<JzAssetManager *>(m_assetManager.get());

    if (m_hotReloadEnabled) {
        JzRE_LOG_INFO("JzAssetSystem: Hot reload enabled with {}s check interval",
                      m_hotReloadCheckInterval);
    }
}

void JzAssetSystem::AddSearchPath(const String &path)
{
    m_assetManager->AddSearchPath(path);
}

Bool JzAssetSystem::IsInitialized() const
{
    return m_assetManager && m_assetManager->IsInitialized();
}

// ==================== Entity Operations ====================

std::vector<JzEntity> JzAssetSystem::SpawnModel(JzWorld &world, JzModelHandle modelHandle)
{
    std::vector<JzEntity> entities;

    JzModel *model = m_assetManager->Get(modelHandle);
    if (!model) {
        JzRE_LOG_ERROR("JzAssetSystem::SpawnModel: Model is null or not loaded");
        return entities;
    }

    const auto &meshes    = model->GetMeshes();
    const auto &materials = model->GetMaterials();
    const auto &modelPath = model->GetPath();

    for (Size i = 0; i < meshes.size(); ++i) {
        const auto &mesh = meshes[i];
        if (!mesh) {
            continue;
        }

        // Create entity
        auto entity = world.CreateEntity();
        entities.push_back(entity);

        // Add transform component (identity)
        world.AddComponent<JzTransformComponent>(entity);

        // Add asset reference tracking component
        auto &assetRef = world.AddComponent<JzAssetReferenceComponent>(entity);

        // Register mesh as an asset and attach to entity
        auto meshPath   = modelPath + "#mesh" + std::to_string(i);
        auto meshHandle = RegisterAsset<JzMesh>(meshPath, mesh);
        if (meshHandle.IsValid()) {
            auto &meshComp         = world.AddComponent<JzMeshAssetComponent>(entity, meshHandle);
            meshComp.isReady       = true;
            meshComp.indexCount    = mesh->GetIndexCount();
            meshComp.materialIndex = mesh->GetMaterialIndex();
            assetRef.AddMesh(meshHandle);
        }

        // Get associated material (if any)
        I32 matIdx = mesh->GetMaterialIndex();
        if (matIdx >= 0 && static_cast<Size>(matIdx) < materials.size()) {
            const auto &material = materials[matIdx];
            if (material) {
                auto matPath   = modelPath + "#mat" + std::to_string(matIdx);
                auto matHandle = RegisterAsset<JzMaterial>(matPath, material);
                if (matHandle.IsValid()) {
                    auto       &matComp       = world.AddComponent<JzMaterialAssetComponent>(entity, matHandle);
                    const auto &props         = material->GetProperties();
                    matComp.ambientColor      = props.ambientColor;
                    matComp.diffuseColor      = props.diffuseColor;
                    matComp.specularColor     = props.specularColor;
                    matComp.shininess         = props.shininess;
                    matComp.opacity           = props.opacity;
                    matComp.baseColor         = JzVec4(props.diffuseColor.x, props.diffuseColor.y,
                                                       props.diffuseColor.z, props.opacity);
                    matComp.hasDiffuseTexture = material->HasDiffuseTexture();
                    matComp.isReady           = true;
                    matComp.UpdateShaderKeywordMask();
                    assetRef.AddMaterial(matHandle);
                }
            }
        }

        // Mark entity as ready (all assets already loaded)
        world.AddComponent<JzAssetReadyTag>(entity);
    }

    return entities;
}

void JzAssetSystem::AttachMesh(JzWorld &world, JzEntity entity, JzMeshHandle handle)
{
    world.AddOrReplaceComponent<JzMeshAssetComponent>(entity, handle);
    m_assetManager->AddRef(handle);

    if (!world.HasComponent<JzAssetReferenceComponent>(entity)) {
        world.AddComponent<JzAssetReferenceComponent>(entity);
    }
    world.GetComponent<JzAssetReferenceComponent>(entity).AddMesh(handle);
}

void JzAssetSystem::AttachMaterial(JzWorld &world, JzEntity entity, JzMaterialHandle handle)
{
    world.AddOrReplaceComponent<JzMaterialAssetComponent>(entity, handle);
    m_assetManager->AddRef(handle);

    if (!world.HasComponent<JzAssetReferenceComponent>(entity)) {
        world.AddComponent<JzAssetReferenceComponent>(entity);
    }
    world.GetComponent<JzAssetReferenceComponent>(entity).AddMaterial(handle);
}

void JzAssetSystem::AttachShader(JzWorld &world, JzEntity entity, JzShaderHandle handle)
{
    world.AddOrReplaceComponent<JzShaderComponent>(entity, handle);
    m_assetManager->AddRef(handle);

    if (!world.HasComponent<JzAssetReferenceComponent>(entity)) {
        world.AddComponent<JzAssetReferenceComponent>(entity);
    }
    auto &assetRef = world.GetComponent<JzAssetReferenceComponent>(entity);
    if (handle.IsValid()) {
        assetRef.shaderRefs.push_back(handle.GetId());
    }
}

void JzAssetSystem::DetachAllAssets(JzWorld &world, JzEntity entity)
{
    if (!world.IsValid(entity)) {
        return;
    }

    // Release all tracked references
    if (auto *assetRef = world.TryGetComponent<JzAssetReferenceComponent>(entity)) {
        for (const auto &id : assetRef->meshRefs) {
            m_assetManager->Release(JzMeshHandle(id));
        }
        for (const auto &id : assetRef->materialRefs) {
            m_assetManager->Release(JzMaterialHandle(id));
        }
        for (const auto &id : assetRef->textureRefs) {
            m_assetManager->Release(JzTextureHandle(id));
        }
        for (const auto &id : assetRef->modelRefs) {
            m_assetManager->Release(JzModelHandle(id));
        }
        for (const auto &id : assetRef->shaderRefs) {
            m_assetManager->Release(JzShaderHandle(id));
        }
    }

    // Remove all asset-related components and tags
    world.RemoveComponent<JzAssetReferenceComponent>(entity);
    world.RemoveComponent<JzMeshAssetComponent>(entity);
    world.RemoveComponent<JzMaterialAssetComponent>(entity);
    world.RemoveComponent<JzShaderComponent>(entity);
    world.RemoveComponent<JzTextureAssetComponent>(entity);
    world.RemoveComponent<JzModelAssetComponent>(entity);
    world.RemoveComponent<JzAssetLoadingTag>(entity);
    world.RemoveComponent<JzAssetReadyTag>(entity);
    world.RemoveComponent<JzAssetLoadFailedTag>(entity);
}

// ==================== Cache Management ====================

void JzAssetSystem::EvictToTarget(Size targetMemoryMB)
{
    m_assetManager->EvictToTarget(targetMemoryMB);
}

void JzAssetSystem::UnloadUnused()
{
    m_assetManager->UnloadUnused();
}

// ==================== Statistics ====================

Size JzAssetSystem::GetTotalMemoryUsage() const
{
    return m_assetManager ? m_assetManager->GetTotalMemoryUsage() : 0;
}

Size JzAssetSystem::GetPendingLoadCount() const
{
    return m_assetManager ? m_assetManager->GetPendingLoadCount() : 0;
}

// ==================== Internal Access ====================

JzAssetManager &JzAssetSystem::GetAssetManager()
{
    return *m_assetManager;
}

const JzAssetManager &JzAssetSystem::GetAssetManager() const
{
    return *m_assetManager;
}

// ==================== Asset Component Processing ====================
// (Absorbed from JzAssetLoadingSystem)

void JzAssetSystem::ProcessMeshAssets(JzWorld &world)
{
    auto view = world.View<JzMeshAssetComponent>();

    for (auto entity : view) {
        auto &meshComp = view.get<JzMeshAssetComponent>(entity);

        if (meshComp.isReady || !meshComp.meshHandle.IsValid()) {
            continue;
        }

        auto loadState = m_assetManager->GetLoadState(meshComp.meshHandle);

        switch (loadState) {
            case JzEAssetLoadState::Loaded:
            {
                JzMesh *mesh = m_assetManager->Get(meshComp.meshHandle);
                if (mesh) {
                    UpdateMeshComponentCache(meshComp, mesh);
                    meshComp.isReady = true;

                    world.RemoveComponent<JzAssetLoadingTag>(entity);
                    world.AddOrReplaceComponent<JzAssetReadyTag>(entity);

                    JzRE_LOG_DEBUG("JzAssetSystem: Mesh asset ready for entity {}",
                                   static_cast<U32>(entity));
                }
                break;
            }

            case JzEAssetLoadState::Loading:
                if (!world.HasComponent<JzAssetLoadingTag>(entity)) {
                    world.AddComponent<JzAssetLoadingTag>(entity);
                }
                break;

            case JzEAssetLoadState::Failed:
                world.RemoveComponent<JzAssetLoadingTag>(entity);
                world.AddOrReplaceComponent<JzAssetLoadFailedTag>(entity);
                JzRE_LOG_WARN("JzAssetSystem: Mesh asset load failed for entity {}",
                              static_cast<U32>(entity));
                break;

            default:
                break;
        }
    }
}

void JzAssetSystem::ProcessMaterialAssets(JzWorld &world)
{
    auto view = world.View<JzMaterialAssetComponent>();

    for (auto entity : view) {
        auto &matComp = view.get<JzMaterialAssetComponent>(entity);

        if (matComp.isReady || !matComp.materialHandle.IsValid()) {
            continue;
        }

        auto loadState = m_assetManager->GetLoadState(matComp.materialHandle);

        switch (loadState) {
            case JzEAssetLoadState::Loaded:
            {
                JzMaterial *material = m_assetManager->Get(matComp.materialHandle);
                if (material) {
                    UpdateMaterialComponentCache(matComp, material);
                    matComp.isReady = true;

                    JzRE_LOG_DEBUG("JzAssetSystem: Material asset ready for entity {}",
                                   static_cast<U32>(entity));
                }
                break;
            }

            case JzEAssetLoadState::Loading:
                break;

            case JzEAssetLoadState::Failed:
                JzRE_LOG_WARN("JzAssetSystem: Material asset load failed for entity {}",
                              static_cast<U32>(entity));
                break;

            default:
                break;
        }
    }
}

void JzAssetSystem::ProcessShaders(JzWorld &world)
{
    auto view = world.View<JzShaderComponent>();

    for (auto entity : view) {
        auto &shaderComp = view.get<JzShaderComponent>(entity);

        if (shaderComp.isReady || !shaderComp.shaderHandle.IsValid()) {
            continue;
        }

        auto loadState = m_assetManager->GetLoadState(shaderComp.shaderHandle);

        switch (loadState) {
            case JzEAssetLoadState::Loaded:
            {
                JzShader *shader = m_assetManager->Get(shaderComp.shaderHandle);
                if (shader && shader->IsCompiled()) {
                    UpdateShaderComponentCache(shaderComp, shader);
                    shaderComp.isReady = true;

                    JzRE_LOG_DEBUG("JzAssetSystem: Shader ready for entity {}",
                                   static_cast<U32>(entity));
                }
                break;
            }

            case JzEAssetLoadState::Loading:
                if (!world.HasComponent<JzAssetLoadingTag>(entity)) {
                    world.AddComponent<JzAssetLoadingTag>(entity);
                }
                break;

            case JzEAssetLoadState::Failed:
                world.RemoveComponent<JzAssetLoadingTag>(entity);
                world.AddOrReplaceComponent<JzAssetLoadFailedTag>(entity);
                JzRE_LOG_WARN("JzAssetSystem: Shader load failed for entity {}",
                              static_cast<U32>(entity));
                break;

            default:
                break;
        }
    }
}

void JzAssetSystem::UpdateMeshComponentCache(JzMeshAssetComponent &comp, JzMesh *mesh)
{
    if (!mesh) {
        return;
    }
    comp.indexCount    = mesh->GetIndexCount();
    comp.materialIndex = mesh->GetMaterialIndex();
}

void JzAssetSystem::UpdateMaterialComponentCache(JzMaterialAssetComponent &comp, JzMaterial *material)
{
    if (!material) {
        return;
    }

    const auto &props = material->GetProperties();

    comp.ambientColor  = props.ambientColor;
    comp.diffuseColor  = props.diffuseColor;
    comp.specularColor = props.specularColor;
    comp.shininess     = props.shininess;
    comp.opacity       = props.opacity;

    // Convert diffuse to base color for PBR compatibility
    comp.baseColor = JzVec4(props.diffuseColor.x, props.diffuseColor.y, props.diffuseColor.z, props.opacity);

    comp.hasDiffuseTexture  = material->HasDiffuseTexture();
    comp.hasNormalTexture   = false;
    comp.hasSpecularTexture = false;
    comp.UpdateShaderKeywordMask();
}

void JzAssetSystem::UpdateShaderComponentCache(JzShaderComponent &comp, JzShader *shader)
{
    if (!shader) {
        return;
    }

    comp.cachedVariant = shader->GetVariant(comp.keywordMask);
}

void JzAssetSystem::UpdateEntityAssetTags(JzWorld &world, JzEntity entity)
{
    if (!world.IsValid(entity)) {
        return;
    }

    Bool allReady   = true;
    Bool anyFailed  = false;
    Bool anyLoading = false;

    if (auto *meshComp = world.TryGetComponent<JzMeshAssetComponent>(entity)) {
        if (meshComp->meshHandle.IsValid() && !meshComp->isReady) {
            allReady   = false;
            anyLoading = true;
        }
    }

    if (auto *matComp = world.TryGetComponent<JzMaterialAssetComponent>(entity)) {
        if (matComp->materialHandle.IsValid() && !matComp->isReady) {
            allReady   = false;
            anyLoading = true;
        }
    }

    if (auto *shaderComp = world.TryGetComponent<JzShaderComponent>(entity)) {
        if (shaderComp->shaderHandle.IsValid() && !shaderComp->isReady) {
            allReady   = false;
            anyLoading = true;
        }
    }

    if (anyFailed) {
        world.RemoveComponent<JzAssetLoadingTag>(entity);
        world.RemoveComponent<JzAssetReadyTag>(entity);
        world.AddOrReplaceComponent<JzAssetLoadFailedTag>(entity);
    } else if (allReady) {
        world.RemoveComponent<JzAssetLoadingTag>(entity);
        world.RemoveComponent<JzAssetLoadFailedTag>(entity);
        world.AddOrReplaceComponent<JzAssetReadyTag>(entity);
    } else if (anyLoading) {
        world.RemoveComponent<JzAssetReadyTag>(entity);
        world.RemoveComponent<JzAssetLoadFailedTag>(entity);
        world.AddOrReplaceComponent<JzAssetLoadingTag>(entity);
    }
}

// ==================== Hot Reload Configuration ====================

void JzAssetSystem::SetHotReloadEnabled(Bool enabled)
{
    m_hotReloadEnabled = enabled;
    if (enabled) {
        JzRE_LOG_INFO("JzAssetSystem: Hot reload enabled");
    } else {
        JzRE_LOG_INFO("JzAssetSystem: Hot reload disabled");
    }
}

Bool JzAssetSystem::IsHotReloadEnabled() const
{
    return m_hotReloadEnabled;
}

void JzAssetSystem::SetHotReloadCheckInterval(F32 seconds)
{
    m_hotReloadCheckInterval = seconds;
}

F32 JzAssetSystem::GetHotReloadCheckInterval() const
{
    return m_hotReloadCheckInterval;
}

void JzAssetSystem::ForceHotReloadCheck()
{
    m_forceCheckNextFrame = true;
}

Bool JzAssetSystem::ReloadShader(JzShaderHandle shaderHandle)
{
    if (!m_assetManager || !m_assetManager->IsInitialized()) {
        JzRE_LOG_WARN("JzAssetSystem: AssetManager not available for shader reload");
        return false;
    }

    JzShader *shader = m_assetManager->Get(shaderHandle);
    if (!shader) {
        JzRE_LOG_WARN("JzAssetSystem: Shader not found for reload");
        return false;
    }

    Bool success = shader->Reload();
    if (success) {
        ++m_shaderReloadCount;
        ++m_totalReloadCount;
        JzRE_LOG_INFO("JzAssetSystem: Reloaded shader '{}'", shader->GetName());
    } else {
        JzRE_LOG_ERROR("JzAssetSystem: Failed to reload shader '{}'", shader->GetName());
    }

    return success;
}

// ==================== Hot Reload Statistics ====================

Size JzAssetSystem::GetHotReloadCount() const
{
    return m_totalReloadCount;
}

Size JzAssetSystem::GetShaderReloadCount() const
{
    return m_shaderReloadCount;
}

// ==================== Hot Reload Internal ====================

void JzAssetSystem::CheckForHotReloadUpdates(JzWorld &world)
{
    // Check shaders (currently the only asset type with hot reload support)
    CheckShaderHotReload(world);

    // Future: Add CheckTextureHotReload(world);
    // Future: Add CheckMaterialHotReload(world);
}

void JzAssetSystem::CheckShaderHotReload(JzWorld &world)
{
    auto usedShaders = CollectUsedShaders(world);

    for (auto shaderHandle : usedShaders) {
        JzShader *shader = m_assetManager->Get(shaderHandle);
        if (!shader) {
            continue;
        }

        if (shader->NeedsReload()) {
            JzRE_LOG_INFO("JzAssetSystem: Detected change in shader '{}'", shader->GetName());

            if (shader->Reload()) {
                ++m_shaderReloadCount;
                ++m_totalReloadCount;
                NotifyShaderReloaded(shaderHandle, world);
                JzRE_LOG_INFO("JzAssetSystem: Successfully reloaded shader '{}'",
                              shader->GetName());
            } else {
                JzRE_LOG_ERROR("JzAssetSystem: Failed to reload shader '{}'", shader->GetName());
            }
        }
    }
}

void JzAssetSystem::NotifyShaderReloaded(JzShaderHandle shaderHandle, JzWorld &world)
{
    // Mark all entities with this shader as dirty
    auto shaderView = world.View<JzShaderComponent>();
    for (auto entity : shaderView) {
        auto &shaderComp = shaderView.get<JzShaderComponent>(entity);
        if (shaderComp.shaderHandle == shaderHandle) {
            // Reset ready state to trigger recompilation
            shaderComp.isReady       = false;
            shaderComp.cachedVariant = nullptr;

            // Add dirty tag for render systems
            world.AddOrReplaceComponent<JzShaderDirtyTag>(entity);

            JzRE_LOG_DEBUG("JzAssetSystem: Marked entity {} for shader update",
                           static_cast<U32>(entity));
        }
    }

    // Also check material components that reference this shader
    auto materialView = world.View<JzMaterialAssetComponent>();
    for (auto entity : materialView) {
        auto &matComp = materialView.get<JzMaterialAssetComponent>(entity);
        if (matComp.shaderHandle == shaderHandle) {
            // Reset cached variant
            matComp.cachedShaderVariant = nullptr;

            // Add dirty tag
            world.AddOrReplaceComponent<JzShaderDirtyTag>(entity);

            JzRE_LOG_DEBUG("JzAssetSystem: Marked material entity {} for shader update",
                           static_cast<U32>(entity));
        }
    }
}

std::unordered_set<JzShaderHandle, JzAssetHandle<JzShader>::Hash>
JzAssetSystem::CollectUsedShaders(JzWorld &world)
{
    std::unordered_set<JzShaderHandle, JzAssetHandle<JzShader>::Hash> usedShaders;

    // Collect from shader components
    auto shaderView = world.View<JzShaderComponent>();
    for (auto entity : shaderView) {
        auto &shaderComp = shaderView.get<JzShaderComponent>(entity);
        if (shaderComp.shaderHandle.IsValid()) {
            usedShaders.insert(shaderComp.shaderHandle);
        }
    }

    // Collect from material components
    auto materialView = world.View<JzMaterialAssetComponent>();
    for (auto entity : materialView) {
        auto &matComp = materialView.get<JzMaterialAssetComponent>(entity);
        if (matComp.shaderHandle.IsValid()) {
            usedShaders.insert(matComp.shaderHandle);
        }
    }

    return usedShaders;
}

} // namespace JzRE
