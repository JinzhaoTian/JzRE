/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzWorld.h"

JzRE::JzEntity JzRE::JzWorld::CreateEntity()
{
    return m_entityManager.CreateEntity();
}

void JzRE::JzWorld::DestroyEntity(JzRE::JzEntity entity)
{
    m_entityManager.DestroyEntity(entity);

    // for (auto &system : m_systems) {
    //
    // }

    for (auto &pool : m_componentPools) {
        if (pool) {
            pool->RemoveEntity(entity);
        }
    }
}

void JzRE::JzWorld::Update(JzRE::F32 delta)
{
    for (const auto &system : m_systems) {
        system->Update(m_entityManager, delta);
    }
}