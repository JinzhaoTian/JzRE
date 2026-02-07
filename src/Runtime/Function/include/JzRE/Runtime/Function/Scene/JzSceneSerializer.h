/**
 * @file    JzSceneSerializer.h
 * @brief   Scene serialization and deserialization
 * @author  Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <filesystem>
#include <vector>

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Function/ECS/JzEntity.h"

namespace JzRE {

// Forward declarations
class JzWorld;

/**
 * @brief Scene serialization and deserialization utility
 *
 * Provides functionality to save and load scenes in JSON format (.jzscene).
 * Handles entity serialization including:
 * - Transform components (position, rotation, scale)
 * - Asset path references (model, material, shader)
 * - Name and UUID components
 * - Common game components (camera, light, etc.)
 */
class JzSceneSerializer {
public:
    /**
     * @brief Serialize all entities in the world to a JSON file
     *
     * @param world The ECS world to serialize
     * @param filepath Path to save the .jzscene file
     * @return True if serialization succeeded
     */
    static Bool Serialize(JzWorld &world, const std::filesystem::path &filepath);

    /**
     * @brief Deserialize a scene file and create entities in the world
     *
     * @param world The ECS world to populate
     * @param filepath Path to the .jzscene file
     * @return True if deserialization succeeded
     */
    static Bool Deserialize(JzWorld &world, const std::filesystem::path &filepath);

    /**
     * @brief Clear all user-created entities from the world
     *
     * Removes all entities except system entities (window, camera, lights).
     * Call before loading a new scene.
     *
     * @param world The ECS world to clear
     */
    static void ClearScene(JzWorld &world);

    /**
     * @brief Get the list of serializable entities
     *
     * Returns entities that have JzNameComponent and should be saved.
     * Excludes system entities like window config.
     *
     * @param world The ECS world to query
     * @return Vector of serializable entity IDs
     */
    static std::vector<JzEntity> GetSerializableEntities(JzWorld &world);

    /**
     * @brief Scene file extension
     */
    static constexpr const char *SCENE_EXTENSION = ".jzscene";

    /**
     * @brief Current scene format version
     */
    static constexpr U32 SCENE_VERSION = 1;
};

} // namespace JzRE
