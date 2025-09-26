/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/ECS/JzMoveSystem.h"
#include "JzRE/ECS/JzEntityManager.h"
#include "JzRE/ECS/JzComponent.h"

void JzRE::JzMoveSystem::Update(JzRE::JzEntityManager &manager, JzRE::F32 delta)
{
    // Get all entities that have both a TransformComponent and a VelocityComponent
    auto entities = manager.View<TransformComponent, VelocityComponent>();

    for (auto entity : entities) {
        auto &transform = manager.GetComponent<TransformComponent>(entity);
        auto &velocity  = manager.GetComponent<VelocityComponent>(entity);

        transform.position += velocity.velocity * delta;
    }
}
