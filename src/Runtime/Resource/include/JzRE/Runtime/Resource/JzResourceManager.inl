/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Resource/JzResourceManager.h"
#include "JzRE/Runtime/Core/JzLogger.h"

namespace JzRE {

template <typename T>
void JzResourceManager::RegisterFactory(std::unique_ptr<JzResourceFactory> factory)
{
    m_factories[std::type_index(typeid(T))] = std::move(factory);
}

template <typename T>
std::shared_ptr<T> JzResourceManager::GetResource(const String &name)
{
    std::lock_guard<std::mutex> lock(m_cacheMutex);

    // cache lookup
    auto it = m_resourceCache.find(name);
    if (it != m_resourceCache.end()) {
        if (auto sharedRes = it->second.lock()) {
            return std::static_pointer_cast<T>(sharedRes);
        }
    }

    // cache miss or expired: create new resource
    auto factory_it = m_factories.find(std::type_index(typeid(T)));
    if (factory_it == m_factories.end()) {
        // Log error: No factory registered for this type
        JzRE_LOG_ERROR("No factory registered for this type");
        return nullptr;
    }

    JzResource        *newRawRes = factory_it->second->Create(name);
    std::shared_ptr<T> newRes    = std::shared_ptr<T>(static_cast<T *>(newRawRes));

    // load and cache
    newRes->Load(); // For now, synchronous load
    m_resourceCache[name] = newRes;

    return newRes;
}

} // namespace JzRE