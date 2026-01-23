/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Function/ECS/JzWorld.h"

namespace JzRE {

// ==================== Component Management ====================

template <typename T, typename... Args>
decltype(auto) JzWorld::AddComponent(JzEntity entity, Args &&...args)
{
    if constexpr (std::is_same_v<decltype(m_registry.emplace<T>(entity, std::forward<Args>(args)...)), void>) {
        m_registry.emplace<T>(entity, std::forward<Args>(args)...);
        return m_registry.get<T>(entity);
    } else {
        return m_registry.emplace<T>(entity, std::forward<Args>(args)...);
    }
}

template <typename T, typename... Args>
decltype(auto) JzWorld::AddOrReplaceComponent(JzEntity entity, Args &&...args)
{
    if constexpr (std::is_same_v<decltype(m_registry.emplace<T>(entity, std::forward<Args>(args)...)), void>) {
        m_registry.emplace_or_replace<T>(entity, std::forward<Args>(args)...);
        return m_registry.get<T>(entity);
    } else {
        return m_registry.emplace_or_replace<T>(entity, std::forward<Args>(args)...);
    }
}

template <typename T>
void JzWorld::RemoveComponent(JzEntity entity)
{
    m_registry.remove<T>(entity);
}

template <typename T>
T &JzWorld::GetComponent(JzEntity entity)
{
    return m_registry.get<T>(entity);
}

template <typename T>
const T &JzWorld::GetComponent(JzEntity entity) const
{
    return m_registry.get<T>(entity);
}

template <typename T>
T *JzWorld::TryGetComponent(JzEntity entity)
{
    return m_registry.try_get<T>(entity);
}

template <typename T>
const T *JzWorld::TryGetComponent(JzEntity entity) const
{
    return m_registry.try_get<T>(entity);
}

template <typename T>
Bool JzWorld::HasComponent(JzEntity entity) const
{
    return m_registry.all_of<T>(entity);
}

// ==================== View/Query ====================

template <typename... Components>
auto JzWorld::View()
{
    return m_registry.view<Components...>();
}

template <typename... Components>
auto JzWorld::View() const
{
    return m_registry.view<Components...>();
}

// ==================== System Management ====================

template <typename T, typename... Args>
std::shared_ptr<T> JzWorld::RegisterSystem(Args &&...args)
{
    static_assert(std::is_base_of_v<JzSystem, T>, "T must derive from JzSystem");
    auto system = std::make_shared<T>(std::forward<Args>(args)...);
    m_systems.push_back(system);
    return system;
}

} // namespace JzRE
