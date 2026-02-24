/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>
#include <unordered_set>
#include <vector>

#include "JzRE/Runtime/Function/ECS/JzEntity.h"
#include "JzRE/Runtime/Function/ECS/JzSystem.h"
#include "JzRE/Runtime/Resource/JzAssetHandle.h"
#include "JzRE/Runtime/Resource/JzAssetManager.h"
#include "JzRE/Runtime/Resource/JzMesh.h"
#include "JzRE/Runtime/Resource/JzMaterial.h"
#include "JzRE/Runtime/Resource/JzTexture.h"
#include "JzRE/Runtime/Resource/JzModel.h"
#include "JzRE/Runtime/Resource/JzShader.h"

namespace JzRE {

// Forward declarations
class JzWorld;
class JzModel;
class JzMesh;
class JzMaterial;
class JzShader;
class JzResourceFactory;
struct JzMeshAssetComponent;
struct JzMaterialAssetComponent;
struct JzShaderComponent;

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
     * Registers the asset manager with JzServiceContainer and world context.
     * Must be called before using any asset operations.
     *
     * @param world The ECS world (for context registration)
     * @param config Asset manager configuration
     */
    void Initialize(JzWorld &world, const JzAssetManagerConfig &config);

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
    void AttachShader(JzWorld &world, JzEntity entity, JzShaderHandle handle);

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

    // ==================== Hot Reload Configuration ====================

    /**
     * @brief Enable or disable hot reload functionality
     *
     * Hot reload monitors asset files for changes and automatically
     * reloads them during runtime. This is intended for authoring/development
     * mode and should be disabled in release builds.
     *
     * @param enabled True to enable hot reload
     */
    void SetHotReloadEnabled(Bool enabled);

    /**
     * @brief Check if hot reload is enabled
     */
    [[nodiscard]] Bool IsHotReloadEnabled() const;

    /**
     * @brief Set the interval between hot reload checks
     *
     * @param seconds Time in seconds between file modification checks (default: 1.0)
     */
    void SetHotReloadCheckInterval(F32 seconds);

    /**
     * @brief Get the current hot reload check interval
     */
    [[nodiscard]] F32 GetHotReloadCheckInterval() const;

    /**
     * @brief Force an immediate hot reload check on all tracked assets
     *
     * Useful for triggering reload from host tooling or debug controls.
     */
    void ForceHotReloadCheck();

    /**
     * @brief Manually reload a specific shader asset
     *
     * @param handle Handle to the shader to reload
     * @return True if reload succeeded
     */
    Bool ReloadShader(JzShaderHandle handle);

    // ==================== Hot Reload Statistics ====================

    /**
     * @brief Get total number of assets reloaded since initialization
     */
    [[nodiscard]] Size GetHotReloadCount() const;

    /**
     * @brief Get shader reload count specifically
     */
    [[nodiscard]] Size GetShaderReloadCount() const;

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
    void ProcessShaders(JzWorld &world);

    void UpdateMeshComponentCache(JzMeshAssetComponent &comp, JzMesh *mesh);
    void UpdateMaterialComponentCache(JzMaterialAssetComponent &comp, JzMaterial *material);
    void UpdateShaderComponentCache(JzShaderComponent &comp, JzShader *shader);

    void UpdateEntityAssetTags(JzWorld &world, JzEntity entity);

    // ==================== Hot Reload Internal ====================

    void CheckForHotReloadUpdates(JzWorld &world);
    void CheckShaderHotReload(JzWorld &world);
    void NotifyShaderReloaded(JzShaderHandle handle, JzWorld &world);

    std::unordered_set<JzShaderHandle, JzAssetHandle<JzShader>::Hash>
    CollectUsedShaders(JzWorld &world);

    // ==================== Member Variables ====================

    std::unique_ptr<JzAssetManager> m_assetManager;

    // Hot Reload State
    Bool m_hotReloadEnabled       = false;
    F32  m_hotReloadCheckInterval = 1.0f;
    F32  m_timeSinceLastCheck     = 0.0f;
    Bool m_forceCheckNextFrame    = false;
    Size m_totalReloadCount       = 0;
    Size m_shaderReloadCount      = 0;
};

} // namespace JzRE

#include "JzRE/Runtime/Function/ECS/JzAssetSystem.inl" // IWYU pragma: keep
