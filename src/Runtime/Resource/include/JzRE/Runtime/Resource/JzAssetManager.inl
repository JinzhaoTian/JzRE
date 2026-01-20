/**
 * @file    JzAssetManager.inl
 * @brief   Template implementation for JzAssetManager
 * @author  Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Resource/JzAssetManager.h"
#include "JzRE/Runtime/Core/JzLogger.h"

namespace JzRE {

template <typename T>
JzAssetHandle<T> JzAssetManager::LoadSync(const String &path)
{
    if (!m_initialized) {
        JzRE_LOG_ERROR("JzAssetManager::LoadSync: Not initialized");
        return JzAssetHandle<T>::Invalid();
    }

    auto *registry = GetOrCreateRegistry<T>();
    if (!registry) {
        JzRE_LOG_ERROR("JzAssetManager::LoadSync: Failed to get registry for type");
        return JzAssetHandle<T>::Invalid();
    }

    // Check if already loaded
    auto existingHandle = registry->FindByPath(path);
    if (existingHandle.IsValid()) {
        auto state = registry->GetLoadState(existingHandle);
        if (state == JzEAssetLoadState::Loaded) {
            return existingHandle;
        }
        if (state == JzEAssetLoadState::Loading) {
            // Wait for async load to complete - for now just return the handle
            // The caller should check IsLoaded()
            return existingHandle;
        }
    }

    // Allocate new slot
    auto handle = registry->Allocate(path);
    if (!handle.IsValid()) {
        JzRE_LOG_ERROR("JzAssetManager::LoadSync: Failed to allocate slot for '{}'", path);
        return JzAssetHandle<T>::Invalid();
    }

    // Set loading state
    registry->SetLoadState(handle, JzEAssetLoadState::Loading);

    // Perform synchronous load
    DoLoadAsset<T>(handle, path);

    return handle;
}

template <typename T>
JzAssetHandle<T> JzAssetManager::GetOrLoad(const String &path)
{
    if (!m_initialized) {
        return JzAssetHandle<T>::Invalid();
    }

    auto *registry = GetOrCreateRegistry<T>();
    if (!registry) {
        return JzAssetHandle<T>::Invalid();
    }

    // Check if already exists
    auto existingHandle = registry->FindByPath(path);
    if (existingHandle.IsValid()) {
        return existingHandle;
    }

    // Load synchronously
    return LoadSync<T>(path);
}

template <typename T>
JzAssetHandle<T> JzAssetManager::LoadAsync(const String          &path,
                                           JzAssetLoadCallback<T> callback,
                                           I32                    priority)
{
    if (!m_initialized) {
        JzRE_LOG_ERROR("JzAssetManager::LoadAsync: Not initialized");
        if (callback) {
            callback(JzAssetHandle<T>::Invalid(), false);
        }
        return JzAssetHandle<T>::Invalid();
    }

    auto *registry = GetOrCreateRegistry<T>();
    if (!registry) {
        if (callback) {
            callback(JzAssetHandle<T>::Invalid(), false);
        }
        return JzAssetHandle<T>::Invalid();
    }

    // Check if already loaded
    auto existingHandle = registry->FindByPath(path);
    if (existingHandle.IsValid()) {
        auto state = registry->GetLoadState(existingHandle);
        if (state == JzEAssetLoadState::Loaded) {
            if (callback) {
                callback(existingHandle, true);
            }
            return existingHandle;
        }
        if (state == JzEAssetLoadState::Loading) {
            // Add callback for existing request
            if (callback) {
                std::lock_guard lock(m_callbackMutex);
                m_callbacks[existingHandle.GetId()] = {
                    std::type_index(typeid(T)),
                    [callback, existingHandle](Bool success) {
                        callback(existingHandle, success);
                    }};
            }
            return existingHandle;
        }
    }

    // Allocate new slot
    auto handle = registry->Allocate(path);
    if (!handle.IsValid()) {
        JzRE_LOG_ERROR("JzAssetManager::LoadAsync: Failed to allocate slot for '{}'", path);
        if (callback) {
            callback(JzAssetHandle<T>::Invalid(), false);
        }
        return JzAssetHandle<T>::Invalid();
    }

    // Set loading state
    registry->SetLoadState(handle, JzEAssetLoadState::Loading);

    // Store callback
    if (callback) {
        std::lock_guard lock(m_callbackMutex);
        m_callbacks[handle.GetId()] = {std::type_index(typeid(T)),
                                       [callback, handle](Bool success) {
                                           callback(handle, success);
                                       }};
    }

    // Queue async load request
    JzAssetLoadRequest request;
    request.id        = handle.GetId();
    request.path      = path;
    request.typeIndex = std::type_index(typeid(T));
    request.priority  = priority;

    {
        std::lock_guard lock(m_loadQueueMutex);
        m_loadQueue.push(request);
    }

    return handle;
}

template <typename T>
T *JzAssetManager::Get(JzAssetHandle<T> handle)
{
    if (!m_initialized || !handle.IsValid()) {
        return nullptr;
    }

    auto *registry = GetOrCreateRegistry<T>();
    if (!registry) {
        return nullptr;
    }

    T *asset = registry->Get(handle);

    // Update LRU cache on access
    if (asset && m_lruCache) {
        m_lruCache->RecordAccess(handle.GetId(), registry->GetMemorySize(handle));
    }

    return asset;
}

template <typename T>
const T *JzAssetManager::Get(JzAssetHandle<T> handle) const
{
    if (!m_initialized || !handle.IsValid()) {
        return nullptr;
    }

    std::shared_lock lock(m_registryMutex);
    auto             it = m_registries.find(std::type_index(typeid(T)));
    if (it == m_registries.end()) {
        return nullptr;
    }

    auto *registry = static_cast<JzAssetRegistry<T> *>(it->second.registry.get());
    return registry->Get(handle);
}

template <typename T>
std::shared_ptr<T> JzAssetManager::GetShared(JzAssetHandle<T> handle)
{
    if (!m_initialized || !handle.IsValid()) {
        return nullptr;
    }

    auto *registry = GetOrCreateRegistry<T>();
    if (!registry) {
        return nullptr;
    }

    auto asset = registry->GetShared(handle);

    // Update LRU cache on access
    if (asset && m_lruCache) {
        m_lruCache->RecordAccess(handle.GetId(), registry->GetMemorySize(handle));
    }

    return asset;
}

template <typename T>
Bool JzAssetManager::IsValid(JzAssetHandle<T> handle) const
{
    if (!m_initialized || !handle.IsValid()) {
        return false;
    }

    std::shared_lock lock(m_registryMutex);
    auto             it = m_registries.find(std::type_index(typeid(T)));
    if (it == m_registries.end()) {
        return false;
    }

    auto *registry = static_cast<JzAssetRegistry<T> *>(it->second.registry.get());
    return registry->IsValid(handle);
}

template <typename T>
Bool JzAssetManager::IsLoaded(JzAssetHandle<T> handle) const
{
    return GetLoadState<T>(handle) == JzEAssetLoadState::Loaded;
}

template <typename T>
JzEAssetLoadState JzAssetManager::GetLoadState(JzAssetHandle<T> handle) const
{
    if (!m_initialized || !handle.IsValid()) {
        return JzEAssetLoadState::NotLoaded;
    }

    std::shared_lock lock(m_registryMutex);
    auto             it = m_registries.find(std::type_index(typeid(T)));
    if (it == m_registries.end()) {
        return JzEAssetLoadState::NotLoaded;
    }

    auto *registry = static_cast<JzAssetRegistry<T> *>(it->second.registry.get());
    return registry->GetLoadState(handle);
}

template <typename T>
void JzAssetManager::AddRef(JzAssetHandle<T> handle)
{
    if (!m_initialized || !handle.IsValid()) {
        return;
    }

    auto *registry = GetOrCreateRegistry<T>();
    if (registry) {
        registry->AddRef(handle);
    }
}

template <typename T>
void JzAssetManager::Release(JzAssetHandle<T> handle)
{
    if (!m_initialized || !handle.IsValid()) {
        return;
    }

    auto *registry = GetOrCreateRegistry<T>();
    if (registry) {
        registry->Release(handle);
    }
}

template <typename T>
void JzAssetManager::ForceUnload(JzAssetHandle<T> handle)
{
    if (!m_initialized || !handle.IsValid()) {
        return;
    }

    auto *registry = GetOrCreateRegistry<T>();
    if (!registry) {
        return;
    }

    // Remove from LRU cache
    if (m_lruCache) {
        m_lruCache->Remove(handle.GetId());
    }

    // Free the slot
    registry->Free(handle);
}

template <typename T>
void JzAssetManager::RegisterFactory(std::unique_ptr<JzResourceFactory> factory)
{
    std::lock_guard lock(m_factoryMutex);
    m_factories[std::type_index(typeid(T))] = std::move(factory);
}

template <typename T>
JzAssetRegistry<T> &JzAssetManager::GetRegistry()
{
    auto *registry = GetOrCreateRegistry<T>();
    if (!registry) {
        throw std::runtime_error("Failed to get or create registry");
    }
    return *registry;
}

template <typename T>
const JzAssetRegistry<T> &JzAssetManager::GetRegistry() const
{
    std::shared_lock lock(m_registryMutex);
    auto             it = m_registries.find(std::type_index(typeid(T)));
    if (it == m_registries.end()) {
        throw std::runtime_error("Registry not found for type");
    }
    return *static_cast<JzAssetRegistry<T> *>(it->second.registry.get());
}

template <typename T>
void JzAssetManager::DoLoadAsset(JzAssetHandle<T> handle, const String &path)
{
    auto *registry = GetOrCreateRegistry<T>();
    if (!registry) {
        return;
    }

    // Find full path
    String fullPath = FindFullPath(path);
    if (fullPath.empty()) {
        fullPath = path; // Use original path if not found
    }

    // Try to use factory to create the resource
    std::shared_ptr<T> asset;

    {
        std::lock_guard lock(m_factoryMutex);
        auto            factoryIt = m_factories.find(std::type_index(typeid(T)));
        if (factoryIt != m_factories.end()) {
            JzResource *rawResource = factoryIt->second->Create(fullPath);
            if (rawResource) {
                // Load the resource
                Bool loadSuccess = rawResource->Load();
                if (loadSuccess) {
                    asset = std::shared_ptr<T>(static_cast<T *>(rawResource));
                } else {
                    delete rawResource;
                }
            }
        }
    }

    if (asset) {
        registry->Set(handle, asset);
        registry->SetLoadState(handle, JzEAssetLoadState::Loaded);

        // Estimate memory size based on resource type
        // This is a simple heuristic - specific resource types should override
        Size memSize = sizeof(T); // Basic size

        // Try to get more accurate size from the resource if it implements GetMemorySize
        // For now, use a conservative estimate
        registry->SetMemorySize(handle, memSize);

        // Update LRU cache
        if (m_lruCache) {
            m_lruCache->RecordAccess(handle.GetId(), memSize);
        }

        JzRE_LOG_INFO("JzAssetManager: Loaded '{}' successfully", path);
    } else {
        registry->SetError(handle, "Failed to load resource");
        JzRE_LOG_ERROR("JzAssetManager: Failed to load '{}'", path);
    }
}

template <typename T>
JzAssetRegistry<T> *JzAssetManager::GetOrCreateRegistry()
{
    std::type_index typeIdx(typeid(T));

    // Try to find existing registry
    {
        std::shared_lock lock(m_registryMutex);
        auto             it = m_registries.find(typeIdx);
        if (it != m_registries.end()) {
            return static_cast<JzAssetRegistry<T> *>(it->second.registry.get());
        }
    }

    // Create new registry
    {
        std::unique_lock lock(m_registryMutex);

        // Double-check after acquiring exclusive lock
        auto it = m_registries.find(typeIdx);
        if (it != m_registries.end()) {
            return static_cast<JzAssetRegistry<T> *>(it->second.registry.get());
        }

        // Create and store
        auto  registry = std::make_unique<JzAssetRegistry<T>>(1024);
        auto *rawPtr   = registry.get();

        RegistryEntry entry;
        entry.registry = std::unique_ptr<void, void (*)(void *)>(
            registry.release(),
            [](void *ptr) { delete static_cast<JzAssetRegistry<T> *>(ptr); });
        entry.typeIndex = typeIdx;

        m_registries[typeIdx] = std::move(entry);

        return rawPtr;
    }
}

} // namespace JzRE
