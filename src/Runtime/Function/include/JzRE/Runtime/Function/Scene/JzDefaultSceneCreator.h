/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#pragma once

#include <filesystem>

namespace JzRE {

struct JzProjectConfig;

/**
 * @brief Default scene creation utility
 *
 * Creates a default scene with basic entities (camera, light)
 * when no scene file is specified in the project configuration.
 */
class JzDefaultSceneCreator {
public:
    /**
     * @brief Create a default scene file in the project's Scenes directory
     *
     * @param config The project configuration
     * @return Path to the created scene file, or empty path on failure
     */
    static std::filesystem::path CreateDefaultScene(const JzProjectConfig &config);

    /**
     * @brief Get the default scene path for a project
     *
     * @param config The project configuration
     * @return Path to the default scene file
     */
    static std::filesystem::path GetDefaultScenePath(const JzProjectConfig &config);
};

} // namespace JzRE
