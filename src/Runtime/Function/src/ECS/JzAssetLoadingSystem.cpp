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
#include "JzRE/Runtime/Function/ECS/JzEnttWorld.h"
#include "JzRE/Runtime/Resource/JzAssetManager.h"
#include "JzRE/Runtime/Resource/JzMaterial.h"
#include "JzRE/Runtime/Resource/JzMesh.h"

namespace JzRE {

JzAssetLoadingSystem::JzAssetLoadingSystem() = default;

JzAssetLoadingSystem::~JzAssetLoadingSystem() = default;

void JzAssetLoadingSystem::OnInit(JzEnttWorld &world)
{
    JzRE_LOG_INFO("JzAssetLoadingSystem: Initialized");
}

void JzAssetLoadingSystem::Update(JzEnttWorld &world, F32 delta)
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
}

void JzAssetLoadingSystem::OnShutdown(JzEnttWorld &world)
{
    JzRE_LOG_INFO("JzAssetLoadingSystem: Shutdown");
}

void JzAssetLoadingSystem::ProcessMeshAssets(JzEnttWorld &world, JzAssetManager &assetManager)
{
    auto &registry = world.GetRegistry();

    // Query all entities with JzMeshAssetComponent
    auto view = registry.view<JzMeshAssetComponent>();

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
                    registry.remove<JzAssetLoadingTag>(entity);
                    registry.emplace_or_replace<JzAssetReadyTag>(entity);

                    JzRE_LOG_DEBUG("JzAssetLoadingSystem: Mesh asset ready for entity {}",
                                   static_cast<U32>(entity));
                }
                break;
            }

            case JzEAssetLoadState::Loading:
                // Still loading - ensure loading tag is present
                if (!registry.all_of<JzAssetLoadingTag>(entity)) {
                    registry.emplace<JzAssetLoadingTag>(entity);
                }
                break;

            case JzEAssetLoadState::Failed:
                // Load failed - mark as failed
                registry.remove<JzAssetLoadingTag>(entity);
                registry.emplace_or_replace<JzAssetLoadFailedTag>(entity);
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

void JzAssetLoadingSystem::ProcessMaterialAssets(JzEnttWorld &world, JzAssetManager &assetManager)
{
    auto &registry = world.GetRegistry();

    // Query all entities with JzMaterialAssetComponent
    auto view = registry.view<JzMaterialAssetComponent>();

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

void JzAssetLoadingSystem::UpdateEntityAssetTags(JzEnttWorld &world, U32 entityId)
{
    auto &registry = world.GetRegistry();
    auto  entity   = static_cast<entt::entity>(entityId);

    if (!registry.valid(entity)) {
        return;
    }

    Bool allReady   = true;
    Bool anyFailed  = false;
    Bool anyLoading = false;

    // Check mesh component
    if (auto *meshComp = registry.try_get<JzMeshAssetComponent>(entity)) {
        if (meshComp->meshHandle.IsValid()) {
            if (!meshComp->isReady) {
                allReady = false;
                // Check if loading or failed would require querying AssetManager
                anyLoading = true;
            }
        }
    }

    // Check material component
    if (auto *matComp = registry.try_get<JzMaterialAssetComponent>(entity)) {
        if (matComp->materialHandle.IsValid()) {
            if (!matComp->isReady) {
                allReady   = false;
                anyLoading = true;
            }
        }
    }

    // Update tags based on state
    if (anyFailed) {
        registry.remove<JzAssetLoadingTag>(entity);
        registry.remove<JzAssetReadyTag>(entity);
        registry.emplace_or_replace<JzAssetLoadFailedTag>(entity);
    } else if (allReady) {
        registry.remove<JzAssetLoadingTag>(entity);
        registry.remove<JzAssetLoadFailedTag>(entity);
        registry.emplace_or_replace<JzAssetReadyTag>(entity);
    } else if (anyLoading) {
        registry.remove<JzAssetReadyTag>(entity);
        registry.remove<JzAssetLoadFailedTag>(entity);
        registry.emplace_or_replace<JzAssetLoadingTag>(entity);
    }
}

} // namespace JzRE
