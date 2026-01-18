/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Function/ECS/JzSpatialPartitionSystem.h"
#include "JzRE/Runtime/Function/ECS/JzComponent.h"
#include "JzRE/Runtime/Function/ECS/JzEntityManager.h"

void JzRE::JzSpatialPartitionSystem::Update(JzRE::JzEntityManager &manager, JzRE::F32 delta)
{
    auto view = manager.View<JzSpatialComponent>();
    for (auto entity : view) {
        auto &spatial = manager.GetComponent<JzSpatialComponent>(entity);

        I32 newCellX = static_cast<I32>(spatial.position.x / CELL_SIZE);
        I32 newCellY = static_cast<I32>(spatial.position.y / CELL_SIZE);
        I32 newCellZ = static_cast<I32>(spatial.position.z / CELL_SIZE);

        if (newCellX != spatial.gridCellX || newCellY != spatial.gridCellY || newCellZ != spatial.gridCellZ) {
            // TODO
            // RemoveFromCell(spatial.gridCellX, spatial.gridCellY, spatial.gridCellZ, entity);
            // AddToCell(newCellX, newCellY, newCellZ, entity);

            spatial.gridCellX = newCellX;
            spatial.gridCellY = newCellY;
            spatial.gridCellZ = newCellZ;
        }
    }
}

std::vector<JzRE::JzEntity> JzRE::JzSpatialPartitionSystem::QueryEntitiesInRange(const JzRE::JzVec3 &center, JzRE::F32 radius)
{
    std::vector<JzEntity>     results;
    JzBoundingSphereComponent sphere(center, radius);

    // TODO 计算影响的网格范围
    // auto [minCell, maxCell] = CalculateAffectedCells(sphere);

    // for (int x = minCell.x; x <= maxCell.x; ++x) {
    //     for (int y = minCell.y; y <= maxCell.y; ++y) {
    //         for (int z = minCell.z; z <= maxCell.z; ++z) {
    //             auto &cellEntities = gridCells[GetCellIndex(x, y, z)];
    //             for (auto entity : cellEntities) {
    //                 if (TestSphereAABB(sphere, GetEntityBounds(entity))) {
    //                     results.push_back(entity);
    //                 }
    //             }
    //         }
    //     }
    // }

    return results;
}