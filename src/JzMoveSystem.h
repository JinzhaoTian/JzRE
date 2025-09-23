/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzSystem.h"

namespace JzRE {

/**
 * @brief A system that updates the position of entities based on their velocity.
 */
class JzMoveSystem : public JzSystem {
public:
    /**
     * @brief Updates the position of all entities with Transform and Velocity components.
     *
     * @param manager The entity manager.
     * @param delta The delta time since the last frame.
     */
    void Update(JzEntityManager &manager, F32 delta) override;
};

} // namespace JzRE
