/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/ECS/JzEntity.h"
#include "JzRE/ECS/JzSystem.h"

namespace JzRE {

/**
 * @brief Scene system
 */
class JzSceneSystem : public JzSystem {
public:
    /**
     * @brief Update
     *
     * @param manager The entity manager to query entities from.
     * @param delta The delta time since the last frame.
     */
    void Update(JzEntityManager &manager, F32 delta) override;

private:
    void UpdateWorldTransform(JzEntityManager &manager, JzEntity entity);
};

} // namespace JzRE