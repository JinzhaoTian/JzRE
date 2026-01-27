/**
 * @file    JzAssetSystem.inl
 * @brief   Template implementation for JzAssetSystem
 * @author  Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Function/ECS/JzAssetSystem.h"

namespace JzRE {

// ==================== Initialization ====================

template <typename T>
void JzAssetSystem::RegisterFactory(std::unique_ptr<JzResourceFactory> factory)
{
    m_assetManager->RegisterFactory<T>(std::move(factory));
}

// ==================== Loading API ====================

template <typename T>
JzAssetHandle<T> JzAssetSystem::LoadSync(const String &path)
{
    return m_assetManager->LoadSync<T>(path);
}

template <typename T>
JzAssetHandle<T> JzAssetSystem::GetOrLoad(const String &path)
{
    return m_assetManager->GetOrLoad<T>(path);
}

template <typename T>
JzAssetHandle<T> JzAssetSystem::LoadAsync(const String          &path,
                                          JzAssetLoadCallback<T> callback,
                                          I32                    priority)
{
    return m_assetManager->LoadAsync<T>(path, std::move(callback), priority);
}

// ==================== Registration API ====================

template <typename T>
JzAssetHandle<T> JzAssetSystem::RegisterAsset(const String &path, std::shared_ptr<T> asset)
{
    auto &registry = m_assetManager->GetRegistry<T>();

    auto handle = registry.Allocate(path);
    if (!handle.IsValid()) {
        return JzAssetHandle<T>::Invalid();
    }

    registry.Set(handle, std::move(asset));
    registry.SetLoadState(handle, JzEAssetLoadState::Loaded);
    m_assetManager->AddRef(handle);

    return handle;
}

// ==================== Access API ====================

template <typename T>
T *JzAssetSystem::Get(JzAssetHandle<T> handle)
{
    return m_assetManager->Get(handle);
}

template <typename T>
const T *JzAssetSystem::Get(JzAssetHandle<T> handle) const
{
    return m_assetManager->Get(handle);
}

template <typename T>
std::shared_ptr<T> JzAssetSystem::GetShared(JzAssetHandle<T> handle)
{
    return m_assetManager->GetShared(handle);
}

template <typename T>
Bool JzAssetSystem::IsValid(JzAssetHandle<T> handle) const
{
    return m_assetManager->IsValid(handle);
}

template <typename T>
Bool JzAssetSystem::IsLoaded(JzAssetHandle<T> handle) const
{
    return m_assetManager->IsLoaded(handle);
}

template <typename T>
JzEAssetLoadState JzAssetSystem::GetLoadState(JzAssetHandle<T> handle) const
{
    return m_assetManager->GetLoadState(handle);
}

// ==================== Reference Counting ====================

template <typename T>
void JzAssetSystem::AddRef(JzAssetHandle<T> handle)
{
    m_assetManager->AddRef(handle);
}

template <typename T>
void JzAssetSystem::Release(JzAssetHandle<T> handle)
{
    m_assetManager->Release(handle);
}

} // namespace JzRE
