/**
 * @file    JzLRUCache.h
 * @brief   LRU cache manager for asset memory management
 * @author  Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Resource/JzAssetId.h"

namespace JzRE {

/**
 * @brief LRU cache entry for tracking asset access
 */
struct JzLRUEntry {
    JzAssetId id;             ///< Asset identifier
    Size      memorySize;     ///< Memory usage in bytes
    U64       lastAccessTime; ///< Last access timestamp (ms)

    Bool operator<(const JzLRUEntry &other) const
    {
        return lastAccessTime < other.lastAccessTime;
    }
};

/**
 * @brief LRU cache manager for asset memory budget enforcement
 *
 * Tracks asset access times and memory usage to support eviction
 * of least recently used assets when memory budget is exceeded.
 *
 * Thread-safe for concurrent access.
 */
class JzLRUCacheManager {
public:
    /**
     * @brief Construct with maximum memory budget
     * @param maxMemoryMB Maximum memory budget in megabytes
     */
    explicit JzLRUCacheManager(Size maxMemoryMB);

    /**
     * @brief Destructor
     */
    ~JzLRUCacheManager() = default;

    // Non-copyable
    JzLRUCacheManager(const JzLRUCacheManager &)            = delete;
    JzLRUCacheManager &operator=(const JzLRUCacheManager &) = delete;

    /**
     * @brief Record an asset access (update timestamp and memory)
     *
     * @param id Asset identifier
     * @param memorySize Memory size in bytes
     */
    void RecordAccess(JzAssetId id, Size memorySize);

    /**
     * @brief Update memory size for an existing entry
     *
     * @param id Asset identifier
     * @param memorySize New memory size in bytes
     */
    void UpdateMemorySize(JzAssetId id, Size memorySize);

    /**
     * @brief Remove an asset from tracking
     *
     * @param id Asset identifier to remove
     */
    void Remove(JzAssetId id);

    /**
     * @brief Check if an asset is being tracked
     */
    [[nodiscard]] Bool Contains(JzAssetId id) const;

    /**
     * @brief Get eviction candidates sorted by LRU order
     *
     * Returns assets that should be evicted to reach the target memory.
     * Assets in the exclude set are never returned.
     *
     * @param targetMemoryMB Target memory to reach (in MB)
     * @param excludeIds Set of asset IDs that should not be evicted
     * @return Vector of asset IDs to evict, in LRU order (oldest first)
     */
    [[nodiscard]] std::vector<JzAssetId> GetEvictionCandidates(
        Size                                                  targetMemoryMB,
        const std::unordered_set<JzAssetId, JzAssetId::Hash> &excludeIds = {}) const;

    /**
     * @brief Get assets that exceed memory budget
     *
     * @param excludeIds Set of asset IDs that should not be evicted
     * @return Vector of asset IDs to evict to stay within budget
     */
    [[nodiscard]] std::vector<JzAssetId> GetOverBudgetEvictions(
        const std::unordered_set<JzAssetId, JzAssetId::Hash> &excludeIds = {}) const;

    // ==================== Statistics ====================

    /**
     * @brief Get current total memory usage in bytes
     */
    [[nodiscard]] Size GetCurrentMemoryUsage() const;

    /**
     * @brief Get current total memory usage in megabytes
     */
    [[nodiscard]] Size GetCurrentMemoryUsageMB() const;

    /**
     * @brief Get maximum memory budget in bytes
     */
    [[nodiscard]] Size GetMaxMemory() const;

    /**
     * @brief Get maximum memory budget in megabytes
     */
    [[nodiscard]] Size GetMaxMemoryMB() const;

    /**
     * @brief Get number of tracked assets
     */
    [[nodiscard]] Size GetTrackedCount() const;

    /**
     * @brief Check if memory usage exceeds budget
     */
    [[nodiscard]] Bool IsOverBudget() const;

    /**
     * @brief Get memory usage ratio (0.0 - 1.0+)
     */
    [[nodiscard]] F32 GetMemoryUsageRatio() const;

    // ==================== Configuration ====================

    /**
     * @brief Set maximum memory budget
     * @param maxMemoryMB New budget in megabytes
     */
    void SetMaxMemory(Size maxMemoryMB);

    /**
     * @brief Clear all tracked entries
     */
    void Clear();

private:
    /**
     * @brief Get current timestamp in milliseconds
     */
    static U64 GetCurrentTimestamp();

    Size m_maxMemoryBytes;     ///< Maximum memory budget
    Size m_currentMemoryBytes; ///< Current tracked memory

    std::unordered_map<JzAssetId, JzLRUEntry, JzAssetId::Hash> m_entries;
    mutable std::mutex                                         m_mutex;
};

} // namespace JzRE
