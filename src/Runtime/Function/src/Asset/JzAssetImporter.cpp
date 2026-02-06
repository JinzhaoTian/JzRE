/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Function/Asset/JzAssetImporter.h"

#include "JzRE/Runtime/Core/JzLogger.h"
#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Runtime/Function/Project/JzProjectManager.h"

JzRE::JzImportResultEntry JzRE::JzAssetImporter::ImportFile(
    const std::filesystem::path& sourcePath,
    const JzImportOptions& options)
{
    JzImportResultEntry entry;
    entry.sourcePath = sourcePath;

    // Check project is loaded
    if (!JzServiceContainer::Has<JzProjectManager>() ||
        !JzServiceContainer::Get<JzProjectManager>().HasLoadedProject()) {
        entry.result       = JzEImportResult::NoProjectLoaded;
        entry.errorMessage = "No project is currently loaded";
        return entry;
    }

    // Check source exists
    if (!std::filesystem::exists(sourcePath)) {
        entry.result       = JzEImportResult::SourceNotFound;
        entry.errorMessage = "Source file not found: " + sourcePath.string();
        return entry;
    }

    // Resolve destination
    entry.destinationPath = ResolveDestination(sourcePath, options);

    // Check for overwrite
    if (std::filesystem::exists(entry.destinationPath) && !options.overwriteExisting) {
        entry.result       = JzEImportResult::DestinationExists;
        entry.errorMessage = "File already exists: " + entry.destinationPath.string();
        return entry;
    }

    // Ensure directory exists
    if (!EnsureDirectoryExists(entry.destinationPath.parent_path())) {
        entry.result       = JzEImportResult::CopyFailed;
        entry.errorMessage = "Failed to create directory: " + entry.destinationPath.parent_path().string();
        return entry;
    }

    // Copy file
    std::error_code ec;
    auto copyOptions = std::filesystem::copy_options::none;
    if (options.overwriteExisting) {
        copyOptions = std::filesystem::copy_options::overwrite_existing;
    }
    std::filesystem::copy_file(sourcePath, entry.destinationPath, copyOptions, ec);
    if (ec) {
        entry.result       = JzEImportResult::CopyFailed;
        entry.errorMessage = ec.message();
        return entry;
    }

    // Mark project dirty
    auto& projectManager = JzServiceContainer::Get<JzProjectManager>();
    projectManager.MarkDirty();

    entry.result = JzEImportResult::Success;
    JzRE_LOG_INFO("Imported asset: {} -> {}", sourcePath.string(), entry.destinationPath.string());
    return entry;
}

std::vector<JzRE::JzImportResultEntry> JzRE::JzAssetImporter::ImportFiles(
    const std::vector<std::filesystem::path>& sourcePaths,
    const JzImportOptions& options)
{
    std::vector<JzImportResultEntry> results;
    results.reserve(sourcePaths.size());

    for (const auto& path : sourcePaths) {
        results.push_back(ImportFile(path, options));
    }

    return results;
}

JzRE::String JzRE::JzAssetImporter::GetSubdirectoryForType(JzEFileType fileType)
{
    switch (fileType) {
        case JzEFileType::MODEL:       return "Models";
        case JzEFileType::TEXTURE:     return "Textures";
        case JzEFileType::SHADER:
        case JzEFileType::SHADER_PART: return "Shaders";
        case JzEFileType::MATERIAL:    return "Materials";
        case JzEFileType::FONT:        return "Fonts";
        case JzEFileType::SOUND:       return "Sounds";
        default:                       return "";
    }
}

std::vector<std::pair<JzRE::String, JzRE::String>> JzRE::JzAssetImporter::GetSupportedFileFilters()
{
    return {
        {"Model Files",    "*.fbx;*.obj"},
        {"Texture Files",  "*.png;*.jpg;*.jpeg;*.tga;*.hdr"},
        {"Shader Files",   "*.ovfx;*.ovfxh"},
        {"Material Files", "*.ovmat"},
        {"Font Files",     "*.ttf"},
        {"Sound Files",    "*.wav;*.mp3;*.ogg"}
    };
}

std::filesystem::path JzRE::JzAssetImporter::ResolveDestination(
    const std::filesystem::path& sourcePath,
    const JzImportOptions& options)
{
    auto& projectManager = JzServiceContainer::Get<JzProjectManager>();
    auto  contentPath    = projectManager.GetConfig().GetContentPath();

    String subfolder;
    if (!options.targetSubfolder.empty()) {
        subfolder = options.targetSubfolder;
    } else if (options.autoDetectSubfolder) {
        auto fileType = JzFileSystemUtils::GetFileType(sourcePath.string());
        subfolder     = GetSubdirectoryForType(fileType);
    }

    auto destDir = subfolder.empty() ? contentPath : contentPath / subfolder;
    return destDir / sourcePath.filename();
}

JzRE::Bool JzRE::JzAssetImporter::EnsureDirectoryExists(const std::filesystem::path& dirPath)
{
    if (std::filesystem::exists(dirPath)) {
        return true;
    }

    std::error_code ec;
    std::filesystem::create_directories(dirPath, ec);
    return !ec;
}
