/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#include "JzRE/CLI/commands/JzImportCommand.h"

#include <format>
#include <sstream>
#include <unordered_set>

#include <nlohmann/json.hpp>

#include "JzRE/CLI/JzCliArgParser.h"

namespace JzRE {

namespace {

using Json = nlohmann::json;

const String kDomain = "import";

String BuildHelp()
{
    return "import command:\n"
           "  JzRE import <src> [--project <file.jzreproject>] [--overwrite] [--subfolder <dir>] [--model]\n"
           "\n"
           "  <src>        Source file or directory to import.\n"
           "  --project    Path to the project file.\n"
           "  --overwrite  Overwrite existing assets.\n"
           "  --subfolder  Place imported assets into a content subfolder.\n"
           "  --model      Import as a 3D model (includes textures and materials).";
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

JzCliResult HandleImportRegular(JzCliContext                &context,
                                const std::filesystem::path &projectPath,
                                const std::vector<String>   &srcList,
                                const JzImportOptions       &options,
                                JzCliOutputFormat            format)
{
    const auto loadResult = LoadProjectOrError(context, projectPath);
    if (!loadResult.IsSuccess()) {
        return loadResult;
    }

    std::vector<std::filesystem::path> sourcePaths;
    sourcePaths.reserve(srcList.size());
    for (const auto &src : srcList) {
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

JzCliResult HandleImportModel(JzCliContext                &context,
                              const std::filesystem::path &projectPath,
                              const String                &src,
                              const JzImportOptions       &options,
                              JzCliOutputFormat            format)
{
    const auto loadResult = LoadProjectOrError(context, projectPath);
    if (!loadResult.IsSuccess()) {
        return loadResult;
    }

    auto result = context.GetAssetImporter().ImportModelWithDependencies(
        ResolveAbsolute(src), options);

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
    ss << "Model import: " << ImportResultToString(result.modelEntry.result) << "\n";
    ss << "Dependencies: " << dependencySuccess << "/" << result.dependencyEntries.size();

    if (result.modelEntry.result != JzEImportResult::Success || !result.allSucceeded) {
        return JzCliResult::Error(JzCliExitCode::IoError, ss.str());
    }

    return JzCliResult::Ok(ss.str());
}

} // namespace

const String &JzImportCommand::GetDomain() const
{
    return kDomain;
}

JzCliResult JzImportCommand::Execute(JzCliContext              &context,
                                     const std::vector<String> &args,
                                     JzCliOutputFormat          format)
{
    if (args.empty() || args.front() == "--help" || args.front() == "-h") {
        return JzCliResult::Ok(BuildHelp());
    }

    const std::unordered_set<String> flags  = {"--overwrite", "--model"};
    auto                             parsed = JzCliArgParser::Parse(args, flags);

    if (parsed.positionals.empty()) {
        return JzCliResult::Error(JzCliExitCode::InvalidArguments,
                                  "Missing required argument: <src>\n\n" + BuildHelp());
    }

    auto *projectFile = parsed.GetFirstValue("--project");
    if (projectFile == nullptr || projectFile->empty()) {
        return JzCliResult::Error(JzCliExitCode::InvalidArguments,
                                  "Missing required option: --project");
    }

    const auto projectPath = ResolveAbsolute(*projectFile);

    JzImportOptions options;
    options.overwriteExisting = parsed.HasOption("--overwrite");
    if (auto *subfolder = parsed.GetFirstValue("--subfolder")) {
        options.autoDetectSubfolder = false;
        options.targetSubfolder     = *subfolder;
    }

    if (parsed.HasOption("--model")) {
        return HandleImportModel(context, projectPath, parsed.positionals.front(), options, format);
    }

    return HandleImportRegular(context, projectPath, parsed.positionals, options, format);
}

String JzImportCommand::GetHelp() const
{
    return "  import   Import assets into a project";
}

} // namespace JzRE
