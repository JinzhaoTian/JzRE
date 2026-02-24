/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Core/JzRETypes.h"

#include <filesystem>
#include <vector>

namespace JzRE {

/**
 * @brief Startup mode for the project.
 */
enum class JzEStartupMode : U8 {
    Authoring, // Launch in authoring-tools mode
    Runtime    // Launch in standalone runtime mode
};

/**
 * @brief Graphics API selection.
 */
enum class JzERenderAPI : U8 {
    Auto,   // Automatically select best available
    OpenGL, // OpenGL backend
    Vulkan, // Vulkan backend
    D3D12,  // Direct3D 12 backend
    Metal   // Metal backend
};

/**
 * @brief Target platform identifier.
 */
enum class JzETargetPlatform : U8 {
    Windows,
    Linux,
    MacOS,
    Android,
    iOS,
    Web
};

/**
 * @brief Plugin configuration entry.
 */
struct JzPluginEntry {
    String name;
    String version;
    Bool   enabled{true};
};

/**
 * @brief Asset import rule mapping file extension to factory.
 */
struct JzImportRule {
    String extension; // File extension (e.g., ".png", ".fbx")
    String factory;   // Factory name to use for import
};

/**
 * @brief Core project configuration.
 *
 * Contains runtime-essential fields for project initialization.
 * This is the data structure that gets serialized to/from .jzreproject files.
 */
struct JzProjectConfig {
    // === Runtime Essential ===

    String projectName;   // Human-readable project name
    String projectId;     // Stable unique identifier (UUID)
    String engineVersion; // Compatible engine version (e.g., "1.0.0")

    std::filesystem::path rootPath;               // Project root directory
    std::filesystem::path contentRoot{"Content"}; // Asset root relative to rootPath
    std::filesystem::path configRoot{"Config"};   // Config directory relative to rootPath

    String         defaultScene; // Default scene to load on startup
    JzEStartupMode startupMode{JzEStartupMode::Authoring};
    JzERenderAPI   renderAPI{JzERenderAPI::Auto};

    std::vector<JzETargetPlatform> targetPlatforms;

    // === Resources & Build ===

    std::filesystem::path assetRegistry{"AssetRegistry.json"};
    std::filesystem::path shaderCache{"Intermediate/ShaderCache"};
    std::filesystem::path shaderSourceRoot{"Content/Shaders/src"};
    std::filesystem::path shaderCookedRoot{"Content/Shaders"};
    Bool                  shaderAutoCook{true};
    std::filesystem::path buildOutput{"Build"};

    std::vector<JzImportRule> importRules;

    // === Modules & Plugins ===

    std::vector<String>                modules;
    std::vector<JzPluginEntry>         plugins;
    std::vector<std::filesystem::path> pluginSearchPaths;

    // === Version & Migration ===

    U32    projectVersion{1};             // Project file format version
    String minCompatibleVersion{"1.0.0"}; // Minimum compatible engine version

    // === Utility Methods ===

    /**
     * @brief Get the absolute content directory path.
     */
    [[nodiscard]] std::filesystem::path GetContentPath() const
    {
        return rootPath / contentRoot;
    }

    /**
     * @brief Get the absolute config directory path.
     */
    [[nodiscard]] std::filesystem::path GetConfigPath() const
    {
        return rootPath / configRoot;
    }

    /**
     * @brief Get the absolute asset registry path.
     */
    [[nodiscard]] std::filesystem::path GetAssetRegistryPath() const
    {
        return rootPath / assetRegistry;
    }

    /**
     * @brief Get the absolute shader cache path.
     */
    [[nodiscard]] std::filesystem::path GetShaderCachePath() const
    {
        return rootPath / shaderCache;
    }

    /**
     * @brief Get the absolute shader source directory path.
     */
    [[nodiscard]] std::filesystem::path GetShaderSourcePath() const
    {
        return rootPath / shaderSourceRoot;
    }

    /**
     * @brief Get the absolute shader cooked output directory path.
     */
    [[nodiscard]] std::filesystem::path GetShaderCookedPath() const
    {
        return rootPath / shaderCookedRoot;
    }

    /**
     * @brief Get the absolute build output path.
     */
    [[nodiscard]] std::filesystem::path GetBuildOutputPath() const
    {
        return rootPath / buildOutput;
    }

    /**
     * @brief Check if a target platform is supported.
     */
    [[nodiscard]] Bool SupportsTargetPlatform(JzETargetPlatform platform) const
    {
        for (const auto &p : targetPlatforms) {
            if (p == platform) return true;
        }
        return false;
    }
};

/**
 * @brief Workspace-level project settings for host tooling.
 *
 * Stored separately from runtime config to keep runtime lightweight while
 * allowing host applications to persist layout and recent scene state.
 * Typically saved as ProjectName.workspace alongside ProjectName.jzreproject.
 */
struct JzProjectWorkspaceSettings {
    std::filesystem::path workspaceLayout;       // Host workspace layout file
    std::vector<String>   recentScenes;          // Recently opened scene paths
    std::filesystem::path workspaceSettingsFile; // Host workspace preferences file

    // Last camera position for each scene (scene path -> camera state)
    // Can be expanded as needed
};

} // namespace JzRE
