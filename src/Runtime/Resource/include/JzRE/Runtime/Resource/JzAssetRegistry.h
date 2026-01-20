/**
 * @file    JzAssetRegistry.h
 * @brief   Type-independent asset registry with slot-based allocation
 * @author  Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <atomic>
#include <memory>
#include <queue>
#include <shared_mutex>
#include <unordered_map>
#include <vector>

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Resource/JzAssetHandle.h"

namespace JzRE {

/**
 * @brief Asset loading state enumeration
 */
enum class JzEAssetLoadState : U8 {
    NotLoaded, ///< Asset has not been loaded yet
    Loading,   ///< Asset is currently being loaded (async)
    Loaded,    ///< Asset is fully loaded and ready to use
    Failed,    ///< Asset failed to load
    Unloading  ///< Asset is being unloaded
};

/**
 * @brief Convert load state to string for debugging
 */
inline const char *ToString(JzEAssetLoadState state)
{
    switch (state) {
        case JzEAssetLoadState::NotLoaded: return "NotLoaded";
        case JzEAssetLoadState::Loading: return "Loading";
        case JzEAssetLoadState::Loaded: return "Loaded";
        case JzEAssetLoadState::Failed: return "Failed";
        case JzEAssetLoadState::Unloading: return "Unloading";
        default: return "Unknown";
    }
}

/**
 * @brief Asset slot - stores asset data and metadata
 *
 * @tparam T The asset type
 */
template <typename T>
struct JzAssetSlot {
    std::shared_ptr<T> asset;          ///< The actual asset data
    String             path;           ///< Asset path for lookup
    U32                generation = 0; ///< Generation counter
    JzEAssetLoadState  loadState  = JzEAssetLoadState::NotLoaded;
    std::atomic<U32>   refCount{0};        ///< Reference count
    U64                lastAccessTime = 0; ///< Last access timestamp (ms)
    Size               memorySize     = 0; ///< Estimated memory usage (bytes)
    String             errorMessage;       ///< Error message if load failed

    // Default constructor
    JzAssetSlot() = default;

    // Move constructor (std::atomic is not copyable, so we need explicit move)
    JzAssetSlot(JzAssetSlot &&other) noexcept :
        asset(std::move(other.asset)),
        path(std::move(other.path)),
        generation(other.generation),
        loadState(other.loadState),
        refCount(other.refCount.load()),
        lastAccessTime(other.lastAccessTime),
        memorySize(other.memorySize),
        errorMessage(std::move(other.errorMessage))
    { }

    // Move assignment operator
    JzAssetSlot &operator=(JzAssetSlot &&other) noexcept
    {
        if (this != &other) {
            asset      = std::move(other.asset);
            path       = std::move(other.path);
            generation = other.generation;
            loadState  = other.loadState;
            refCount.store(other.refCount.load());
            lastAccessTime = other.lastAccessTime;
            memorySize     = other.memorySize;
            errorMessage   = std::move(other.errorMessage);
        }
        return *this;
    }

    // Delete copy operations (std::atomic is not copyable)
    JzAssetSlot(const JzAssetSlot &)            = delete;
    JzAssetSlot &operator=(const JzAssetSlot &) = delete;
};

/**
 * @brief Type-independent asset registry
 *
 * Features:
 * - Slot-based allocation for fast lookup (O(1) by handle)
 * - Generation mechanism to prevent dangling references
 * - Thread-safe operations
 * - Path-to-handle mapping for cache lookup
 *
 * @tparam T The asset type managed by this registry
 *
 * @note Each asset type should have its own registry instance.
 *       The JzAssetManager manages multiple registries.
 */
template <typename T>
class JzAssetRegistry {
public:
    /**
     * @brief Construct with initial capacity
     * @param initialCapacity Initial number of slots to allocate
     */
    explicit JzAssetRegistry(Size initialCapacity = 1024);

    /**
     * @brief Destructor
     */
    ~JzAssetRegistry();

    // Non-copyable
    JzAssetRegistry(const JzAssetRegistry &)            = delete;
    JzAssetRegistry &operator=(const JzAssetRegistry &) = delete;

    // Move-only
    JzAssetRegistry(JzAssetRegistry &&) noexcept            = default;
    JzAssetRegistry &operator=(JzAssetRegistry &&) noexcept = default;

    // ==================== Asset Operations ====================

    /**
     * @brief Allocate a new asset slot
     *
     * @param path Asset path for identification
     * @return Handle to the new slot, or invalid handle if allocation failed
     *
     * @note This only allocates the slot. The asset data must be set separately.
     */
    JzAssetHandle<T> Allocate(const String &path);

    /**
     * @brief Free an asset slot
     *
     * @param handle Handle to free
     *
     * @note The generation is incremented, invalidating all existing handles to this slot.
     */
    void Free(JzAssetHandle<T> handle);

    /**
     * @brief Check if a handle is valid (including generation check)
     *
     * @param handle Handle to validate
     * @return true if the handle points to a valid, active slot
     */
    [[nodiscard]] Bool IsValid(JzAssetHandle<T> handle) const;

    // ==================== Data Access ====================

    /**
     * @brief Get raw pointer to asset data
     *
     * @param handle Handle to the asset
     * @return Pointer to asset data, or nullptr if handle is invalid
     *
     * @note Updates last access time for LRU tracking
     */
    T *Get(JzAssetHandle<T> handle);

    /**
     * @brief Get const pointer to asset data
     */
    const T *Get(JzAssetHandle<T> handle) const;

    /**
     * @brief Get shared_ptr to asset (for compatibility with existing code)
     *
     * @param handle Handle to the asset
     * @return shared_ptr to asset, or nullptr if handle is invalid
     */
    std::shared_ptr<T> GetShared(JzAssetHandle<T> handle);

    /**
     * @brief Set the asset data for a slot
     *
     * @param handle Handle to the slot
     * @param asset Asset data to store
     */
    void Set(JzAssetHandle<T> handle, std::shared_ptr<T> asset);

    // ==================== Path Lookup ====================

    /**
     * @brief Find handle by path
     *
     * @param path Asset path to search for
     * @return Handle if found, invalid handle otherwise
     */
    [[nodiscard]] JzAssetHandle<T> FindByPath(const String &path) const;

    /**
     * @brief Get the path for a handle
     *
     * @param handle Handle to query
     * @return Path string, empty if handle is invalid
     */
    [[nodiscard]] String GetPath(JzAssetHandle<T> handle) const;

    // ==================== State Management ====================

    /**
     * @brief Get the load state of an asset
     */
    [[nodiscard]] JzEAssetLoadState GetLoadState(JzAssetHandle<T> handle) const;

    /**
     * @brief Set the load state of an asset
     */
    void SetLoadState(JzAssetHandle<T> handle, JzEAssetLoadState state);

    /**
     * @brief Set error message for failed loads
     */
    void SetError(JzAssetHandle<T> handle, const String &message);

    /**
     * @brief Get error message
     */
    [[nodiscard]] String GetError(JzAssetHandle<T> handle) const;

    // ==================== Reference Counting ====================

    /**
     * @brief Increment reference count
     */
    void AddRef(JzAssetHandle<T> handle);

    /**
     * @brief Decrement reference count
     */
    void Release(JzAssetHandle<T> handle);

    /**
     * @brief Get current reference count
     */
    [[nodiscard]] U32 GetRefCount(JzAssetHandle<T> handle) const;

    // ==================== Memory Management ====================

    /**
     * @brief Set the memory size for an asset
     */
    void SetMemorySize(JzAssetHandle<T> handle, Size size);

    /**
     * @brief Get the memory size for an asset
     */
    [[nodiscard]] Size GetMemorySize(JzAssetHandle<T> handle) const;

    /**
     * @brief Get the last access time for an asset
     */
    [[nodiscard]] U64 GetLastAccessTime(JzAssetHandle<T> handle) const;

    // ==================== Statistics ====================

    /**
     * @brief Get total number of allocated slots (including free slots)
     */
    [[nodiscard]] Size GetCapacity() const;

    /**
     * @brief Get number of active (in-use) slots
     */
    [[nodiscard]] Size GetActiveCount() const;

    /**
     * @brief Get number of loaded assets
     */
    [[nodiscard]] Size GetLoadedCount() const;

    /**
     * @brief Get total memory usage of all loaded assets
     */
    [[nodiscard]] Size GetTotalMemoryUsage() const;

    /**
     * @brief Get all active handles (for iteration)
     *
     * @return Vector of all valid handles
     */
    [[nodiscard]] std::vector<JzAssetHandle<T>> GetAllHandles() const;

private:
    /**
     * @brief Get current timestamp in milliseconds
     */
    static U64 GetCurrentTimestamp();

    /**
     * @brief Grow the slot array if needed
     */
    void GrowIfNeeded();

    mutable std::shared_mutex                    m_mutex;           ///< Read-write lock
    std::vector<JzAssetSlot<T>>                  m_slots;           ///< Slot storage
    std::queue<U32>                              m_freeIndices;     ///< Free slot indices
    std::unordered_map<String, JzAssetHandle<T>> m_pathToHandle;    ///< Path lookup cache
    Size                                         m_activeCount = 0; ///< Number of active slots
};

} // namespace JzRE

#include "JzRE/Runtime/Resource/JzAssetRegistry.inl" // IWYU pragma: keep
