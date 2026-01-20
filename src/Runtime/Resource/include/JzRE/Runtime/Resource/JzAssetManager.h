/**
 * @file    JzAssetManager.h
 * @brief   Modern asset manager with async loading and LRU caching
 * @author  Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzThreadPool.h"
#include "JzRE/Runtime/Resource/JzAssetHandle.h"
#include "JzRE/Runtime/Resource/JzAssetRegistry.h"
#include "JzRE/Runtime/Resource/JzLRUCache.h"
#include "JzRE/Runtime/Resource/JzResourceFactory.h"

namespace JzRE {

// Forward declarations
class JzMesh;
class JzTexture;
class JzMaterial;
class JzModel;
class JzShader;
class JzResource;

/**
 * @brief Asset load callback type
 */
template <typename T>
using JzAssetLoadCallback = std::function<void(JzAssetHandle<T>, Bool success)>;

/**
 * @brief Asset manager configuration
 */
struct JzAssetManagerConfig {
    Size maxCacheMemoryMB     = 512;   ///< Maximum memory budget (MB)
    Size asyncWorkerCount     = 2;     ///< Number of async loading threads
    F32  lruEvictionThreshold = 0.8f;  ///< Eviction threshold (0.0-1.0)
    Bool enableHotReload      = false; ///< Enable hot reload (editor mode)
};

/**
 * @brief Async load request
 */
struct JzAssetLoadRequest {
    JzAssetId       id;
    String          path;
    std::type_index typeIndex = typeid(void);
    I32             priority  = 0;

    // Priority queue comparison (higher priority first)
    Bool operator<(const JzAssetLoadRequest &other) const
    {
        return priority < other.priority;
    }
};

/**
 * @brief Async load result
 */
struct JzAssetLoadResult {
    JzAssetId       id;
    std::type_index typeIndex = typeid(void);
    Bool            success   = false;
    String          errorMessage;
};

/**
 * @brief Modern asset manager with ECS-friendly design
 *
 * Design goals:
 * 1. Type-safe handle system with generation validation
 * 2. Async loading support
 * 3. LRU cache with memory budget
 * 4. ECS-friendly interface
 *
 * @example
 * @code
 * JzAssetManager assetManager;
 * assetManager.Initialize();
 *
 * // Sync load
 * auto meshHandle = assetManager.LoadSync<JzMesh>("cube.obj");
 *
 * // Async load with callback
 * assetManager.LoadAsync<JzTexture>("diffuse.png", [](auto handle, bool success) {
 *     if (success) {
 *         // Texture loaded
 *     }
 * });
 *
 * // Access loaded asset
 * JzMesh* mesh = assetManager.Get(meshHandle);
 * @endcode
 */
class JzAssetManager {
public:
    /**
     * @brief Construct with configuration
     */
    explicit JzAssetManager(const JzAssetManagerConfig &config = {});

    /**
     * @brief Destructor
     */
    ~JzAssetManager();

    // Non-copyable
    JzAssetManager(const JzAssetManager &)            = delete;
    JzAssetManager &operator=(const JzAssetManager &) = delete;

    // ==================== Lifecycle ====================

    /**
     * @brief Initialize the asset manager
     *
     * Creates registries for each asset type and starts worker threads.
     */
    void Initialize();

    /**
     * @brief Shutdown the asset manager
     *
     * Stops worker threads and releases all assets.
     */
    void Shutdown();

    /**
     * @brief Check if initialized
     */
    [[nodiscard]] Bool IsInitialized() const
    {
        return m_initialized;
    }

    // ==================== Synchronous Loading ====================

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
     *
     * @tparam T Asset type
     * @param path Asset path
     * @return Handle to the asset
     */
    template <typename T>
    JzAssetHandle<T> GetOrLoad(const String &path);

    // ==================== Asynchronous Loading ====================

    /**
     * @brief Load an asset asynchronously
     *
     * @tparam T Asset type
     * @param path Asset path
     * @param callback Optional callback when load completes
     * @param priority Load priority (higher = more urgent)
     * @return Handle to the asset (may not be loaded yet)
     */
    template <typename T>
    JzAssetHandle<T> LoadAsync(const String          &path,
                               JzAssetLoadCallback<T> callback = nullptr,
                               I32                    priority = 0);

    /**
     * @brief Cancel a pending async load request
     *
     * @param id Asset ID to cancel
     */
    void CancelLoad(JzAssetId id);

    // ==================== Asset Access ====================

    /**
     * @brief Get raw pointer to asset data
     *
     * @tparam T Asset type
     * @param handle Handle to the asset
     * @return Pointer to asset, or nullptr if not loaded/invalid
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

    /**
     * @brief Force unload an asset (ignoring reference count)
     */
    template <typename T>
    void ForceUnload(JzAssetHandle<T> handle);

    // ==================== Cache Management ====================

    /**
     * @brief Update - process async results and LRU eviction
     *
     * Call this once per frame.
     */
    void Update();

    /**
     * @brief Evict assets to reach target memory
     *
     * @param targetMemoryMB Target memory in MB
     */
    void EvictToTarget(Size targetMemoryMB);

    /**
     * @brief Unload all assets with zero reference count
     */
    void UnloadUnused();

    // ==================== Search Paths ====================

    /**
     * @brief Add a search path for asset files
     */
    void AddSearchPath(const String &path);

    /**
     * @brief Find full path for a relative asset path
     */
    [[nodiscard]] String FindFullPath(const String &relativePath) const;

    // ==================== Factory Registration ====================

    /**
     * @brief Register a factory for a resource type
     *
     * This is for compatibility with existing JzResourceFactory implementations.
     */
    template <typename T>
    void RegisterFactory(std::unique_ptr<JzResourceFactory> factory);

    // ==================== Registry Access ====================

    /**
     * @brief Get the registry for a specific type
     */
    template <typename T>
    JzAssetRegistry<T> &GetRegistry();

    /**
     * @brief Get const registry for a specific type
     */
    template <typename T>
    const JzAssetRegistry<T> &GetRegistry() const;

    // ==================== Statistics ====================

    /**
     * @brief Get total memory usage across all registries
     */
    [[nodiscard]] Size GetTotalMemoryUsage() const;

    /**
     * @brief Get number of pending async load requests
     */
    [[nodiscard]] Size GetPendingLoadCount() const;

    /**
     * @brief Get the configuration
     */
    [[nodiscard]] const JzAssetManagerConfig &GetConfig() const
    {
        return m_config;
    }

private:
    // ==================== Internal Types ====================

    /**
     * @brief Type-erased registry storage with custom deleter
     */
    struct RegistryEntry {
        std::unique_ptr<void, void (*)(void *)> registry  = {nullptr, [](void *) { }};
        std::type_index                         typeIndex = typeid(void);
    };

    /**
     * @brief Pending callback storage
     */
    struct PendingCallback {
        std::type_index           typeIndex = typeid(void);
        std::function<void(Bool)> callback;
    };

    // ==================== Internal Methods ====================

    /**
     * @brief Initialize registries for all supported asset types
     */
    void InitializeRegistries();

    /**
     * @brief Process the async load queue
     */
    void ProcessAsyncQueue();

    /**
     * @brief Process completed load results
     */
    void ProcessResults();

    /**
     * @brief Perform the actual asset load
     */
    template <typename T>
    void DoLoadAsset(JzAssetHandle<T> handle, const String &path);

    /**
     * @brief Get or create registry for type
     */
    template <typename T>
    JzAssetRegistry<T> *GetOrCreateRegistry();

    // ==================== Member Variables ====================

    JzAssetManagerConfig m_config;
    Bool                 m_initialized = false;

    // Registries (one per asset type)
    std::unordered_map<std::type_index, RegistryEntry> m_registries;
    mutable std::shared_mutex                          m_registryMutex;

    // Factories (compatible with existing system)
    std::unordered_map<std::type_index, std::unique_ptr<JzResourceFactory>> m_factories;
    std::mutex                                                              m_factoryMutex;

    // Async loading
    std::unique_ptr<JzThreadPool>           m_loadThreadPool;
    std::priority_queue<JzAssetLoadRequest> m_loadQueue;
    mutable std::mutex                      m_loadQueueMutex;

    // Load results (main thread processes these)
    std::queue<JzAssetLoadResult> m_resultQueue;
    std::mutex                    m_resultQueueMutex;

    // Callbacks
    std::unordered_map<JzAssetId, PendingCallback, JzAssetId::Hash> m_callbacks;
    std::mutex                                                      m_callbackMutex;

    // LRU cache manager
    std::unique_ptr<JzLRUCacheManager> m_lruCache;

    // Search paths
    std::vector<String> m_searchPaths;
    mutable std::mutex  m_searchPathMutex;
};

} // namespace JzRE

#include "JzRE/Runtime/Resource/JzAssetManager.inl" // IWYU pragma: keep
