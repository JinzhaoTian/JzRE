/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/ECS/JzCollisionDetectionSystem.h"
#include "JzRE/ECS/JzEntityManager.h"
#include "JzRE/ECS/JzComponent.h"

void JzRE::JzCollisionDetectionSystem::Update(JzRE::JzEntityManager &manager, JzRE::F32 delta)
{
    // TODO 使用空间分区系统获取可能碰撞的实体对
    // auto potentialCollisions = spatialPartitionSystem->GetPotentialCollisions();

    // for (auto &pair : potentialCollisions) {
    //     // 进行精确的碰撞检测
    //     if (CheckCollision(pair.first, pair.second)) {
    //         // 处理碰撞事件
    //     }
    // }
}