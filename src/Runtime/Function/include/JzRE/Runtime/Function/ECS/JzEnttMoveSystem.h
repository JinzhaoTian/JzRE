/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Function/ECS/JzEnttSystem.h"
#include "JzRE/Runtime/Function/ECS/JzEnttWorld.h"
#include "JzRE/Runtime/Function/ECS/JzEnttComponents.h"

namespace JzRE {

/**
 * @brief Movement system that updates entity positions based on velocity.
 *
 * This is an EnTT-based reimplementation of the original JzMoveSystem.
 * It demonstrates the EnTT idioms for iterating over entities with
 * specific component combinations.
 */
class JzEnttMoveSystem : public JzEnttSystem {
public:
    /**
     * @brief Updates the positions of all entities with transform and velocity components.
     *
     * @param world The EnTT world containing entities and components.
     * @param delta The delta time since the last frame.
     */
    void Update(JzEnttWorld &world, F32 delta) override
    {
        // EnTT view provides cache-friendly iteration over entities
        // with the specified components. The .each() method provides
        // structured bindings for convenient access.
        auto view = world.View<JzTransformComponent, JzVelocityComponent>();

        for (auto [entity, transform, velocity] : view.each()) {
            // Update position based on velocity and delta time
            transform.position += velocity.velocity * delta;
        }
    }
};

} // namespace JzRE
