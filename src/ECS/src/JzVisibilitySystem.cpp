/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/ECS/JzVisibilitySystem.h"
#include "JzRE/ECS/JzEntityManager.h"
#include "JzRE/ECS/JzComponent.h"

void JzRE::JzVisibilitySystem::Update(JzRE::JzEntityManager &manager, JzRE::F32 delta)
{
    auto cameraView = manager.View<JzCameraComponent, JzTransformComponent>();
    for (auto entity : cameraView) {
        auto &camera    = manager.GetComponent<JzCameraComponent>(entity);
        auto &transform = manager.GetComponent<JzTransformComponent>(entity);

        // Frustum frustum   = camera.GetFrustum(transform);

        // // 从空间分区系统中获取在视锥体内的实体列表
        // auto visibleEntities = spatialPartitionSystem->Query(frustum);

        // // 将可见实体列表传递给渲染系统，可以通过事件或者直接设置共享资源
        // // 例如，设置一个共享的VisibleSet，渲染系统会读取它
    }
}