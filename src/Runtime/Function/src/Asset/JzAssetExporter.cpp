/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Function/Asset/JzAssetExporter.h"

#include "JzRE/Runtime/Core/JzLogger.h"
#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Runtime/Function/Project/JzProjectManager.h"

JzRE::JzExportResultEntry JzRE::JzAssetExporter::ExportFile(
    const std::filesystem::path& assetPath,
    const std::filesystem::path& destinationDir,
    const JzExportOptions& options)
{
    JzExportResultEntry entry;
    entry.sourcePath = assetPath;

    // Check project is loaded
    if (!JzServiceContainer::Has<JzProjectManager>() ||
        !JzServiceContainer::Get<JzProjectManager>().HasLoadedProject()) {
        entry.result       = JzEExportResult::NoProjectLoaded;
        entry.errorMessage = "No project is currently loaded";
        return entry;
    }

    // Check source exists
    if (!std::filesystem::exists(assetPath)) {
        entry.result       = JzEExportResult::SourceNotFound;
        entry.errorMessage = "Asset file not found: " + assetPath.string();
        return entry;
    }

    // Resolve destination
    entry.destinationPath = ResolveExportDestination(assetPath, destinationDir, options);

    // Check for overwrite
    if (std::filesystem::exists(entry.destinationPath) && !options.overwriteExisting) {
        entry.result       = JzEExportResult::DestinationError;
        entry.errorMessage = "File already exists: " + entry.destinationPath.string();
        return entry;
    }

    // Ensure destination directory exists
    std::error_code ec;
    std::filesystem::create_directories(entry.destinationPath.parent_path(), ec);
    if (ec) {
        entry.result       = JzEExportResult::DestinationError;
        entry.errorMessage = "Failed to create directory: " + ec.message();
        return entry;
    }

    // Copy file
    auto copyOptions = std::filesystem::copy_options::none;
    if (options.overwriteExisting) {
        copyOptions = std::filesystem::copy_options::overwrite_existing;
    }
    std::filesystem::copy_file(assetPath, entry.destinationPath, copyOptions, ec);
    if (ec) {
        entry.result       = JzEExportResult::CopyFailed;
        entry.errorMessage = ec.message();
        return entry;
    }

    entry.result = JzEExportResult::Success;
    JzRE_LOG_INFO("Exported asset: {} -> {}", assetPath.string(), entry.destinationPath.string());
    return entry;
}

std::vector<JzRE::JzExportResultEntry> JzRE::JzAssetExporter::ExportFiles(
    const std::vector<std::filesystem::path>& assetPaths,
    const std::filesystem::path& destinationDir,
    const JzExportOptions& options)
{
    std::vector<JzExportResultEntry> results;
    results.reserve(assetPaths.size());

    for (const auto& path : assetPaths) {
        results.push_back(ExportFile(path, destinationDir, options));
    }

    return results;
}

std::filesystem::path JzRE::JzAssetExporter::ResolveExportDestination(
    const std::filesystem::path& assetPath,
    const std::filesystem::path& destinationDir,
    const JzExportOptions& options)
{
    if (options.preserveSubfolders) {
        auto& projectManager = JzServiceContainer::Get<JzProjectManager>();
        auto  contentPath    = projectManager.GetConfig().GetContentPath();

        // Compute relative path from Content directory
        std::error_code ec;
        auto relativePath = std::filesystem::relative(assetPath, contentPath, ec);

        if (!ec && !relativePath.empty() && relativePath.string() != ".") {
            return destinationDir / relativePath;
        }
    }

    // Fall back to flat copy (just filename)
    return destinationDir / assetPath.filename();
}
