/**
 * @file    JzAssetRegistry.inl
 * @brief   Template implementation for JzAssetRegistry
 * @author  Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

namespace JzRE {

template<typename T>
JzAssetRegistry<T>::JzAssetRegistry(Size initialCapacity)
{
    m_slots.resize(initialCapacity);

    // Initialize free list (skip index 0 to avoid confusion with invalid)
    for (U32 i = 1; i < static_cast<U32>(initialCapacity); ++i) {
        m_freeIndices.push(i);
    }
}

template<typename T>
JzAssetRegistry<T>::~JzAssetRegistry()
{
    // Clear all assets
    std::unique_lock lock(m_mutex);
    for (auto& slot : m_slots) {
        slot.asset.reset();
    }
    m_pathToHandle.clear();
}

template<typename T>
JzAssetHandle<T> JzAssetRegistry<T>::Allocate(const String& path)
{
    std::unique_lock lock(m_mutex);

    // Check if already allocated for this path
    auto it = m_pathToHandle.find(path);
    if (it != m_pathToHandle.end()) {
        // Validate the existing handle
        const auto& existingHandle = it->second;
        if (existingHandle.GetId().index < m_slots.size()) {
            const auto& slot = m_slots[existingHandle.GetId().index];
            if (slot.generation == existingHandle.GetId().generation) {
                return existingHandle; // Return existing handle
            }
        }
        // Handle is stale, remove it
        m_pathToHandle.erase(it);
    }

    // Grow if needed
    GrowIfNeeded();

    if (m_freeIndices.empty()) {
        return JzAssetHandle<T>::Invalid(); // Should not happen after grow
    }

    // Get a free slot
    U32 index = m_freeIndices.front();
    m_freeIndices.pop();

    // Initialize slot
    auto& slot       = m_slots[index];
    slot.generation  = slot.generation + 1; // Increment generation
    slot.path        = path;
    slot.loadState   = JzEAssetLoadState::NotLoaded;
    slot.refCount    = 0;
    slot.lastAccessTime = GetCurrentTimestamp();
    slot.memorySize  = 0;
    slot.errorMessage.clear();
    slot.asset.reset();

    // Create handle
    JzAssetId   id{index, slot.generation};
    JzAssetHandle<T> handle(id);

    // Register path mapping
    m_pathToHandle[path] = handle;
    ++m_activeCount;

    return handle;
}

template<typename T>
void JzAssetRegistry<T>::Free(JzAssetHandle<T> handle)
{
    if (!handle.IsValid()) {
        return;
    }

    std::unique_lock lock(m_mutex);

    const auto id = handle.GetId();
    if (id.index >= m_slots.size()) {
        return;
    }

    auto& slot = m_slots[id.index];
    if (slot.generation != id.generation) {
        return; // Handle is stale
    }

    // Remove from path mapping
    m_pathToHandle.erase(slot.path);

    // Clear slot data (generation is NOT incremented here - it's incremented on next Allocate)
    slot.asset.reset();
    slot.path.clear();
    slot.loadState = JzEAssetLoadState::NotLoaded;
    slot.refCount  = 0;
    slot.memorySize = 0;
    slot.errorMessage.clear();

    // Return slot to free list
    m_freeIndices.push(id.index);
    --m_activeCount;
}

template<typename T>
Bool JzAssetRegistry<T>::IsValid(JzAssetHandle<T> handle) const
{
    if (!handle.IsValid()) {
        return false;
    }

    std::shared_lock lock(m_mutex);

    const auto id = handle.GetId();
    if (id.index >= m_slots.size()) {
        return false;
    }

    return m_slots[id.index].generation == id.generation;
}

template<typename T>
T* JzAssetRegistry<T>::Get(JzAssetHandle<T> handle)
{
    if (!handle.IsValid()) {
        return nullptr;
    }

    std::shared_lock lock(m_mutex);

    const auto id = handle.GetId();
    if (id.index >= m_slots.size()) {
        return nullptr;
    }

    auto& slot = m_slots[id.index];
    if (slot.generation != id.generation) {
        return nullptr;
    }

    // Update access time (for LRU)
    slot.lastAccessTime = GetCurrentTimestamp();

    return slot.asset.get();
}

template<typename T>
const T* JzAssetRegistry<T>::Get(JzAssetHandle<T> handle) const
{
    if (!handle.IsValid()) {
        return nullptr;
    }

    std::shared_lock lock(m_mutex);

    const auto id = handle.GetId();
    if (id.index >= m_slots.size()) {
        return nullptr;
    }

    const auto& slot = m_slots[id.index];
    if (slot.generation != id.generation) {
        return nullptr;
    }

    return slot.asset.get();
}

template<typename T>
std::shared_ptr<T> JzAssetRegistry<T>::GetShared(JzAssetHandle<T> handle)
{
    if (!handle.IsValid()) {
        return nullptr;
    }

    std::shared_lock lock(m_mutex);

    const auto id = handle.GetId();
    if (id.index >= m_slots.size()) {
        return nullptr;
    }

    auto& slot = m_slots[id.index];
    if (slot.generation != id.generation) {
        return nullptr;
    }

    // Update access time (for LRU)
    slot.lastAccessTime = GetCurrentTimestamp();

    return slot.asset;
}

template<typename T>
void JzAssetRegistry<T>::Set(JzAssetHandle<T> handle, std::shared_ptr<T> asset)
{
    if (!handle.IsValid()) {
        return;
    }

    std::unique_lock lock(m_mutex);

    const auto id = handle.GetId();
    if (id.index >= m_slots.size()) {
        return;
    }

    auto& slot = m_slots[id.index];
    if (slot.generation != id.generation) {
        return;
    }

    slot.asset          = std::move(asset);
    slot.lastAccessTime = GetCurrentTimestamp();
}

template<typename T>
JzAssetHandle<T> JzAssetRegistry<T>::FindByPath(const String& path) const
{
    std::shared_lock lock(m_mutex);

    auto it = m_pathToHandle.find(path);
    if (it != m_pathToHandle.end()) {
        // Validate the handle before returning
        const auto& handle = it->second;
        const auto  id     = handle.GetId();
        if (id.index < m_slots.size() && m_slots[id.index].generation == id.generation) {
            return handle;
        }
    }

    return JzAssetHandle<T>::Invalid();
}

template<typename T>
String JzAssetRegistry<T>::GetPath(JzAssetHandle<T> handle) const
{
    if (!handle.IsValid()) {
        return "";
    }

    std::shared_lock lock(m_mutex);

    const auto id = handle.GetId();
    if (id.index >= m_slots.size()) {
        return "";
    }

    const auto& slot = m_slots[id.index];
    if (slot.generation != id.generation) {
        return "";
    }

    return slot.path;
}

template<typename T>
JzEAssetLoadState JzAssetRegistry<T>::GetLoadState(JzAssetHandle<T> handle) const
{
    if (!handle.IsValid()) {
        return JzEAssetLoadState::NotLoaded;
    }

    std::shared_lock lock(m_mutex);

    const auto id = handle.GetId();
    if (id.index >= m_slots.size()) {
        return JzEAssetLoadState::NotLoaded;
    }

    const auto& slot = m_slots[id.index];
    if (slot.generation != id.generation) {
        return JzEAssetLoadState::NotLoaded;
    }

    return slot.loadState;
}

template<typename T>
void JzAssetRegistry<T>::SetLoadState(JzAssetHandle<T> handle, JzEAssetLoadState state)
{
    if (!handle.IsValid()) {
        return;
    }

    std::unique_lock lock(m_mutex);

    const auto id = handle.GetId();
    if (id.index >= m_slots.size()) {
        return;
    }

    auto& slot = m_slots[id.index];
    if (slot.generation != id.generation) {
        return;
    }

    slot.loadState = state;
}

template<typename T>
void JzAssetRegistry<T>::SetError(JzAssetHandle<T> handle, const String& message)
{
    if (!handle.IsValid()) {
        return;
    }

    std::unique_lock lock(m_mutex);

    const auto id = handle.GetId();
    if (id.index >= m_slots.size()) {
        return;
    }

    auto& slot = m_slots[id.index];
    if (slot.generation != id.generation) {
        return;
    }

    slot.errorMessage = message;
    slot.loadState    = JzEAssetLoadState::Failed;
}

template<typename T>
String JzAssetRegistry<T>::GetError(JzAssetHandle<T> handle) const
{
    if (!handle.IsValid()) {
        return "";
    }

    std::shared_lock lock(m_mutex);

    const auto id = handle.GetId();
    if (id.index >= m_slots.size()) {
        return "";
    }

    const auto& slot = m_slots[id.index];
    if (slot.generation != id.generation) {
        return "";
    }

    return slot.errorMessage;
}

template<typename T>
void JzAssetRegistry<T>::AddRef(JzAssetHandle<T> handle)
{
    if (!handle.IsValid()) {
        return;
    }

    std::shared_lock lock(m_mutex);

    const auto id = handle.GetId();
    if (id.index >= m_slots.size()) {
        return;
    }

    auto& slot = m_slots[id.index];
    if (slot.generation != id.generation) {
        return;
    }

    slot.refCount.fetch_add(1, std::memory_order_relaxed);
}

template<typename T>
void JzAssetRegistry<T>::Release(JzAssetHandle<T> handle)
{
    if (!handle.IsValid()) {
        return;
    }

    std::shared_lock lock(m_mutex);

    const auto id = handle.GetId();
    if (id.index >= m_slots.size()) {
        return;
    }

    auto& slot = m_slots[id.index];
    if (slot.generation != id.generation) {
        return;
    }

    U32 prev = slot.refCount.fetch_sub(1, std::memory_order_relaxed);
    if (prev == 0) {
        // Underflow - should not happen, but clamp to 0
        slot.refCount.store(0, std::memory_order_relaxed);
    }
}

template<typename T>
U32 JzAssetRegistry<T>::GetRefCount(JzAssetHandle<T> handle) const
{
    if (!handle.IsValid()) {
        return 0;
    }

    std::shared_lock lock(m_mutex);

    const auto id = handle.GetId();
    if (id.index >= m_slots.size()) {
        return 0;
    }

    const auto& slot = m_slots[id.index];
    if (slot.generation != id.generation) {
        return 0;
    }

    return slot.refCount.load(std::memory_order_relaxed);
}

template<typename T>
void JzAssetRegistry<T>::SetMemorySize(JzAssetHandle<T> handle, Size size)
{
    if (!handle.IsValid()) {
        return;
    }

    std::unique_lock lock(m_mutex);

    const auto id = handle.GetId();
    if (id.index >= m_slots.size()) {
        return;
    }

    auto& slot = m_slots[id.index];
    if (slot.generation != id.generation) {
        return;
    }

    slot.memorySize = size;
}

template<typename T>
Size JzAssetRegistry<T>::GetMemorySize(JzAssetHandle<T> handle) const
{
    if (!handle.IsValid()) {
        return 0;
    }

    std::shared_lock lock(m_mutex);

    const auto id = handle.GetId();
    if (id.index >= m_slots.size()) {
        return 0;
    }

    const auto& slot = m_slots[id.index];
    if (slot.generation != id.generation) {
        return 0;
    }

    return slot.memorySize;
}

template<typename T>
U64 JzAssetRegistry<T>::GetLastAccessTime(JzAssetHandle<T> handle) const
{
    if (!handle.IsValid()) {
        return 0;
    }

    std::shared_lock lock(m_mutex);

    const auto id = handle.GetId();
    if (id.index >= m_slots.size()) {
        return 0;
    }

    const auto& slot = m_slots[id.index];
    if (slot.generation != id.generation) {
        return 0;
    }

    return slot.lastAccessTime;
}

template<typename T>
Size JzAssetRegistry<T>::GetCapacity() const
{
    std::shared_lock lock(m_mutex);
    return m_slots.size();
}

template<typename T>
Size JzAssetRegistry<T>::GetActiveCount() const
{
    std::shared_lock lock(m_mutex);
    return m_activeCount;
}

template<typename T>
Size JzAssetRegistry<T>::GetLoadedCount() const
{
    std::shared_lock lock(m_mutex);

    Size count = 0;
    for (const auto& slot : m_slots) {
        if (slot.loadState == JzEAssetLoadState::Loaded) {
            ++count;
        }
    }
    return count;
}

template<typename T>
Size JzAssetRegistry<T>::GetTotalMemoryUsage() const
{
    std::shared_lock lock(m_mutex);

    Size total = 0;
    for (const auto& slot : m_slots) {
        if (slot.loadState == JzEAssetLoadState::Loaded) {
            total += slot.memorySize;
        }
    }
    return total;
}

template<typename T>
std::vector<JzAssetHandle<T>> JzAssetRegistry<T>::GetAllHandles() const
{
    std::shared_lock lock(m_mutex);

    std::vector<JzAssetHandle<T>> handles;
    handles.reserve(m_activeCount);

    for (const auto& [path, handle] : m_pathToHandle) {
        const auto id = handle.GetId();
        if (id.index < m_slots.size() && m_slots[id.index].generation == id.generation) {
            handles.push_back(handle);
        }
    }

    return handles;
}

template<typename T>
U64 JzAssetRegistry<T>::GetCurrentTimestamp()
{
    using namespace std::chrono;
    return static_cast<U64>(duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count());
}

template<typename T>
void JzAssetRegistry<T>::GrowIfNeeded()
{
    if (!m_freeIndices.empty()) {
        return;
    }

    // Double the capacity
    Size oldSize = m_slots.size();
    Size newSize = oldSize == 0 ? 64 : oldSize * 2;

    m_slots.resize(newSize);

    // Add new indices to free list
    for (U32 i = static_cast<U32>(oldSize); i < static_cast<U32>(newSize); ++i) {
        m_freeIndices.push(i);
    }
}

} // namespace JzRE
