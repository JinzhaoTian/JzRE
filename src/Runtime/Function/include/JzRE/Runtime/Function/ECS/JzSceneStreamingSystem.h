/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Function/ECS/JzSystem.h"

namespace JzRE {

/**
 * @brief Scene streaming system
 */
class JzSceneStreamingSystem : public JzSystem {
public:
    /**
     * @brief Update
     *
     * @param manager The entity manager to query entities from.
     * @param delta The delta time since the last frame.
     */
    void Update(JzEntityManager &manager, F32 delta) override;

private:
    void ProcessStreamingRequests(JzEntityManager &manager);
};

} // namespace JzRE