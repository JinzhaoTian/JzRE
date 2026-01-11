/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Function/ECS/JzSceneQuerySystem.h"
#include "JzRE/Runtime/Function/ECS/JzEntityManager.h"
#include "JzRE/Runtime/Function/ECS/JzComponent.h"

void JzRE::JzSceneQuerySystem::Update(JzRE::JzEntityManager &manager, JzRE::F32 delta)
{
    auto view = manager.View<JzSpatialComponent, JzMeshComponent>();

    // TODO
}

// // 视锥体剔除
// std::vector<Entity> FrustumCull(const Frustum &frustum)
// {
//     std::vector<Entity> visibleEntities;

//     auto view = registry.view<SpatialComponent, MeshComponent>();
//     for (auto entity : view) {
//         auto &spatial = view.get<SpatialComponent>(entity);
//         if (frustum.Intersects(spatial.bounds)) {
//             visibleEntities.push_back(entity);
//         }
//     }

//     return visibleEntities;
// }

// // 射线检测
// RaycastResult Raycast(const Ray &ray)
// {
//     RaycastResult result;

//     // 使用空间分区加速查询
//     auto candidates = spatialSystem->QueryEntitiesInRange(ray.origin, ray.GetLength());

//     for (auto entity : candidates) {
//         if (auto *spatial = registry.try_get<SpatialComponent>(entity)) {
//             float distance;
//             if (ray.Intersects(spatial->bounds, distance)) {
//                 if (distance < result.distance) {
//                     result.entity   = entity;
//                     result.distance = distance;
//                     result.hitPoint = ray.origin + ray.direction * distance;
//                 }
//             }
//         }
//     }

//     return result;
// }

// // 查找子实体
// std::vector<Entity> FindChildren(Entity parent)
// {
//     std::vector<Entity> children;

//     if (auto *node = registry.try_get<SceneNodeComponent>(parent)) {
//         children = node->children;
//     }

//     return children;
// }