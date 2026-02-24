/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/CLI/commands/JzAssetCommand.h"

#include <format>
#include <sstream>
#include <unordered_set>

#include <nlohmann/json.hpp>

#include "JzRE/CLI/JzCliArgParser.h"

namespace JzRE {

namespace {

using Json = nlohmann::json;

const String kDomain = "asset";

String BuildHelp()
{
    std::ostringstream ss;
    ss << "asset commands:\n";
    ss << "  JzRE asset import --project <file.jzreproject> --src <file...> [--overwrite] [--subfolder <ContentSubDir>]\n";
    ss << "  JzRE asset import-model --project <file.jzreproject> --src <model.obj|fbx> [--overwrite]\n";
    ss << "  JzRE asset export --project <file.jzreproject> --src <file...> --out <dir> [--overwrite] [--flat]";
    return ss.str();
}

String ImportResultToString(JzEImportResult result)
{
    switch (result) {
        case JzEImportResult::Success: return "success";
        case JzEImportResult::NoProjectLoaded: return "no_project_loaded";
        case JzEImportResult::SourceNotFound: return "source_not_found";
        case JzEImportResult::DestinationExists: return "destination_exists";
        case JzEImportResult::CopyFailed: return "copy_failed";
        case JzEImportResult::UnsupportedFileType: return "unsupported_file_type";
    }
    return "unknown";
}

String ExportResultToString(JzEExportResult result)
{
    switch (result) {
        case JzEExportResult::Success: return "success";
        case JzEExportResult::NoProjectLoaded: return "no_project_loaded";
        case JzEExportResult::SourceNotFound: return "source_not_found";
        case JzEExportResult::DestinationError: return "destination_error";
        case JzEExportResult::CopyFailed: return "copy_failed";
    }
    return "unknown";
}

std::filesystem::path ResolveAbsolute(const String &path)
{
    std::filesystem::path result(path);
    if (result.is_relative()) {
        result = std::filesystem::current_path() / result;
    }
    return result.lexically_normal();
}

JzCliResult LoadProjectOrError(JzCliContext &context, const std::filesystem::path &projectPath)
{
    const auto loadResult = context.LoadProject(projectPath);
    if (loadResult != JzEProjectResult::Success) {
        return JzCliResult::Error(
            JzCliExitCode::ProjectError,
            std::format("Failed to load project '{}': {}",
                        projectPath.string(),
                        static_cast<int>(loadResult)));
    }
    return JzCliResult::Ok();
}

JzCliResult HandleImport(JzCliContext              &context,
                         const std::vector<String> &args,
                         JzCliOutputFormat          format)
{
    const std::unordered_set<String> flags = {"--overwrite"};
    auto parsed = JzCliArgParser::Parse(args, flags);

    auto *projectFile = parsed.GetFirstValue("--project");
    if (projectFile == nullptr || projectFile->empty()) {
        return JzCliResult::Error(JzCliExitCode::InvalidArguments,
                                  "Missing required option: --project");
    }

    const auto loadResult = LoadProjectOrError(context, ResolveAbsolute(*projectFile));
    if (!loadResult.IsSuccess()) {
        return loadResult;
    }

    const auto *srcFiles = parsed.GetOptionValues("--src");
    if (srcFiles == nullptr || srcFiles->empty()) {
        return JzCliResult::Error(JzCliExitCode::InvalidArguments,
                                  "Missing required option: --src");
    }

    JzImportOptions options;
    options.overwriteExisting = parsed.HasOption("--overwrite");
    if (auto *subfolder = parsed.GetFirstValue("--subfolder")) {
        options.autoDetectSubfolder = false;
        options.targetSubfolder     = *subfolder;
    }

    std::vector<std::filesystem::path> sourcePaths;
    sourcePaths.reserve(srcFiles->size());
    for (const auto &src : *srcFiles) {
        sourcePaths.push_back(ResolveAbsolute(src));
    }

    auto results = context.GetAssetImporter().ImportFiles(sourcePaths, options);

    Size successCount = 0;
    for (const auto &entry : results) {
        if (entry.result == JzEImportResult::Success) {
            ++successCount;
        }
    }

    if (format == JzCliOutputFormat::Json) {
        Json payload;
        payload["imported"] = successCount;
        payload["total"]    = results.size();
        payload["entries"]  = Json::array();
        for (const auto &entry : results) {
            payload["entries"].push_back({
                {"source", entry.sourcePath.string()},
                {"destination", entry.destinationPath.string()},
                {"result", ImportResultToString(entry.result)},
                {"error", entry.errorMessage},
            });
        }
        if (successCount != results.size()) {
            return JzCliResult::Error(JzCliExitCode::IoError, payload.dump(2));
        }
        return JzCliResult::Ok(payload.dump(2));
    }

    std::ostringstream ss;
    ss << "Imported " << successCount << "/" << results.size() << " assets";
    if (successCount != results.size()) {
        ss << "\n";
        for (const auto &entry : results) {
            if (entry.result != JzEImportResult::Success) {
                ss << "  - " << entry.sourcePath.string() << ": " << entry.errorMessage << "\n";
            }
        }
        return JzCliResult::Error(JzCliExitCode::IoError, ss.str());
    }

    return JzCliResult::Ok(ss.str());
}

JzCliResult HandleImportModel(JzCliContext              &context,
                              const std::vector<String> &args,
                              JzCliOutputFormat          format)
{
    const std::unordered_set<String> flags = {"--overwrite"};
    auto parsed = JzCliArgParser::Parse(args, flags);

    auto *projectFile = parsed.GetFirstValue("--project");
    if (projectFile == nullptr || projectFile->empty()) {
        return JzCliResult::Error(JzCliExitCode::InvalidArguments,
                                  "Missing required option: --project");
    }

    const auto loadResult = LoadProjectOrError(context, ResolveAbsolute(*projectFile));
    if (!loadResult.IsSuccess()) {
        return loadResult;
    }

    auto *src = parsed.GetFirstValue("--src");
    if (src == nullptr || src->empty()) {
        return JzCliResult::Error(JzCliExitCode::InvalidArguments,
                                  "Missing required option: --src");
    }

    JzImportOptions options;
    options.overwriteExisting = parsed.HasOption("--overwrite");

    auto result = context.GetAssetImporter().ImportModelWithDependencies(ResolveAbsolute(*src), options);

    Size dependencySuccess = 0;
    for (const auto &entry : result.dependencyEntries) {
        if (entry.result == JzEImportResult::Success) {
            ++dependencySuccess;
        }
    }

    if (format == JzCliOutputFormat::Json) {
        Json payload;
        payload["model"] = {
            {"source", result.modelEntry.sourcePath.string()},
            {"destination", result.modelEntry.destinationPath.string()},
            {"result", ImportResultToString(result.modelEntry.result)},
            {"error", result.modelEntry.errorMessage},
        };
        payload["dependencies_total"]   = result.dependencyEntries.size();
        payload["dependencies_success"] = dependencySuccess;
        payload["all_succeeded"]        = result.allSucceeded;

        if (result.modelEntry.result != JzEImportResult::Success || !result.allSucceeded) {
            return JzCliResult::Error(JzCliExitCode::IoError, payload.dump(2));
        }

        return JzCliResult::Ok(payload.dump(2));
    }

    std::ostringstream ss;
    ss << "Model import result: " << ImportResultToString(result.modelEntry.result) << "\n";
    ss << "Dependencies: " << dependencySuccess << "/" << result.dependencyEntries.size();

    if (result.modelEntry.result != JzEImportResult::Success || !result.allSucceeded) {
        return JzCliResult::Error(JzCliExitCode::IoError, ss.str());
    }

    return JzCliResult::Ok(ss.str());
}

JzCliResult HandleExport(JzCliContext              &context,
                         const std::vector<String> &args,
                         JzCliOutputFormat          format)
{
    const std::unordered_set<String> flags = {"--overwrite", "--flat"};
    auto parsed = JzCliArgParser::Parse(args, flags);

    auto *projectFile = parsed.GetFirstValue("--project");
    if (projectFile == nullptr || projectFile->empty()) {
        return JzCliResult::Error(JzCliExitCode::InvalidArguments,
                                  "Missing required option: --project");
    }

    const auto loadResult = LoadProjectOrError(context, ResolveAbsolute(*projectFile));
    if (!loadResult.IsSuccess()) {
        return loadResult;
    }

    auto *outDir = parsed.GetFirstValue("--out");
    if (outDir == nullptr || outDir->empty()) {
        return JzCliResult::Error(JzCliExitCode::InvalidArguments,
                                  "Missing required option: --out");
    }

    const auto *srcFiles = parsed.GetOptionValues("--src");
    if (srcFiles == nullptr || srcFiles->empty()) {
        return JzCliResult::Error(JzCliExitCode::InvalidArguments,
                                  "Missing required option: --src");
    }

    JzExportOptions options;
    options.overwriteExisting  = parsed.HasOption("--overwrite");
    options.preserveSubfolders = !parsed.HasOption("--flat");

    const auto contentPath = context.GetProjectManager().GetConfig().GetContentPath();

    std::vector<std::filesystem::path> assetPaths;
    assetPaths.reserve(srcFiles->size());
    for (const auto &src : *srcFiles) {
        std::filesystem::path assetPath(src);
        if (assetPath.is_relative()) {
            assetPath = contentPath / assetPath;
        }
        assetPaths.push_back(assetPath.lexically_normal());
    }

    auto results = context.GetAssetExporter().ExportFiles(assetPaths, ResolveAbsolute(*outDir), options);

    Size successCount = 0;
    for (const auto &entry : results) {
        if (entry.result == JzEExportResult::Success) {
            ++successCount;
        }
    }

    if (format == JzCliOutputFormat::Json) {
        Json payload;
        payload["exported"] = successCount;
        payload["total"]    = results.size();
        payload["entries"]  = Json::array();
        for (const auto &entry : results) {
            payload["entries"].push_back({
                {"source", entry.sourcePath.string()},
                {"destination", entry.destinationPath.string()},
                {"result", ExportResultToString(entry.result)},
                {"error", entry.errorMessage},
            });
        }
        if (successCount != results.size()) {
            return JzCliResult::Error(JzCliExitCode::IoError, payload.dump(2));
        }
        return JzCliResult::Ok(payload.dump(2));
    }

    std::ostringstream ss;
    ss << "Exported " << successCount << "/" << results.size() << " assets";
    if (successCount != results.size()) {
        return JzCliResult::Error(JzCliExitCode::IoError, ss.str());
    }

    return JzCliResult::Ok(ss.str());
}

} // namespace

const String &JzAssetCommand::GetDomain() const
{
    return kDomain;
}

JzCliResult JzAssetCommand::Execute(JzCliContext              &context,
                                    const std::vector<String> &args,
                                    JzCliOutputFormat          format)
{
    if (args.empty() || args.front() == "--help" || args.front() == "-h") {
        return JzCliResult::Ok(BuildHelp());
    }

    const auto subcommand = args.front();
    const std::vector<String> subArgs(args.begin() + 1, args.end());

    if (subcommand == "import") {
        return HandleImport(context, subArgs, format);
    }
    if (subcommand == "import-model") {
        return HandleImportModel(context, subArgs, format);
    }
    if (subcommand == "export") {
        return HandleExport(context, subArgs, format);
    }

    return JzCliResult::Error(JzCliExitCode::InvalidArguments,
                              std::format("Unknown asset subcommand '{}'.\n\n{}",
                                          subcommand,
                                          BuildHelp()));
}

String JzAssetCommand::GetHelp() const
{
    return "  asset    Asset import/export operations";
}

} // namespace JzRE
