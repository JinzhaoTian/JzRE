/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Function/Asset/JzAssetImporter.h"

#include <algorithm>
#include <fstream>

#include "JzRE/Runtime/Core/JzLogger.h"
#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Runtime/Function/Project/JzProjectManager.h"

JzRE::JzImportResultEntry JzRE::JzAssetImporter::ImportFile(
    const std::filesystem::path &sourcePath,
    const JzImportOptions       &options)
{
    JzImportResultEntry entry;
    entry.sourcePath = sourcePath;

    // Check project is loaded
    if (!JzServiceContainer::Has<JzProjectManager>() || !JzServiceContainer::Get<JzProjectManager>().HasLoadedProject()) {
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
    auto            copyOptions = std::filesystem::copy_options::none;
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
    auto &projectManager = JzServiceContainer::Get<JzProjectManager>();
    projectManager.MarkDirty();

    entry.result = JzEImportResult::Success;
    JzRE_LOG_INFO("Imported asset: {} -> {}", sourcePath.string(), entry.destinationPath.string());
    return entry;
}

std::vector<JzRE::JzImportResultEntry> JzRE::JzAssetImporter::ImportFiles(
    const std::vector<std::filesystem::path> &sourcePaths,
    const JzImportOptions                    &options)
{
    std::vector<JzImportResultEntry> results;
    results.reserve(sourcePaths.size());

    for (const auto &path : sourcePaths) {
        results.push_back(ImportFile(path, options));
    }

    return results;
}

JzRE::String JzRE::JzAssetImporter::GetSubdirectoryForType(JzEFileType fileType)
{
    switch (fileType) {
        case JzEFileType::MODEL: return "Models";
        case JzEFileType::TEXTURE: return "Textures";
        case JzEFileType::SHADER: return "Shaders";
        case JzEFileType::SHADER_PART: return "Shaders/src";
        case JzEFileType::MATERIAL: return "Materials";
        case JzEFileType::FONT: return "Fonts";
        case JzEFileType::SOUND: return "Sounds";
        default: return "";
    }
}

std::vector<std::pair<JzRE::String, JzRE::String>> JzRE::JzAssetImporter::GetSupportedFileFilters()
{
    return {
        {"Model Files", "*.fbx;*.obj"},
        {"Texture Files", "*.png;*.jpg;*.jpeg;*.tga;*.hdr"},
        {"Shader Files", "*.jzshader;*.jzsblob;*.jzshader.src.json;*.hlsl;*.hlsli"},
        {"Material Files", "*.ovmat"},
        {"Font Files", "*.ttf"},
        {"Sound Files", "*.wav;*.mp3;*.ogg"}};
}

std::filesystem::path JzRE::JzAssetImporter::ResolveDestination(
    const std::filesystem::path &sourcePath,
    const JzImportOptions       &options)
{
    auto &projectManager = JzServiceContainer::Get<JzProjectManager>();
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

JzRE::Bool JzRE::JzAssetImporter::EnsureDirectoryExists(const std::filesystem::path &dirPath)
{
    if (std::filesystem::exists(dirPath)) {
        return true;
    }

    std::error_code ec;
    std::filesystem::create_directories(dirPath, ec);
    return !ec;
}

// ==================== Model Import with Dependencies ====================

JzRE::JzModelImportResult JzRE::JzAssetImporter::ImportModelWithDependencies(
    const std::filesystem::path &modelPath,
    const JzImportOptions       &options)
{
    JzModelImportResult result;

    // Validate prerequisites
    if (!JzServiceContainer::Has<JzProjectManager>() || !JzServiceContainer::Get<JzProjectManager>().HasLoadedProject()) {
        result.modelEntry.sourcePath   = modelPath;
        result.modelEntry.result       = JzEImportResult::NoProjectLoaded;
        result.modelEntry.errorMessage = "No project is currently loaded";
        return result;
    }

    if (!std::filesystem::exists(modelPath)) {
        result.modelEntry.sourcePath   = modelPath;
        result.modelEntry.result       = JzEImportResult::SourceNotFound;
        result.modelEntry.errorMessage = "Source file not found: " + modelPath.string();
        return result;
    }

    // Destination subfolder: Content/Models/<stem>/
    auto modelStem     = modelPath.stem().string();
    auto destSubfolder = "Models/" + modelStem;

    // Import the model file itself
    JzImportOptions modelOptions     = options;
    modelOptions.autoDetectSubfolder = false;
    modelOptions.targetSubfolder     = destSubfolder;
    result.modelEntry                = ImportFile(modelPath, modelOptions);

    if (result.modelEntry.result != JzEImportResult::Success) {
        return result;
    }

    // Discover and import dependencies (.mtl, textures)
    auto dependencies   = DiscoverModelDependencies(modelPath);
    result.allSucceeded = true;

    for (const auto &depPath : dependencies) {
        JzImportOptions depOptions     = options;
        depOptions.autoDetectSubfolder = false;
        depOptions.targetSubfolder     = destSubfolder;

        auto depResult = ImportFile(depPath, depOptions);
        result.dependencyEntries.push_back(depResult);

        if (depResult.result != JzEImportResult::Success) {
            result.allSucceeded = false;
            JzRE_LOG_WARN("Failed to import dependency: {} - {}",
                          depPath.string(), depResult.errorMessage);
        }
    }

    if (result.allSucceeded) {
        JzRE_LOG_INFO("Imported model with {} dependencies: {}",
                      dependencies.size(), modelPath.filename().string());
    }

    return result;
}

std::vector<std::filesystem::path> JzRE::JzAssetImporter::DiscoverModelDependencies(
    const std::filesystem::path &modelPath)
{
    std::vector<std::filesystem::path> dependencies;
    auto                               modelDir = modelPath.parent_path();
    auto                               ext      = modelPath.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == ".obj") {
        // Parse OBJ for mtllib references
        auto mtlLibs = ParseObjMtlLibs(modelPath);
        for (const auto &mtlName : mtlLibs) {
            auto mtlPath = modelDir / mtlName;
            if (std::filesystem::exists(mtlPath)) {
                dependencies.push_back(mtlPath);

                // Parse each MTL for texture references
                auto textures = ParseMtlTextures(mtlPath);
                for (const auto &texName : textures) {
                    auto texPath = modelDir / texName;
                    if (std::filesystem::exists(texPath)) {
                        dependencies.push_back(texPath);
                    } else {
                        JzRE_LOG_WARN("Model dependency not found: {}", texPath.string());
                    }
                }
            } else {
                JzRE_LOG_WARN("MTL file not found: {}", mtlPath.string());
            }
        }
    }

    return dependencies;
}

std::vector<JzRE::String> JzRE::JzAssetImporter::ParseObjMtlLibs(
    const std::filesystem::path &objPath)
{
    std::vector<String> mtlLibs;
    std::ifstream       file(objPath);
    if (!file.is_open()) return mtlLibs;

    String line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        // Look for "mtllib <filename>"
        if (line.size() > 7 && line.substr(0, 7) == "mtllib ") {
            String mtlName = line.substr(7);
            // Trim trailing whitespace
            while (!mtlName.empty() && (mtlName.back() == ' ' || mtlName.back() == '\r' || mtlName.back() == '\n')) {
                mtlName.pop_back();
            }
            if (!mtlName.empty()) {
                mtlLibs.push_back(mtlName);
            }
        }
    }
    return mtlLibs;
}

std::vector<JzRE::String> JzRE::JzAssetImporter::ParseMtlTextures(
    const std::filesystem::path &mtlPath)
{
    std::vector<String> textures;
    std::ifstream       file(mtlPath);
    if (!file.is_open()) return textures;

    static const std::vector<String> mapDirectives = {
        "map_Kd", "map_Ks", "map_Ka", "map_Bump", "map_d",
        "map_Ns", "map_Ke", "bump", "disp", "decal", "refl"};

    String line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        for (const auto &directive : mapDirectives) {
            if (line.size() > directive.size() && line.substr(0, directive.size()) == directive && line[directive.size()] == ' ') {
                String textureName = line.substr(directive.size() + 1);
                // Trim trailing whitespace
                while (!textureName.empty() && (textureName.back() == ' ' || textureName.back() == '\r' || textureName.back() == '\n')) {
                    textureName.pop_back();
                }
                // Handle MTL options before texture path (e.g., "-s 1 1 1 texture.png")
                // The texture path is always the last token
                auto lastSpace = textureName.find_last_of(' ');
                if (lastSpace != String::npos) {
                    String candidate = textureName.substr(lastSpace + 1);
                    if (candidate.find('.') != String::npos) {
                        textureName = candidate;
                    }
                }
                if (!textureName.empty()) {
                    // Avoid duplicates
                    if (std::find(textures.begin(), textures.end(), textureName) == textures.end()) {
                        textures.push_back(textureName);
                    }
                }
                break;
            }
        }
    }
    return textures;
}
