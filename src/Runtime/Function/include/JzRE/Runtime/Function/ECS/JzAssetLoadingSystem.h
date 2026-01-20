/**
 * @file    JzAssetLoadingSystem.h
 * @brief   ECS system for coordinating asset loading
 * @author  Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Function/ECS/JzEnttSystem.h"

namespace JzRE {

// Forward declarations
class JzEnttWorld;
class JzAssetManager;
class JzMesh;
class JzMaterial;
struct JzMeshAssetComponent;
struct JzMaterialAssetComponent;

/**
 * @brief System for coordinating asset loading with ECS
 *
 * This system:
 * 1. Monitors entities with asset components (JzMeshAssetComponent, etc.)
 * 2. Checks if referenced assets are loaded
 * 3. Updates component cache data when assets become ready
 * 4. Manages asset loading state tags (JzAssetLoadingTag, JzAssetReadyTag)
 * 5. Handles asset reference cleanup on entity destruction
 *
 * Execution phase: Logic (runs before rendering to prepare data)
 *
 * @example
 * @code
 * // Create entity with asset components
 * auto entity = world.CreateEntity();
 * auto meshHandle = assetManager.LoadAsync<JzMesh>("cube.obj");
 * world.AddComponent<JzMeshAssetComponent>(entity, meshHandle);
 * world.AddComponent<JzTransformComponent>(entity);
 *
 * // AssetLoadingSystem will:
 * // 1. Add JzAssetLoadingTag while mesh is loading
 * // 2. Update JzMeshAssetComponent.indexCount etc. when loaded
 * // 3. Replace JzAssetLoadingTag with JzAssetReadyTag
 *
 * // Render systems can filter by JzAssetReadyTag for efficiency
 * @endcode
 */
class JzAssetLoadingSystem : public JzEnttSystem {
public:
    /**
     * @brief Default constructor
     */
    JzAssetLoadingSystem();

    /**
     * @brief Destructor
     */
    ~JzAssetLoadingSystem() override;

    /**
     * @brief Initialize the system
     *
     * Sets up entity destruction listeners for cleanup.
     *
     * @param world The ECS world
     */
    void OnInit(JzEnttWorld& world) override;

    /**
     * @brief Update asset loading state
     *
     * @param world The ECS world
     * @param delta Delta time (unused but required by interface)
     */
    void Update(JzEnttWorld& world, F32 delta) override;

    /**
     * @brief Cleanup when system is destroyed
     *
     * @param world The ECS world
     */
    void OnShutdown(JzEnttWorld& world) override;

    /**
     * @brief Get system execution phase
     *
     * @return JzSystemPhase::Logic
     */
    JzSystemPhase GetPhase() const override
    {
        return JzSystemPhase::Logic;
    }

private:
    /**
     * @brief Process mesh asset components
     *
     * Checks load state and updates component cache for all
     * JzMeshAssetComponent instances.
     */
    void ProcessMeshAssets(JzEnttWorld& world, JzAssetManager& assetManager);

    /**
     * @brief Process material asset components
     */
    void ProcessMaterialAssets(JzEnttWorld& world, JzAssetManager& assetManager);

    /**
     * @brief Update cached data in mesh component from loaded asset
     */
    void UpdateMeshComponentCache(JzMeshAssetComponent& comp, JzMesh* mesh);

    /**
     * @brief Update cached data in material component from loaded asset
     */
    void UpdateMaterialComponentCache(JzMaterialAssetComponent& comp, JzMaterial* material);

    /**
     * @brief Update asset state tags for an entity
     *
     * Adds/removes JzAssetLoadingTag, JzAssetReadyTag, JzAssetLoadFailedTag
     * based on the load state of all asset components on the entity.
     */
    void UpdateEntityAssetTags(JzEnttWorld& world, U32 entityId);
};

} // namespace JzRE
