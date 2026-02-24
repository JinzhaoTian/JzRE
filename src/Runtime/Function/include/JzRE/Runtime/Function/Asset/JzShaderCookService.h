/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#pragma once

#include <filesystem>
#include <unordered_map>
#include <vector>

#include "JzRE/Runtime/Core/JzRETypes.h"

namespace JzRE {

class JzAssetSystem;

/**
 * @brief Configuration for shader source auto-cook service.
 */
struct JzShaderCookServiceConfig {
    std::filesystem::path sourceRoot;                      ///< Shader source root directory.
    std::filesystem::path outputRoot;                      ///< Cooked shader output directory.
    std::filesystem::path shaderToolPath;                  ///< Optional explicit shader tool path.
    F32                   scanIntervalSeconds = 0.5f;      ///< Polling interval.
};

/**
 * @brief Polling-based auto-cook bridge from shader source to cooked runtime artifacts.
 *
 * The service watches source files under `sourceRoot` and invokes
 * `JzREShaderTool --input <manifest> --output-dir <outputRoot>` when
 * changes are detected. After successful cooking it requests an immediate
 * shader hot-reload pass through `JzAssetSystem`.
 */
class JzShaderCookService {
public:
    /**
     * @brief Construct service with configuration.
     *
     * @param config Auto-cook configuration.
     */
    explicit JzShaderCookService(const JzShaderCookServiceConfig &config);

    /**
     * @brief Initialize service state and resolve tool path.
     *
     * @return true when service is ready to run.
     */
    Bool Initialize();

    /**
     * @brief Shutdown service and clear cached timestamps.
     */
    void Shutdown();

    /**
     * @brief Poll source changes and trigger incremental cooking.
     *
     * @param deltaSeconds Frame delta time in seconds.
     * @param assetSystem Asset system used to trigger hot reload checks.
     */
    void Update(F32 deltaSeconds, JzAssetSystem &assetSystem);

    /**
     * @brief Check whether the service is initialized.
     *
     * @return true if service is initialized.
     */
    [[nodiscard]] Bool IsInitialized() const;

private:
    Bool ScanShaderManifests(std::vector<std::filesystem::path> &outManifests) const;
    Bool ShouldRecookManifest(const std::filesystem::path &manifestPath);
    Bool CookManifest(const std::filesystem::path &manifestPath) const;
    std::filesystem::file_time_type ComputeManifestDependencyTimestamp(
        const std::filesystem::path &manifestPath) const;
    std::filesystem::path ResolveShaderToolPath() const;

private:
    JzShaderCookServiceConfig m_config;
    std::filesystem::path     m_resolvedShaderToolPath;
    std::unordered_map<String, std::filesystem::file_time_type> m_manifestTimestamps;
    F32  m_timeSinceLastScan = 0.0f;
    Bool m_initialized       = false;
};

} // namespace JzRE
