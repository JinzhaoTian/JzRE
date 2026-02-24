/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Resource/JzLRUCache.h"

#include <algorithm>
#include <chrono>

namespace JzRE {

JzLRUCacheManager::JzLRUCacheManager(Size maxMemoryMB) :
    m_maxMemoryBytes(maxMemoryMB * 1024 * 1024),
    m_currentMemoryBytes(0)
{
}

void JzLRUCacheManager::RecordAccess(JzAssetId id, Size memorySize)
{
    std::lock_guard lock(m_mutex);

    auto it = m_entries.find(id);
    if (it != m_entries.end()) {
        // Update existing entry
        m_currentMemoryBytes      -= it->second.memorySize;
        it->second.memorySize      = memorySize;
        it->second.lastAccessTime  = GetCurrentTimestamp();
        m_currentMemoryBytes      += memorySize;
    } else {
        // Add new entry
        JzLRUEntry entry;
        entry.id             = id;
        entry.memorySize     = memorySize;
        entry.lastAccessTime = GetCurrentTimestamp();

        m_entries[id]         = entry;
        m_currentMemoryBytes += memorySize;
    }
}

void JzLRUCacheManager::UpdateMemorySize(JzAssetId id, Size memorySize)
{
    std::lock_guard lock(m_mutex);

    auto it = m_entries.find(id);
    if (it != m_entries.end()) {
        m_currentMemoryBytes  -= it->second.memorySize;
        it->second.memorySize  = memorySize;
        m_currentMemoryBytes  += memorySize;
    }
}

void JzLRUCacheManager::Remove(JzAssetId id)
{
    std::lock_guard lock(m_mutex);

    auto it = m_entries.find(id);
    if (it != m_entries.end()) {
        m_currentMemoryBytes -= it->second.memorySize;
        m_entries.erase(it);
    }
}

Bool JzLRUCacheManager::Contains(JzAssetId id) const
{
    std::lock_guard lock(m_mutex);
    return m_entries.find(id) != m_entries.end();
}

std::vector<JzAssetId> JzLRUCacheManager::GetEvictionCandidates(
    Size                                                  targetMemoryMB,
    const std::unordered_set<JzAssetId, JzAssetId::Hash> &excludeIds) const
{
    std::lock_guard lock(m_mutex);

    Size targetMemoryBytes = targetMemoryMB * 1024 * 1024;

    if (m_currentMemoryBytes <= targetMemoryBytes) {
        return {}; // No eviction needed
    }

    // Collect all entries and sort by last access time (oldest first)
    std::vector<JzLRUEntry> sortedEntries;
    sortedEntries.reserve(m_entries.size());

    for (const auto &[id, entry] : m_entries) {
        if (excludeIds.find(id) == excludeIds.end()) {
            sortedEntries.push_back(entry);
        }
    }

    std::sort(sortedEntries.begin(), sortedEntries.end());

    // Collect candidates until we reach target
    std::vector<JzAssetId> candidates;
    Size                   memoryToFree = m_currentMemoryBytes - targetMemoryBytes;
    Size                   memoryFreed  = 0;

    for (const auto &entry : sortedEntries) {
        if (memoryFreed >= memoryToFree) {
            break;
        }
        candidates.push_back(entry.id);
        memoryFreed += entry.memorySize;
    }

    return candidates;
}

std::vector<JzAssetId> JzLRUCacheManager::GetOverBudgetEvictions(
    const std::unordered_set<JzAssetId, JzAssetId::Hash> &excludeIds) const
{
    return GetEvictionCandidates(m_maxMemoryBytes / (1024 * 1024), excludeIds);
}

Size JzLRUCacheManager::GetCurrentMemoryUsage() const
{
    std::lock_guard lock(m_mutex);
    return m_currentMemoryBytes;
}

Size JzLRUCacheManager::GetCurrentMemoryUsageMB() const
{
    std::lock_guard lock(m_mutex);
    return m_currentMemoryBytes / (1024 * 1024);
}

Size JzLRUCacheManager::GetMaxMemory() const
{
    std::lock_guard lock(m_mutex);
    return m_maxMemoryBytes;
}

Size JzLRUCacheManager::GetMaxMemoryMB() const
{
    std::lock_guard lock(m_mutex);
    return m_maxMemoryBytes / (1024 * 1024);
}

Size JzLRUCacheManager::GetTrackedCount() const
{
    std::lock_guard lock(m_mutex);
    return m_entries.size();
}

Bool JzLRUCacheManager::IsOverBudget() const
{
    std::lock_guard lock(m_mutex);
    return m_currentMemoryBytes > m_maxMemoryBytes;
}

F32 JzLRUCacheManager::GetMemoryUsageRatio() const
{
    std::lock_guard lock(m_mutex);
    if (m_maxMemoryBytes == 0) {
        return 0.0f;
    }
    return static_cast<F32>(m_currentMemoryBytes) / static_cast<F32>(m_maxMemoryBytes);
}

void JzLRUCacheManager::SetMaxMemory(Size maxMemoryMB)
{
    std::lock_guard lock(m_mutex);
    m_maxMemoryBytes = maxMemoryMB * 1024 * 1024;
}

void JzLRUCacheManager::Clear()
{
    std::lock_guard lock(m_mutex);
    m_entries.clear();
    m_currentMemoryBytes = 0;
}

U64 JzLRUCacheManager::GetCurrentTimestamp()
{
    using namespace std::chrono;
    return static_cast<U64>(duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count());
}

} // namespace JzRE
