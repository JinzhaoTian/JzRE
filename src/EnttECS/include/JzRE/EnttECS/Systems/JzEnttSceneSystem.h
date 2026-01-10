/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/EnttECS/JzEnttSystem.h"
#include "JzRE/EnttECS/JzEnttWorld.h"
#include "JzRE/EnttECS/JzEnttComponents.h"

namespace JzRE {

/**
 * @brief Scene system that updates world transforms based on hierarchy.
 *
 * This is an EnTT-based reimplementation of the original JzSceneSystem.
 * It demonstrates hierarchical transform updates using EnTT's component access.
 */
class JzEnttSceneSystem : public JzEnttSystem {
public:
    /**
     * @brief Updates world transforms for all scene nodes.
     *
     * @param world The EnTT world containing entities and components.
     * @param delta The delta time since the last frame.
     */
    void Update(JzEnttWorld &world, F32 delta) override
    {
        // Find root nodes (nodes without a parent or with invalid parent)
        auto view = world.View<JzSceneNodeComponent>();

        for (auto entity : view) {
            auto &node = view.get<JzSceneNodeComponent>(entity);

            // Only process root nodes here, children will be processed recursively
            if (node.parent == INVALID_ENTT_ENTITY || !world.IsValid(node.parent)) {
                UpdateWorldTransform(world, entity);
            }
        }
    }

private:
    /**
     * @brief Recursively updates world transforms for an entity and its children.
     *
     * @param world The EnTT world containing entities and components.
     * @param entity The entity to update.
     */
    void UpdateWorldTransform(JzEnttWorld &world, JzEnttEntity entity)
    {
        auto *node = world.TryGetComponent<JzSceneNodeComponent>(entity);
        if (!node) {
            return;
        }

        // Calculate world transform
        if (world.IsValid(node->parent)) {
            auto *parentNode = world.TryGetComponent<JzSceneNodeComponent>(node->parent);
            if (parentNode) {
                node->worldTransform = parentNode->worldTransform * node->localTransform;
            } else {
                node->worldTransform = node->localTransform;
            }
        } else {
            node->worldTransform = node->localTransform;
        }

        // Recursively update children
        for (auto childId : node->children) {
            // Convert old entity ID to entt entity if needed
            // Note: This assumes children are stored as entt entities
            JzEnttEntity child = static_cast<JzEnttEntity>(childId);
            if (world.IsValid(child)) {
                UpdateWorldTransform(world, child);
            }
        }
    }
};

} // namespace JzRE
