/**
 * @file    JzSceneSerializer.cpp
 * @brief   Scene serialization and deserialization implementation
 * @author  Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Function/Scene/JzSceneSerializer.h"

#include <fstream>
#include <nlohmann/json.hpp>

#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Runtime/Function/ECS/JzWorld.h"
#include "JzRE/Runtime/Function/ECS/JzEntityComponents.h"
#include "JzRE/Runtime/Function/ECS/JzTransformComponents.h"
#include "JzRE/Runtime/Function/ECS/JzAssetComponents.h"
#include "JzRE/Runtime/Function/ECS/JzCameraComponents.h"
#include "JzRE/Runtime/Function/ECS/JzWindowComponents.h"
#include "JzRE/Runtime/Function/ECS/JzAssetSystem.h"

using json = nlohmann::json;

namespace JzRE {

Bool JzSceneSerializer::Serialize(JzWorld &world, const std::filesystem::path &filepath)
{
    json sceneJson;
    sceneJson["version"] = SCENE_VERSION;
    sceneJson["entities"] = json::array();

    auto entities = GetSerializableEntities(world);

    for (auto entity : entities) {
        json entityJson;

        // Serialize name
        if (world.HasComponent<JzNameComponent>(entity)) {
            auto &name = world.GetComponent<JzNameComponent>(entity);
            entityJson["name"] = name.name;
        }

        // Serialize UUID
        if (world.HasComponent<JzUUIDComponent>(entity)) {
            auto &uuid = world.GetComponent<JzUUIDComponent>(entity);
            entityJson["uuid"] = uuid.uuid;
        }

        // Serialize transform
        if (world.HasComponent<JzTransformComponent>(entity)) {
            auto &transform = world.GetComponent<JzTransformComponent>(entity);
            entityJson["transform"] = {
                {"position", {transform.position.x, transform.position.y, transform.position.z}},
                {"rotation", {transform.rotation.x, transform.rotation.y, transform.rotation.z}},
                {"scale", {transform.scale.x, transform.scale.y, transform.scale.z}}
            };
        }

        // Serialize asset paths
        if (world.HasComponent<JzAssetPathComponent>(entity)) {
            auto &assetPath = world.GetComponent<JzAssetPathComponent>(entity);
            json assetsJson;
            if (!assetPath.modelPath.empty()) {
                assetsJson["model"] = assetPath.modelPath;
            }
            if (!assetPath.materialPath.empty()) {
                assetsJson["material"] = assetPath.materialPath;
            }
            if (!assetPath.shaderPath.empty()) {
                assetsJson["shader"] = assetPath.shaderPath;
            }
            if (!assetsJson.empty()) {
                entityJson["assets"] = assetsJson;
            }
        }

        // Serialize tags
        json tagsJson = json::array();
        if (world.HasComponent<JzActiveTag>(entity)) {
            tagsJson.push_back("JzActiveTag");
        }
        if (world.HasComponent<JzStaticTag>(entity)) {
            tagsJson.push_back("JzStaticTag");
        }
        if (!tagsJson.empty()) {
            entityJson["tags"] = tagsJson;
        }

        sceneJson["entities"].push_back(entityJson);
    }

    // Write to file
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    file << sceneJson.dump(2);
    return true;
}

Bool JzSceneSerializer::Deserialize(JzWorld &world, const std::filesystem::path &filepath)
{
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    json sceneJson;
    try {
        file >> sceneJson;
    } catch (const json::parse_error &) {
        return false;
    }

    // Check version
    if (!sceneJson.contains("version") || sceneJson["version"] != SCENE_VERSION) {
        return false;
    }

    if (!sceneJson.contains("entities") || !sceneJson["entities"].is_array()) {
        return false;
    }

    // Get asset system for loading models
    JzAssetSystem *assetSystem = nullptr;
    if (JzServiceContainer::Has<JzAssetSystem>()) {
        assetSystem = &JzServiceContainer::Get<JzAssetSystem>();
    }

    for (const auto &entityJson : sceneJson["entities"]) {
        JzEntity entity = INVALID_ENTITY;

        // Check if this entity has a model to load
        if (entityJson.contains("assets") && entityJson["assets"].contains("model") && assetSystem) {
            String modelPath = entityJson["assets"]["model"];
            auto modelHandle = assetSystem->LoadSync<JzModel>(modelPath);

            if (assetSystem->IsValid(modelHandle)) {
                auto spawnedEntities = assetSystem->SpawnModel(world, modelHandle);
                if (!spawnedEntities.empty()) {
                    entity = spawnedEntities[0];

                    // Add asset path component
                    world.AddComponent<JzAssetPathComponent>(entity, modelPath);
                }
            }
        }

        // If no model, create empty entity
        if (!IsValidEntity(entity)) {
            entity = world.CreateEntity();
        }

        // Apply name
        if (entityJson.contains("name")) {
            String name = entityJson["name"];
            if (world.HasComponent<JzNameComponent>(entity)) {
                world.GetComponent<JzNameComponent>(entity).name = name;
            } else {
                world.AddComponent<JzNameComponent>(entity, name);
            }
        }

        // Apply UUID
        if (entityJson.contains("uuid")) {
            U64 uuid = entityJson["uuid"];
            if (!world.HasComponent<JzUUIDComponent>(entity)) {
                world.AddComponent<JzUUIDComponent>(entity, uuid);
            }
        }

        // Apply transform
        if (entityJson.contains("transform")) {
            auto &t = entityJson["transform"];
            JzTransformComponent transform;

            if (t.contains("position") && t["position"].is_array()) {
                transform.position = JzVec3(t["position"][0], t["position"][1], t["position"][2]);
            }
            if (t.contains("rotation") && t["rotation"].is_array()) {
                transform.rotation = JzVec3(t["rotation"][0], t["rotation"][1], t["rotation"][2]);
            }
            if (t.contains("scale") && t["scale"].is_array()) {
                transform.scale = JzVec3(t["scale"][0], t["scale"][1], t["scale"][2]);
            }

            if (world.HasComponent<JzTransformComponent>(entity)) {
                world.GetComponent<JzTransformComponent>(entity) = transform;
            } else {
                world.AddComponent<JzTransformComponent>(entity, transform);
            }
        }

        // Apply tags
        if (entityJson.contains("tags") && entityJson["tags"].is_array()) {
            for (const auto &tag : entityJson["tags"]) {
                String tagName = tag;
                if (tagName == "JzActiveTag" && !world.HasComponent<JzActiveTag>(entity)) {
                    world.AddComponent<JzActiveTag>(entity);
                }
                if (tagName == "JzStaticTag" && !world.HasComponent<JzStaticTag>(entity)) {
                    world.AddComponent<JzStaticTag>(entity);
                }
            }
        }
    }

    return true;
}

void JzSceneSerializer::ClearScene(JzWorld &world)
{
    auto entities = GetSerializableEntities(world);

    if (JzServiceContainer::Has<JzAssetSystem>()) {
        auto &assetSystem = JzServiceContainer::Get<JzAssetSystem>();
        for (auto entity : entities) {
            assetSystem.DetachAllAssets(world, entity);
        }
    }

    for (auto entity : entities) {
        world.DestroyEntity(entity);
    }
}

std::vector<JzEntity> JzSceneSerializer::GetSerializableEntities(JzWorld &world)
{
    std::vector<JzEntity> result;

    // Get all entities with JzNameComponent (user-created entities)
    auto view = world.View<JzNameComponent>();
    for (auto [entity, name] : view.each()) {
        // Skip system entities (camera, lights created by runtime)
        // Only include entities with asset path or without special tags
        if (world.HasComponent<JzAssetPathComponent>(entity) ||
            (!world.HasComponent<JzMainCameraTag>(entity) &&
             !world.HasComponent<JzPrimaryWindowTag>(entity))) {
            result.push_back(entity);
        }
    }

    return result;
}

} // namespace JzRE
