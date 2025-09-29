/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <unordered_map>
#include "JzRE/Core/JzRETypes.h"
#include "JzRE/Core/JzVector.h"
#include "JzRE/ECS/JzEntity.h"
#include "JzRE/ECS/JzSystem.h"

namespace JzRE {

/**
 * @brief Spatial partition system
 */
class JzSpatialPartitionSystem : public JzSystem {
public:
    /**
     * @brief Update
     *
     * @param manager The entity manager to query entities from.
     * @param delta The delta time since the last frame.
     */
    void Update(JzEntityManager &manager, F32 delta) override;

    std::vector<JzEntity> QueryEntitiesInRange(const JzVec3 &center, F32 radius);

private:
    std::unordered_map<int, std::vector<JzEntity>> gridCells;
    static constexpr F32                           CELL_SIZE = 10.0f;
};

} // namespace JzRE