/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#include "JzRE/Runtime/Function/Scene/JzDefaultSceneCreator.h"

#include <filesystem>
#include <fstream>

#include <nlohmann/json.hpp>

#include "JzRE/Runtime/Core/JzLogger.h"
#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Runtime/Function/ECS/JzWorld.h"
#include "JzRE/Runtime/Function/ECS/JzTransformComponents.h"
#include "JzRE/Runtime/Function/ECS/JzCameraComponents.h"
#include "JzRE/Runtime/Function/ECS/JzLightComponents.h"
#include "JzRE/Runtime/Function/Project/JzProjectConfig.h"
#include "JzRE/Runtime/Function/Scene/JzSceneSerializer.h"

namespace JzRE {

namespace {

constexpr const char *ScenesDirectory = "Scenes";
constexpr const char *DefaultSceneName = "main.jzscene";

} // namespace

std::filesystem::path JzDefaultSceneCreator::GetDefaultScenePath(const JzProjectConfig &config)
{
    auto scenesPath = config.rootPath / config.contentRoot / ScenesDirectory;
    return scenesPath / DefaultSceneName;
}

std::filesystem::path JzDefaultSceneCreator::CreateDefaultScene(const JzProjectConfig &config)
{
    auto scenesPath = config.rootPath / config.contentRoot / ScenesDirectory;

    // Create Scenes directory if it doesn't exist
    std::error_code ec;
    if (!std::filesystem::exists(scenesPath, ec)) {
        if (!std::filesystem::create_directories(scenesPath, ec)) {
            JzRE_LOG_ERROR("JzDefaultSceneCreator: Failed to create scenes directory: {}", ec.message());
            return {};
        }
    }

    auto scenePath = scenesPath / DefaultSceneName;

    // Create a temporary world and add default entities
    // Note: We can't use JzWorld directly here as it requires proper initialization
    // Instead, we'll create a minimal scene file directly in JSON format

    // For now, create an empty scene that will be populated by the runtime's default entities
    // The runtime already creates default camera and light entities in PreloadAssets()
    // before loading the scene, so this scene file can contain additional user entities

    // Create minimal scene JSON
    nlohmann::json sceneJson;
    sceneJson["version"]   = JzSceneSerializer::SCENE_VERSION;
    sceneJson["entities"]  = nlohmann::json::array();

    // Write the scene file
    std::ofstream file(scenePath);
    if (!file.is_open()) {
        JzRE_LOG_ERROR("JzDefaultSceneCreator: Failed to create scene file: {}", scenePath.string());
        return {};
    }

    file << sceneJson.dump(2);
    file.close();

    JzRE_LOG_INFO("JzDefaultSceneCreator: Created default scene at {}", scenePath.string());
    return scenePath;
}

} // namespace JzRE
