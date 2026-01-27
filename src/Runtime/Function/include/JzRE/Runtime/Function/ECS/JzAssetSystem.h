/**
 * @file    JzAssetSystem.h
 * @brief   ECS system facade for asset management
 * @author  Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 *
 * JzAssetSystem is the primary entry point for all asset operations.
 * It wraps JzAssetManager (Resource layer) and provides high-level
 * ECS-oriented operations for loading, registering, and attaching
 * assets to entities.
 *
 * @example
 * @code
 * auto &assetSystem = JzServiceContainer::Get<JzAssetSystem>();
 *
 * // Load a model
 * auto modelHandle = assetSystem.LoadSync<JzModel>("cube.obj");
 * assetSystem.AddRef(modelHandle);
 *
 * // Spawn entities from the model
 * auto entities = assetSystem.SpawnModel(world, modelHandle);
 *
 * // Cleanup
 * for (auto entity : entities) {
 *     assetSystem.DetachAllAssets(world, entity);
 *     world.DestroyEntity(entity);
 * }
 * assetSystem.Release(modelHandle);
 * @endcode
 */

#pragma once

#include <memory>
#include <vector>

#include "JzRE/Runtime/Function/ECS/JzEntity.h"
#include "JzRE/Runtime/Function/ECS/JzSystem.h"
#include "JzRE/Runtime/Resource/JzAssetHandle.h"
#include "JzRE/Runtime/Resource/JzAssetManager.h"
#include "JzRE/Runtime/Resource/JzMesh.h"
#include "JzRE/Runtime/Resource/JzMaterial.h"
#include "JzRE/Runtime/Resource/JzTexture.h"
#include "JzRE/Runtime/Resource/JzModel.h"
#include "JzRE/Runtime/Resource/JzShaderAsset.h"

namespace JzRE {

// Forward declarations
class JzWorld;
class JzModel;
class JzMesh;
class JzMaterial;
class JzShaderAsset;
class JzResourceFactory;
struct JzMeshAssetComponent;
struct JzMaterialAssetComponent;
struct JzShaderAssetComponent;

/**
 * @brief ECS system facade for asset management
 *
 * This system:
 * 1. Owns and manages JzAssetManager lifecycle
 * 2. Provides high-level API for loading, registering, and accessing assets
 * 3. Processes asset components each frame (cache updates, tag management)
 * 4. Spawns entities from loaded models
 * 5. Hides low-level registry operations from external consumers
 *
 * Execution phase: Logic (runs before rendering to prepare data)
 */
class JzAssetSystem : public JzSystem {
public:
    JzAssetSystem();
    ~JzAssetSystem() override;

    // ==================== ECS System Lifecycle ====================

    void OnInit(JzWorld &world) override;
    void Update(JzWorld &world, F32 delta) override;
    void OnShutdown(JzWorld &world) override;

    JzSystemPhase GetPhase() const override
    {
        return JzSystemPhase::Logic;
    }

    // ==================== Initialization ====================

    /**
     * @brief Initialize the asset system with configuration
     *
     * Creates and initializes the internal JzAssetManager.
     * Must be called before using any asset operations.
     *
     * @param config Asset manager configuration
     */
    void Initialize(const JzAssetManagerConfig &config);

    /**
     * @brief Register a resource factory for a given asset type
     */
    template <typename T>
    void RegisterFactory(std::unique_ptr<JzResourceFactory> factory);

    /**
     * @brief Add a search path for locating asset files
     */
    void AddSearchPath(const String &path);

    /**
     * @brief Check if the asset system is initialized
     */
    [[nodiscard]] Bool IsInitialized() const;

    // ==================== Loading API ====================

    /**
     * @brief Load an asset synchronously (blocking)
     *
     * @tparam T Asset type
     * @param path Asset path
     * @return Handle to the loaded asset, or invalid handle on failure
     */
    template <typename T>
    JzAssetHandle<T> LoadSync(const String &path);

    /**
     * @brief Get or load an asset (returns cached if available)
     */
    template <typename T>
    JzAssetHandle<T> GetOrLoad(const String &path);

    /**
     * @brief Load an asset asynchronously
     */
    template <typename T>
    JzAssetHandle<T> LoadAsync(const String          &path,
                               JzAssetLoadCallback<T> callback = nullptr,
                               I32                    priority = 0);

    // ==================== Registration API ====================

    /**
     * @brief Register an already-loaded asset and get a handle
     *
     * Allocates a registry slot, sets the asset data, marks as Loaded,
     * and increments the reference count.
     *
     * This replaces the manual sequence of:
     *   GetRegistry<T>().Allocate() + Set() + SetLoadState(Loaded) + AddRef()
     *
     * @tparam T Asset type
     * @param path Unique path identifier for the asset
     * @param asset The loaded asset data
     * @return Handle to the registered asset
     */
    template <typename T>
    JzAssetHandle<T> RegisterAsset(const String &path, std::shared_ptr<T> asset);

    // ==================== Access API ====================

    /**
     * @brief Get raw pointer to asset data
     */
    template <typename T>
    T *Get(JzAssetHandle<T> handle);

    /**
     * @brief Get const pointer to asset data
     */
    template <typename T>
    const T *Get(JzAssetHandle<T> handle) const;

    /**
     * @brief Get shared_ptr to asset (for compatibility)
     */
    template <typename T>
    std::shared_ptr<T> GetShared(JzAssetHandle<T> handle);

    /**
     * @brief Check if handle is valid (generation check)
     */
    template <typename T>
    Bool IsValid(JzAssetHandle<T> handle) const;

    /**
     * @brief Check if asset is loaded and ready
     */
    template <typename T>
    Bool IsLoaded(JzAssetHandle<T> handle) const;

    /**
     * @brief Get load state of an asset
     */
    template <typename T>
    JzEAssetLoadState GetLoadState(JzAssetHandle<T> handle) const;

    // ==================== Reference Counting ====================

    /**
     * @brief Increment reference count
     */
    template <typename T>
    void AddRef(JzAssetHandle<T> handle);

    /**
     * @brief Decrement reference count
     */
    template <typename T>
    void Release(JzAssetHandle<T> handle);

    // ==================== Entity Operations ====================

    /**
     * @brief Spawn ECS entities from a loaded model
     *
     * Creates one entity per mesh with:
     * - JzTransformComponent (identity)
     * - JzMeshAssetComponent (with cached data populated)
     * - JzMaterialAssetComponent (with cached properties, if material exists)
     * - JzAssetReferenceComponent (tracks all asset refs)
     * - JzAssetReadyTag (since sub-assets are registered as loaded)
     *
     * @param world The ECS world
     * @param modelHandle Handle to the loaded model
     * @return Vector of created entity IDs
     */
    std::vector<JzEntity> SpawnModel(JzWorld &world, JzModelHandle modelHandle);

    /**
     * @brief Attach a mesh asset to an entity
     *
     * Adds JzMeshAssetComponent and updates JzAssetReferenceComponent.
     */
    void AttachMesh(JzWorld &world, JzEntity entity, JzMeshHandle handle);

    /**
     * @brief Attach a material asset to an entity
     */
    void AttachMaterial(JzWorld &world, JzEntity entity, JzMaterialHandle handle);

    /**
     * @brief Attach a shader asset to an entity
     */
    void AttachShader(JzWorld &world, JzEntity entity, JzShaderAssetHandle handle);

    /**
     * @brief Detach all asset references from an entity and release refs
     */
    void DetachAllAssets(JzWorld &world, JzEntity entity);

    // ==================== Cache Management ====================

    /**
     * @brief Evict assets to reach target memory
     */
    void EvictToTarget(Size targetMemoryMB);

    /**
     * @brief Unload all assets with zero reference count
     */
    void UnloadUnused();

    // ==================== Statistics ====================

    [[nodiscard]] Size GetTotalMemoryUsage() const;
    [[nodiscard]] Size GetPendingLoadCount() const;

    // ==================== Internal Access ====================

    /**
     * @brief Get the underlying asset manager
     *
     * Prefer the high-level API methods above. Use this only when
     * low-level registry access is truly necessary.
     */
    JzAssetManager       &GetAssetManager();
    const JzAssetManager &GetAssetManager() const;

private:
    // ==================== Asset Component Processing ====================
    // (Absorbed from JzAssetLoadingSystem)

    void ProcessMeshAssets(JzWorld &world);
    void ProcessMaterialAssets(JzWorld &world);
    void ProcessShaderAssets(JzWorld &world);

    void UpdateMeshComponentCache(JzMeshAssetComponent &comp, JzMesh *mesh);
    void UpdateMaterialComponentCache(JzMaterialAssetComponent &comp, JzMaterial *material);
    void UpdateShaderComponentCache(JzShaderAssetComponent &comp, JzShaderAsset *shader);

    void UpdateEntityAssetTags(JzWorld &world, JzEntity entity);

    // ==================== Member Variables ====================

    std::unique_ptr<JzAssetManager> m_assetManager;
};

} // namespace JzRE

#include "JzRE/Runtime/Function/ECS/JzAssetSystem.inl" // IWYU pragma: keep
