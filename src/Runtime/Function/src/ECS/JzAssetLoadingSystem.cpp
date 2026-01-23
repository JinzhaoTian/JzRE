/**
 * @file    JzAssetLoadingSystem.cpp
 * @brief   Implementation of the asset loading coordination system
 * @author  Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Function/ECS/JzAssetLoadingSystem.h"

#include "JzRE/Runtime/Core/JzLogger.h"
#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Runtime/Function/ECS/JzAssetComponents.h"
#include "JzRE/Runtime/Function/ECS/JzEntity.h"
#include "JzRE/Runtime/Function/ECS/JzWorld.h"
#include "JzRE/Runtime/Resource/JzAssetManager.h"
#include "JzRE/Runtime/Resource/JzMaterial.h"
#include "JzRE/Runtime/Resource/JzMesh.h"
#include "JzRE/Runtime/Resource/JzShaderAsset.h"

namespace JzRE {

JzAssetLoadingSystem::JzAssetLoadingSystem() = default;

JzAssetLoadingSystem::~JzAssetLoadingSystem() = default;

void JzAssetLoadingSystem::OnInit(JzWorld &world)
{
    JzRE_LOG_INFO("JzAssetLoadingSystem: Initialized");
}

void JzAssetLoadingSystem::Update(JzWorld &world, F32 delta)
{
    // Get asset manager from service container
    JzAssetManager *assetManager = nullptr;
    try {
        assetManager = &JzServiceContainer::Get<JzAssetManager>();
    } catch (...) {
        // Asset manager not registered - skip processing
        return;
    }

    if (!assetManager || !assetManager->IsInitialized()) {
        return;
    }

    // Update the asset manager (process async results)
    assetManager->Update();

    // Process asset components
    ProcessMeshAssets(world, *assetManager);
    ProcessMaterialAssets(world, *assetManager);
    ProcessShaderAssets(world, *assetManager);
}

void JzAssetLoadingSystem::OnShutdown(JzWorld &world)
{
    JzRE_LOG_INFO("JzAssetLoadingSystem: Shutdown");
}

void JzAssetLoadingSystem::ProcessMeshAssets(JzWorld &world, JzAssetManager &assetManager)
{
    // Query all entities with JzMeshAssetComponent
    auto view = world.View<JzMeshAssetComponent>();

    for (auto entity : view) {
        auto &meshComp = view.get<JzMeshAssetComponent>(entity);

        // Skip if already ready
        if (meshComp.isReady) {
            continue;
        }

        // Skip if no valid handle
        if (!meshComp.meshHandle.IsValid()) {
            continue;
        }

        // Check load state
        auto loadState = assetManager.GetLoadState(meshComp.meshHandle);

        switch (loadState) {
            case JzEAssetLoadState::Loaded:
            {
                // Asset is loaded - update cache and mark ready
                JzMesh *mesh = assetManager.Get(meshComp.meshHandle);
                if (mesh) {
                    UpdateMeshComponentCache(meshComp, mesh);
                    meshComp.isReady = true;

                    // Update tags
                    world.RemoveComponent<JzAssetLoadingTag>(entity);
                    world.AddOrReplaceComponent<JzAssetReadyTag>(entity);

                    JzRE_LOG_DEBUG("JzAssetLoadingSystem: Mesh asset ready for entity {}",
                                   static_cast<U32>(entity));
                }
                break;
            }

            case JzEAssetLoadState::Loading:
                // Still loading - ensure loading tag is present
                if (!world.HasComponent<JzAssetLoadingTag>(entity)) {
                    world.AddComponent<JzAssetLoadingTag>(entity);
                }
                break;

            case JzEAssetLoadState::Failed:
                // Load failed - mark as failed
                world.RemoveComponent<JzAssetLoadingTag>(entity);
                world.AddOrReplaceComponent<JzAssetLoadFailedTag>(entity);
                JzRE_LOG_WARN("JzAssetLoadingSystem: Mesh asset load failed for entity {}",
                              static_cast<U32>(entity));
                break;

            case JzEAssetLoadState::NotLoaded:
                // Asset not yet requested - this shouldn't happen normally
                // The caller should have triggered the load
                break;

            default:
                break;
        }
    }
}

void JzAssetLoadingSystem::ProcessMaterialAssets(JzWorld &world, JzAssetManager &assetManager)
{
    // Query all entities with JzMaterialAssetComponent
    auto view = world.View<JzMaterialAssetComponent>();

    for (auto entity : view) {
        auto &matComp = view.get<JzMaterialAssetComponent>(entity);

        // Skip if already ready
        if (matComp.isReady) {
            continue;
        }

        // Skip if no valid handle
        if (!matComp.materialHandle.IsValid()) {
            continue;
        }

        // Check load state
        auto loadState = assetManager.GetLoadState(matComp.materialHandle);

        switch (loadState) {
            case JzEAssetLoadState::Loaded:
            {
                // Asset is loaded - update cache and mark ready
                JzMaterial *material = assetManager.Get(matComp.materialHandle);
                if (material) {
                    UpdateMaterialComponentCache(matComp, material);
                    matComp.isReady = true;

                    JzRE_LOG_DEBUG("JzAssetLoadingSystem: Material asset ready for entity {}",
                                   static_cast<U32>(entity));
                }
                break;
            }

            case JzEAssetLoadState::Loading:
                // Still loading
                break;

            case JzEAssetLoadState::Failed:
                // Load failed
                JzRE_LOG_WARN("JzAssetLoadingSystem: Material asset load failed for entity {}",
                              static_cast<U32>(entity));
                break;

            default:
                break;
        }
    }
}

void JzAssetLoadingSystem::UpdateMeshComponentCache(JzMeshAssetComponent &comp, JzMesh *mesh)
{
    if (!mesh) {
        return;
    }

    comp.indexCount    = mesh->GetIndexCount();
    comp.materialIndex = mesh->GetMaterialIndex();
}

void JzAssetLoadingSystem::UpdateMaterialComponentCache(JzMaterialAssetComponent &comp, JzMaterial *material)
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
}

void JzAssetLoadingSystem::ProcessShaderAssets(JzWorld &world, JzAssetManager &assetManager)
{
    // Query all entities with JzShaderAssetComponent
    auto view = world.View<JzShaderAssetComponent>();

    for (auto entity : view) {
        auto &shaderComp = view.get<JzShaderAssetComponent>(entity);

        // Skip if already ready
        if (shaderComp.isReady) {
            continue;
        }

        // Skip if no valid handle
        if (!shaderComp.shaderHandle.IsValid()) {
            continue;
        }

        // Check load state
        auto loadState = assetManager.GetLoadState(shaderComp.shaderHandle);

        switch (loadState) {
            case JzEAssetLoadState::Loaded:
            {
                // Asset is loaded - update cache and mark ready
                JzShaderAsset *shader = assetManager.Get(shaderComp.shaderHandle);
                if (shader && shader->IsCompiled()) {
                    UpdateShaderComponentCache(shaderComp, shader);
                    shaderComp.isReady = true;

                    JzRE_LOG_DEBUG("JzAssetLoadingSystem: Shader asset ready for entity {}",
                                   static_cast<U32>(entity));
                }
                break;
            }

            case JzEAssetLoadState::Loading:
                // Still loading - ensure loading tag is present
                if (!world.HasComponent<JzAssetLoadingTag>(entity)) {
                    world.AddComponent<JzAssetLoadingTag>(entity);
                }
                break;

            case JzEAssetLoadState::Failed:
                // Load failed - mark as failed
                world.RemoveComponent<JzAssetLoadingTag>(entity);
                world.AddOrReplaceComponent<JzAssetLoadFailedTag>(entity);
                JzRE_LOG_WARN("JzAssetLoadingSystem: Shader asset load failed for entity {}",
                              static_cast<U32>(entity));
                break;

            case JzEAssetLoadState::NotLoaded:
                // Asset not yet requested
                break;

            default:
                break;
        }
    }
}

void JzAssetLoadingSystem::UpdateShaderComponentCache(JzShaderAssetComponent &comp, JzShaderAsset *shader)
{
    if (!shader) {
        return;
    }

    // Get the variant based on component's defines
    if (comp.shaderDefines.empty()) {
        // Use main variant if no defines specified
        comp.cachedVariant = shader->GetMainVariant();
    } else {
        // Get or compile variant with specified defines
        comp.cachedVariant = shader->GetVariant(comp.shaderDefines);
    }
}

void JzAssetLoadingSystem::UpdateEntityAssetTags(JzWorld &world, U32 entityId)
{
    auto entity = static_cast<JzEntity>(entityId);

    if (!world.IsValid(entity)) {
        return;
    }

    Bool allReady   = true;
    Bool anyFailed  = false;
    Bool anyLoading = false;

    // Check mesh component
    if (auto *meshComp = world.TryGetComponent<JzMeshAssetComponent>(entity)) {
        if (meshComp->meshHandle.IsValid()) {
            if (!meshComp->isReady) {
                allReady = false;
                // Check if loading or failed would require querying AssetManager
                anyLoading = true;
            }
        }
    }

    // Check material component
    if (auto *matComp = world.TryGetComponent<JzMaterialAssetComponent>(entity)) {
        if (matComp->materialHandle.IsValid()) {
            if (!matComp->isReady) {
                allReady   = false;
                anyLoading = true;
            }
        }
    }

    // Check shader component
    if (auto *shaderComp = world.TryGetComponent<JzShaderAssetComponent>(entity)) {
        if (shaderComp->shaderHandle.IsValid()) {
            if (!shaderComp->isReady) {
                allReady   = false;
                anyLoading = true;
            }
        }
    }

    // Update tags based on state
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

} // namespace JzRE
