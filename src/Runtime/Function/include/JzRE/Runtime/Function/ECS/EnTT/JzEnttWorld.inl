/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Function/ECS/EnTT/JzEnttWorld.h"

namespace JzRE {

// ==================== Component Management ====================

template <typename T, typename... Args>
T &JzEnttWorld::AddComponent(JzEnttEntity entity, Args &&...args)
{
    return m_registry.emplace<T>(entity, std::forward<Args>(args)...);
}

template <typename T>
void JzEnttWorld::RemoveComponent(JzEnttEntity entity)
{
    m_registry.remove<T>(entity);
}

template <typename T>
T &JzEnttWorld::GetComponent(JzEnttEntity entity)
{
    return m_registry.get<T>(entity);
}

template <typename T>
const T &JzEnttWorld::GetComponent(JzEnttEntity entity) const
{
    return m_registry.get<T>(entity);
}

template <typename T>
T *JzEnttWorld::TryGetComponent(JzEnttEntity entity)
{
    return m_registry.try_get<T>(entity);
}

template <typename T>
const T *JzEnttWorld::TryGetComponent(JzEnttEntity entity) const
{
    return m_registry.try_get<T>(entity);
}

template <typename T>
Bool JzEnttWorld::HasComponent(JzEnttEntity entity) const
{
    return m_registry.all_of<T>(entity);
}

// ==================== View/Query ====================

template <typename... Components>
auto JzEnttWorld::View()
{
    return m_registry.view<Components...>();
}

template <typename... Components>
auto JzEnttWorld::View() const
{
    return m_registry.view<Components...>();
}

// ==================== System Management ====================

template <typename T, typename... Args>
std::shared_ptr<T> JzEnttWorld::RegisterSystem(Args &&...args)
{
    static_assert(std::is_base_of_v<JzEnttSystem, T>, "T must derive from JzEnttSystem");
    auto system = std::make_shared<T>(std::forward<Args>(args)...);
    m_systems.push_back(system);
    return system;
}

} // namespace JzRE
