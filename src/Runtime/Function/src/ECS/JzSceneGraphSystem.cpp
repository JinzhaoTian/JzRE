/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Function/ECS/JzSceneGraphSystem.h"
#include "JzRE/Runtime/Function/ECS/JzEntityManager.h"
#include "JzRE/Runtime/Function/ECS/JzComponent.h"

void JzRE::JzSceneGraphSystem::Update(JzRE::JzEntityManager &manager, JzRE::F32 delta)
{
    auto view = manager.View<JzTransformComponent>();
    for (auto entity : view) {
        auto &transform = manager.GetComponent<JzTransformComponent>(entity);
        // if (transform.parent == NullEntity) {
        //     UpdateWorldTransform(entity, transform);
        // }
    }
}