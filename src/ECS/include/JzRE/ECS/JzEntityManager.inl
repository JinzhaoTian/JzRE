/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/ECS/JzEntityManager.h"

namespace JzRE {

template <typename T, typename... Args>
T &JzEntityManager::AddComponent(JzEntity entity, Args &&...args)
{
    auto pool = GetPool<T>();
    T    component(std::forward<Args>(args)...);
    pool->Add(entity, std::move(component));
    return pool->Get(entity);
}

template <typename T>
void JzEntityManager::RemoveComponent(JzEntity entity)
{
    auto pool = GetPool<T>();
    pool->RemoveEntity(entity);
}

template <typename T>
T &JzEntityManager::GetComponent(JzEntity entity)
{
    return GetPool<T>()->Get(entity);
}

template <typename T>
Bool JzEntityManager::HasComponent(JzEntity entity)
{
    return GetPool<T>()->Has(entity);
}

template <typename... Components>
std::vector<JzEntity> JzEntityManager::View()
{
    std::vector<JzEntity> entity_view;

    // Find the smallest component pool to iterate over
    const auto &smallest_pool_entities = (GetPool<Components>()->GetDense(), ...);

    // This is a simplified view. A more complex implementation would use iterators.
    // For now, we just find entities that have all components.
    const auto &first_pool = (GetPool<std::tuple_element_t<0, std::tuple<Components...>>>());
    for (JzEntity entity : first_pool->GetDense()) {
        if ((HasComponent<Components>(entity) && ...)) {
            entity_view.push_back(entity);
        }
    }
    return entity_view;
}

template <typename T>
std::shared_ptr<JzComponentPool<T>> JzEntityManager::GetPool()
{
    const char *type_name = typeid(T).name();
    if (m_component_pools.find(type_name) == m_component_pools.end()) {
        m_component_pools[type_name] = std::make_shared<JzComponentPool<T>>();
    }
    return std::static_pointer_cast<JzComponentPool<T>>(m_component_pools[type_name]);
}

} // namespace JzRE