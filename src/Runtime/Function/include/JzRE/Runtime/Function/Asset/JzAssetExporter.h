/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <filesystem>
#include <vector>

#include "JzRE/Runtime/Core/JzRETypes.h"

namespace JzRE {

/**
 * @brief Result of an export operation.
 */
enum class JzEExportResult : U8 {
    Success,
    NoProjectLoaded,
    SourceNotFound,
    DestinationError,
    CopyFailed
};

/**
 * @brief Options controlling export behavior.
 */
struct JzExportOptions {
    Bool overwriteExisting  = false; ///< Overwrite if file already exists at destination
    Bool preserveSubfolders = true;  ///< Maintain Content/ subfolder structure at destination
};

/**
 * @brief Result detail for a single file export.
 */
struct JzExportResultEntry {
    std::filesystem::path sourcePath;
    std::filesystem::path destinationPath;
    JzEExportResult       result{JzEExportResult::CopyFailed};
    String                errorMessage;
};

/**
 * @brief Service that exports project assets to an external directory.
 *
 * Responsibilities:
 * - Copy asset files from Content/ to an external target directory
 * - Optionally preserve the subfolder structure (Textures/, Models/, etc.)
 * - Handle single or batch file exports
 *
 * Retrieved via JzServiceContainer::Get<JzAssetExporter>().
 */
class JzAssetExporter {
public:
    JzAssetExporter()  = default;
    ~JzAssetExporter() = default;

    /**
     * @brief Export a single asset file to an external directory.
     * @param assetPath Absolute path to the asset within the project
     * @param destinationDir Target directory to export to
     * @param options Export options
     * @return Result entry
     */
    JzExportResultEntry ExportFile(const std::filesystem::path& assetPath,
                                    const std::filesystem::path& destinationDir,
                                    const JzExportOptions& options = {});

    /**
     * @brief Export multiple asset files to an external directory.
     * @param assetPaths List of absolute asset paths
     * @param destinationDir Target directory to export to
     * @param options Export options
     * @return Vector of result entries
     */
    std::vector<JzExportResultEntry> ExportFiles(
        const std::vector<std::filesystem::path>& assetPaths,
        const std::filesystem::path& destinationDir,
        const JzExportOptions& options = {});

private:
    /**
     * @brief Resolve the destination path for an asset export.
     */
    std::filesystem::path ResolveExportDestination(
        const std::filesystem::path& assetPath,
        const std::filesystem::path& destinationDir,
        const JzExportOptions& options);
};

} // namespace JzRE
