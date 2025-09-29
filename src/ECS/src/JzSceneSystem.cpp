/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/ECS/JzSceneSystem.h"
#include "JzRE/ECS/JzEntity.h"
#include "JzRE/ECS/JzEntityManager.h"
#include "JzRE/ECS/JzComponent.h"

void JzRE::JzSceneSystem::Update(JzRE::JzEntityManager &manager, JzRE::F32 delta)
{
    auto view = manager.View<JzSceneNodeComponent>();
    for (auto entity : view) {
        UpdateWorldTransform(manager, entity);
    }
}

void JzRE::JzSceneSystem::UpdateWorldTransform(JzRE::JzEntityManager &manager, JzRE::JzEntity entity)
{
    auto &node = manager.GetComponent<JzSceneNodeComponent>(entity);
    if (node.parent != INVALID_ENTITY) {
        auto &parentNode    = manager.GetComponent<JzSceneNodeComponent>(node.parent);
        node.worldTransform = parentNode.worldTransform * node.localTransform;
    } else {
        node.worldTransform = node.localTransform;
    }

    for (auto child : node.children) {
        UpdateWorldTransform(manager, child);
    }
}