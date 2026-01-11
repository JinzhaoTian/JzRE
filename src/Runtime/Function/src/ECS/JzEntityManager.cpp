/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Function/ECS/JzEntityManager.h"

JzRE::JzEntity JzRE::JzEntityManager::CreateEntity()
{
    return m_entity_counter++;
}

void JzRE::JzEntityManager::DestroyEntity(JzRE::JzEntity entity)
{
    // Iterate over all component pools and remove the entity
    for (auto const &[type_name, pool] : m_component_pools) {
        pool->RemoveEntity(entity);
    }
}
