/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/CLI/commands/JzSceneCommand.h"

#include <fstream>
#include <format>
#include <sstream>

#include <nlohmann/json.hpp>

#include "JzRE/CLI/JzCliArgParser.h"

namespace JzRE {

namespace {

using Json = nlohmann::json;

const String kDomain = "scene";

String BuildHelp()
{
    std::ostringstream ss;
    ss << "scene commands:\n";
    ss << "  JzRE scene validate --file <scene.jzscene>\n";
    ss << "  JzRE scene stats --file <scene.jzscene>";
    return ss.str();
}

JzCliResult LoadSceneJson(const std::filesystem::path &filePath, Json &outJson)
{
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return JzCliResult::Error(JzCliExitCode::IoError,
                                  std::format("Scene file not found: {}", filePath.string()));
    }

    try {
        file >> outJson;
    } catch (const std::exception &e) {
        return JzCliResult::Error(JzCliExitCode::IoError,
                                  std::format("Failed to parse scene JSON '{}': {}",
                                              filePath.string(), e.what()));
    }

    return JzCliResult::Ok();
}

JzCliResult ValidateSceneJson(const Json &scene)
{
    if (!scene.contains("version") || !scene["version"].is_number_unsigned()) {
        return JzCliResult::Error(JzCliExitCode::InvalidArguments,
                                  "Invalid scene: missing unsigned 'version' field");
    }

    if (!scene.contains("entities") || !scene["entities"].is_array()) {
        return JzCliResult::Error(JzCliExitCode::InvalidArguments,
                                  "Invalid scene: missing array 'entities' field");
    }

    return JzCliResult::Ok();
}

JzCliResult HandleValidate(const std::vector<String> &args, JzCliOutputFormat format)
{
    auto parsed = JzCliArgParser::Parse(args);
    auto *file  = parsed.GetFirstValue("--file");
    if (file == nullptr || file->empty()) {
        return JzCliResult::Error(JzCliExitCode::InvalidArguments,
                                  "Missing required option: --file");
    }

    Json scene;
    const auto path       = std::filesystem::path(*file).lexically_normal();
    const auto loadResult = LoadSceneJson(path, scene);
    if (!loadResult.IsSuccess()) {
        return loadResult;
    }

    const auto validateResult = ValidateSceneJson(scene);
    if (!validateResult.IsSuccess()) {
        return validateResult;
    }

    if (format == JzCliOutputFormat::Json) {
        Json payload;
        payload["ok"]     = true;
        payload["file"]   = path.string();
        payload["version"] = scene["version"];
        payload["entities"] = scene["entities"].size();
        return JzCliResult::Ok(payload.dump(2));
    }

    return JzCliResult::Ok(std::format("Scene file is valid: {}", path.string()));
}

JzCliResult HandleStats(const std::vector<String> &args, JzCliOutputFormat format)
{
    auto parsed = JzCliArgParser::Parse(args);
    auto *file  = parsed.GetFirstValue("--file");
    if (file == nullptr || file->empty()) {
        return JzCliResult::Error(JzCliExitCode::InvalidArguments,
                                  "Missing required option: --file");
    }

    Json scene;
    const auto path       = std::filesystem::path(*file).lexically_normal();
    const auto loadResult = LoadSceneJson(path, scene);
    if (!loadResult.IsSuccess()) {
        return loadResult;
    }

    const auto validateResult = ValidateSceneJson(scene);
    if (!validateResult.IsSuccess()) {
        return validateResult;
    }

    Size entityCount      = scene["entities"].size();
    Size namedCount       = 0;
    Size transformCount   = 0;
    Size assetEntryCount  = 0;
    Size taggedEntryCount = 0;

    for (const auto &entity : scene["entities"]) {
        if (entity.contains("name")) {
            ++namedCount;
        }
        if (entity.contains("transform")) {
            ++transformCount;
        }
        if (entity.contains("assets")) {
            ++assetEntryCount;
        }
        if (entity.contains("tags")) {
            ++taggedEntryCount;
        }
    }

    if (format == JzCliOutputFormat::Json) {
        Json payload;
        payload["file"]            = path.string();
        payload["version"]         = scene["version"];
        payload["entities"]        = entityCount;
        payload["named_entities"]  = namedCount;
        payload["with_transform"]  = transformCount;
        payload["with_assets"]     = assetEntryCount;
        payload["with_tags"]       = taggedEntryCount;
        return JzCliResult::Ok(payload.dump(2));
    }

    std::ostringstream ss;
    ss << "Scene stats: " << path.string() << "\n";
    ss << "  Version: " << scene["version"] << "\n";
    ss << "  Entities: " << entityCount << "\n";
    ss << "  Named: " << namedCount << "\n";
    ss << "  With Transform: " << transformCount << "\n";
    ss << "  With Assets: " << assetEntryCount << "\n";
    ss << "  With Tags: " << taggedEntryCount;

    return JzCliResult::Ok(ss.str());
}

} // namespace

const String &JzSceneCommand::GetDomain() const
{
    return kDomain;
}

JzCliResult JzSceneCommand::Execute(JzCliContext              &,
                                    const std::vector<String> &args,
                                    JzCliOutputFormat          format)
{
    if (args.empty() || args.front() == "--help" || args.front() == "-h") {
        return JzCliResult::Ok(BuildHelp());
    }

    const auto subcommand = args.front();
    const std::vector<String> subArgs(args.begin() + 1, args.end());

    if (subcommand == "validate") {
        return HandleValidate(subArgs, format);
    }
    if (subcommand == "stats") {
        return HandleStats(subArgs, format);
    }

    return JzCliResult::Error(JzCliExitCode::InvalidArguments,
                              std::format("Unknown scene subcommand '{}'.\n\n{}",
                                          subcommand,
                                          BuildHelp()));
}

String JzSceneCommand::GetHelp() const
{
    return "  scene    Scene file utilities";
}

} // namespace JzRE
