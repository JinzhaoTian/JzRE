/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Function/ECS/JzWorld.h"

namespace JzRE {

JzEntity JzWorld::CreateEntity()
{
    return m_registry.create();
}

void JzWorld::DestroyEntity(JzEntity entity)
{
    m_registry.destroy(entity);
}

Bool JzWorld::IsValid(JzEntity entity) const
{
    return m_registry.valid(entity);
}

Size JzWorld::GetEntityCount() const
{
    // Use storage().size() - storage().free_list() in older EnTT, or alive() in newer versions
    // For EnTT 3.15+, we use the alive count from the registry
    return static_cast<Size>(m_registry.storage<entt::entity>()->size() - m_registry.storage<entt::entity>()->free_list());
}

void JzWorld::Update(F32 delta)
{
    for (auto &system : m_systems) {
        if (system && system->IsEnabled()) {
            system->Update(*this, delta);
        }
    }
}

} // namespace JzRE
