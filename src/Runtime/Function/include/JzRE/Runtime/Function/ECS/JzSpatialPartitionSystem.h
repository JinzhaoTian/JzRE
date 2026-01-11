/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <unordered_map>
#include <vector>
#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzVector.h"
#include "JzRE/Runtime/Function/ECS/JzEntity.h"
#include "JzRE/Runtime/Function/ECS/JzSystem.h"

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

    /**
     * @brief Query entities in a spherical range.
     *
     * @param center
     * @param radius
     * @return std::vector<JzEntity>
     */
    std::vector<JzEntity> QueryEntitiesInRange(const JzVec3 &center, F32 radius);

private:
    std::unordered_map<int, std::vector<JzEntity>> gridCells;
    static constexpr F32                           CELL_SIZE = 10.0f;
};

} // namespace JzRE