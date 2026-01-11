/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Function/ECS/JzMoveSystem.h"
#include "JzRE/Runtime/Function/ECS/JzEntityManager.h"
#include "JzRE/Runtime/Function/ECS/JzComponent.h"

void JzRE::JzMoveSystem::Update(JzRE::JzEntityManager &manager, JzRE::F32 delta)
{
    // Get all entities that have both a TransformComponent and a VelocityComponent
    auto entities = manager.View<JzTransformComponent, JzVelocityComponent>();

    for (auto entity : entities) {
        auto &transform = manager.GetComponent<JzTransformComponent>(entity);
        auto &velocity  = manager.GetComponent<JzVelocityComponent>(entity);

        transform.position += velocity.velocity * delta;
    }
}
