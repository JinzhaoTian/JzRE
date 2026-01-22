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

void JzWorld::UpdateLogic(F32 delta)
{
    // Update all logic phase systems in order: Input -> Physics -> Animation -> Logic
    for (auto &system : m_systems) {
        if (system && system->IsEnabled() && IsLogicPhase(system->GetPhase())) {
            system->Update(*this, delta);
        }
    }
}

void JzWorld::UpdatePreRender(F32 delta)
{
    // Update all pre-render phase systems in order: PreRender -> Culling
    for (auto &system : m_systems) {
        if (system && system->IsEnabled() && IsPreRenderPhase(system->GetPhase())) {
            system->Update(*this, delta);
        }
    }
}

void JzWorld::UpdateRender(F32 delta)
{
    // Update all render phase systems in order: RenderPrep -> Render
    for (auto &system : m_systems) {
        if (system && system->IsEnabled() && IsRenderPhase(system->GetPhase())) {
            system->Update(*this, delta);
        }
    }
}

} // namespace JzRE
