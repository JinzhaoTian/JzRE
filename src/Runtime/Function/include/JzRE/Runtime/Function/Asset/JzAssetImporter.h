/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <filesystem>
#include <vector>

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzFileSystemUtils.h"

namespace JzRE {

/**
 * @brief Result of an import operation.
 */
enum class JzEImportResult : U8 {
    Success,
    NoProjectLoaded,
    SourceNotFound,
    DestinationExists,
    CopyFailed,
    UnsupportedFileType
};

/**
 * @brief Options controlling how a single import operation behaves.
 */
struct JzImportOptions {
    Bool   overwriteExisting   = false; ///< Overwrite if file already exists in Content/
    Bool   autoDetectSubfolder = true;  ///< Route to Textures/, Models/, etc. based on type
    String targetSubfolder;             ///< Manual override subfolder (e.g. "Textures/Characters")
};

/**
 * @brief Result detail for a single file import.
 */
struct JzImportResultEntry {
    std::filesystem::path sourcePath;
    std::filesystem::path destinationPath;
    JzEImportResult       result{JzEImportResult::CopyFailed};
    String                errorMessage;
};

/**
 * @brief Result of importing a model with its dependencies.
 */
struct JzModelImportResult {
    JzImportResultEntry              modelEntry;          ///< The main model file result
    std::vector<JzImportResultEntry> dependencyEntries;   ///< MTL, textures, etc.
    Bool                             allSucceeded{false}; ///< True if every file imported OK
};

/**
 * @brief Service that imports external asset files into the project Content directory.
 *
 * Responsibilities:
 * - Copy files from an external source path into Content/<SubType>/
 * - Determine the correct subdirectory based on JzFileSystemUtils::GetFileType()
 * - Ensure target directories exist (create if missing)
 * - Notify the JzProjectManager that the project is dirty
 *
 * Retrieved via JzServiceContainer::Get<JzAssetImporter>().
 */
class JzAssetImporter {
public:
    JzAssetImporter()  = default;
    ~JzAssetImporter() = default;

    /**
     * @brief Import a single file into the project.
     * @param sourcePath Absolute path to the external file
     * @param options Import options
     * @return Result entry describing what happened
     */
    JzImportResultEntry ImportFile(const std::filesystem::path &sourcePath,
                                   const JzImportOptions       &options = {});

    /**
     * @brief Import multiple files into the project.
     * @param sourcePaths List of absolute paths to external files
     * @param options Import options (applied to all files)
     * @return Vector of result entries, one per input file
     */
    std::vector<JzImportResultEntry> ImportFiles(
        const std::vector<std::filesystem::path> &sourcePaths,
        const JzImportOptions                    &options = {});

    /**
     * @brief Import a model file along with all its dependencies (MTL, textures).
     *
     * The model and its dependencies are placed in a subfolder:
     *   Content/Models/<stem>/
     * where <stem> is the model filename without extension.
     *
     * @param modelPath   Absolute path to the model file (.obj, .fbx)
     * @param options     Import options
     * @return JzModelImportResult with details for each copied file
     */
    JzModelImportResult ImportModelWithDependencies(
        const std::filesystem::path &modelPath,
        const JzImportOptions       &options = {});

    /**
     * @brief Get the target subdirectory for a file type.
     *
     * Maps JzEFileType to the conventional subdirectory name:
     *   MODEL       -> "Models"
     *   TEXTURE     -> "Textures"
     *   SHADER      -> "Shaders" (`.jzshader`)
     *   SHADER_PART -> "Shaders/src" (`.jzsblob`, `.jzshader.src.json`, `.hlsl`, `.hlsli`)
     *   MATERIAL    -> "Materials"
     *   FONT        -> "Fonts"
     *   SOUND       -> "Sounds"
     *   others      -> "" (root of Content/)
     */
    static String GetSubdirectoryForType(JzEFileType fileType);

    /**
     * @brief Get the list of supported import file extensions as filter pairs
     *        suitable for JzOpenFileDialog::AddFileType().
     * @return Vector of {label, filter} pairs
     */
    static std::vector<std::pair<String, String>> GetSupportedFileFilters();

private:
    /**
     * @brief Resolve the destination path within Content/ for a source file.
     */
    std::filesystem::path ResolveDestination(const std::filesystem::path &sourcePath,
                                             const JzImportOptions       &options);

    /**
     * @brief Ensure the target directory exists, creating it if necessary.
     */
    static Bool EnsureDirectoryExists(const std::filesystem::path &dirPath);

    /**
     * @brief Discover all files referenced by a model file.
     *
     * For .obj files, parses the mtllib directive to find .mtl files,
     * then parses .mtl files for texture map directives.
     *
     * @param modelPath  Absolute path to the model file
     * @return Vector of absolute paths to dependency files that exist on disk
     */
    std::vector<std::filesystem::path> DiscoverModelDependencies(
        const std::filesystem::path &modelPath);

    /**
     * @brief Parse an OBJ file to extract mtllib references.
     */
    static std::vector<String> ParseObjMtlLibs(const std::filesystem::path &objPath);

    /**
     * @brief Parse an MTL file to extract texture map references.
     */
    static std::vector<String> ParseMtlTextures(const std::filesystem::path &mtlPath);
};

} // namespace JzRE
