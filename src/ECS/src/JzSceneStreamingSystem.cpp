/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/ECS/JzSceneStreamingSystem.h"
#include <algorithm>
#include "JzRE/ECS/JzEntityManager.h"
#include "JzRE/ECS/JzComponent.h"

void JzRE::JzSceneStreamingSystem::Update(JzRE::JzEntityManager &manager, JzRE::F32 delta)
{
    auto view = manager.View<JzStreamingComponent, JzSpatialComponent>();

    for (auto entity : view) {
        auto &streaming = manager.GetComponent<JzStreamingComponent>(entity);
        auto &spatial   = manager.GetComponent<JzSpatialComponent>(entity);

        // TODO
        // streaming.distanceToPlayer = glm::distance(spatial.position, viewerPosition);
    }

    ProcessStreamingRequests(manager);
}

void JzRE::JzSceneStreamingSystem::ProcessStreamingRequests(JzRE::JzEntityManager &manager)
{
    std::vector<JzEntity> streamingEntities;
    auto                  view = manager.View<JzStreamingComponent>();
    for (auto entity : view) {
        streamingEntities.push_back(entity);
    }

    std::sort(streamingEntities.begin(), streamingEntities.end(),
              [&](JzEntity a, JzEntity b) {
                  auto &streamA = manager.GetComponent<JzStreamingComponent>(a);
                  auto &streamB = manager.GetComponent<JzStreamingComponent>(b);
                  // TODO return CalculatePriority(streamA) > CalculatePriority(streamB);
                  return true;
              });

    I32 loadedCount = 0;
    for (auto entity : streamingEntities) {
        auto &streaming = manager.GetComponent<JzStreamingComponent>(entity);

        // TODO
        // if (ShouldLoad(streaming, loadedCount)) {
        //     LoadSceneSection(entity);
        //     loadedCount++;
        // } else if (ShouldUnload(streaming)) {
        //     UnloadSceneSection(entity);
        // }
    }
}