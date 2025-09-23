/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzWorld.h"

namespace JzRE {

template <typename T>
void JzWorld::RegisterComponent()
{
    // TODO
}

template <typename T>
T &JzWorld::AddComponent(JzEntity entity, T &&component)
{
    // TODO
}

template <typename T>
void RemoveComponent(JzEntity entity)
{
    // TODO
}

template <typename T>
T &GetComponent(JzEntity entity)
{
    return GetComponentPool<T>()->GetComponent(entity);
}

template <typename T>
std::shared_ptr<JzComponentPool<T>> GetComponentPool()
{
    // TODO
}

template <typename T>
std::shared_ptr<T> JzWorld::RegisterSystem()
{
    auto system = std::make_shared<T>();
    m_systems.push_back(system);
    return system;
}

} // namespace JzRE
