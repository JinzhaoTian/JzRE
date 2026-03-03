/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#include "JzRE/CLI/commands/JzCreateCommand.h"

#include <format>
#include <fstream>
#include <sstream>

#include <nlohmann/json.hpp>

#include "JzRE/CLI/JzCliArgParser.h"

namespace JzRE {

namespace {

using Json = nlohmann::json;

const String kDomain = "create";

String BuildHelp()
{
    std::ostringstream ss;
    ss << "create commands:\n";
    ss << "  JzRE create shader <name> [--dir <dir>] [--project <file.jzreproject>]\n";
    ss << "  JzRE create script <name> [--dir <dir>] [--project <file.jzreproject>]\n";
    ss << "  JzRE create entity <name> [--dir <dir>] [--project <file.jzreproject>]\n";
    ss << "  JzRE create scene <name> [--dir <dir>] [--project <file.jzreproject>]\n";
    ss << "\n";
    ss << "  --dir     Output directory (default: current working directory or project content root).\n";
    ss << "  --project Load project to resolve default --dir from its content root.";
    return ss.str();
}

std::filesystem::path ResolveOutputDir(JzCliContext &context,
                                       const String *dirValue,
                                       const String *projectFile)
{
    if (dirValue != nullptr && !dirValue->empty()) {
        std::filesystem::path p(*dirValue);
        if (p.is_relative()) {
            p = std::filesystem::current_path() / p;
        }
        return p.lexically_normal();
    }

    if (projectFile != nullptr && !projectFile->empty()) {
        const std::filesystem::path projPath(*projectFile);
        if (context.LoadProject(projPath) == JzEProjectResult::Success) {
            return context.GetProjectManager().GetConfig().GetContentPath();
        }
    }

    return std::filesystem::current_path();
}

JzCliResult WriteFile(const std::filesystem::path &path,
                      const String                &content,
                      JzCliOutputFormat            format)
{
    std::error_code ec;
    std::filesystem::create_directories(path.parent_path(), ec);
    if (ec) {
        return JzCliResult::Error(JzCliExitCode::IoError,
                                  std::format("Failed to create directory '{}': {}",
                                              path.parent_path().string(),
                                              ec.message()));
    }

    std::ofstream ofs(path);
    if (!ofs) {
        return JzCliResult::Error(JzCliExitCode::IoError,
                                  std::format("Failed to write file: {}", path.string()));
    }
    ofs << content;

    if (format == JzCliOutputFormat::Json) {
        Json payload;
        payload["ok"]   = true;
        payload["path"] = path.string();
        return JzCliResult::Ok(payload.dump(2));
    }

    return JzCliResult::Ok(std::format("Created: {}", path.string()));
}

JzCliResult HandleCreateShader(JzCliContext              &context,
                               const std::vector<String> &args,
                               JzCliOutputFormat          format)
{
    auto parsed = JzCliArgParser::Parse(args);

    if (parsed.positionals.empty()) {
        return JzCliResult::Error(JzCliExitCode::InvalidArguments,
                                  "Missing required argument: <name>\n"
                                  "  Usage: JzRE create shader <name> [--dir <dir>]");
    }

    const String name    = parsed.positionals.front();
    const auto   outDir  = ResolveOutputDir(context,
                                            parsed.GetFirstValue("--dir"),
                                            parsed.GetFirstValue("--project"));
    const auto   outPath = (outDir / (name + ".jzshader.src.json")).lexically_normal();

    Json manifest;
    manifest["version"] = 1;
    manifest["name"]    = name;
    manifest["stages"]  = {{"vertex", ""}, {"fragment", ""}};

    return WriteFile(outPath, manifest.dump(2) + "\n", format);
}

JzCliResult HandleCreateScript(JzCliContext              &context,
                               const std::vector<String> &args,
                               JzCliOutputFormat          format)
{
    auto parsed = JzCliArgParser::Parse(args);

    if (parsed.positionals.empty()) {
        return JzCliResult::Error(JzCliExitCode::InvalidArguments,
                                  "Missing required argument: <name>\n"
                                  "  Usage: JzRE create script <name> [--dir <dir>]");
    }

    const String name    = parsed.positionals.front();
    const auto   outDir  = ResolveOutputDir(context,
                                            parsed.GetFirstValue("--dir"),
                                            parsed.GetFirstValue("--project"));
    const auto   outPath = (outDir / (name + ".lua")).lexically_normal();

    std::ostringstream ss;
    ss << "-- " << name << ".lua\n";
    ss << "\n";
    ss << "function OnStart(entity)\n";
    ss << "end\n";
    ss << "\n";
    ss << "function OnUpdate(entity, dt)\n";
    ss << "end\n";
    ss << "\n";
    ss << "function OnStop(entity)\n";
    ss << "end\n";

    return WriteFile(outPath, ss.str(), format);
}

JzCliResult HandleCreateEntity(JzCliContext              &context,
                               const std::vector<String> &args,
                               JzCliOutputFormat          format)
{
    auto parsed = JzCliArgParser::Parse(args);

    if (parsed.positionals.empty()) {
        return JzCliResult::Error(JzCliExitCode::InvalidArguments,
                                  "Missing required argument: <name>\n"
                                  "  Usage: JzRE create entity <name> [--dir <dir>]");
    }

    const String name    = parsed.positionals.front();
    const auto   outDir  = ResolveOutputDir(context,
                                            parsed.GetFirstValue("--dir"),
                                            parsed.GetFirstValue("--project"));
    const auto   outPath = (outDir / (name + ".jzentity.json")).lexically_normal();

    Json entity;
    entity["version"]    = 1;
    entity["name"]       = name;
    entity["components"] = Json::array();

    return WriteFile(outPath, entity.dump(2) + "\n", format);
}

JzCliResult HandleCreateScene(JzCliContext              &context,
                              const std::vector<String> &args,
                              JzCliOutputFormat          format)
{
    auto parsed = JzCliArgParser::Parse(args);

    if (parsed.positionals.empty()) {
        return JzCliResult::Error(JzCliExitCode::InvalidArguments,
                                  "Missing required argument: <name>\n"
                                  "  Usage: JzRE create scene <name> [--dir <dir>]");
    }

    const String name    = parsed.positionals.front();
    const auto   outDir  = ResolveOutputDir(context,
                                            parsed.GetFirstValue("--dir"),
                                            parsed.GetFirstValue("--project"));
    const auto   outPath = (outDir / name).lexically_normal();

    // Ensure .jzscene extension
    std::filesystem::path scenePath = outPath;
    if (scenePath.extension() != ".jzscene") {
        scenePath = scenePath.replace_extension(".jzscene");
    }

    // Also ensure it's in a Scenes subdirectory if the user didn't specify one
    if (scenePath.parent_path().filename() != "Scenes") {
        scenePath = scenePath.parent_path() / "Scenes" / scenePath.filename();
    }

    Json scene;
    scene["version"]  = 1;
    scene["entities"] = Json::array();

    return WriteFile(scenePath, scene.dump(2) + "\n", format);
}

} // namespace

const String &JzCreateCommand::GetDomain() const
{
    return kDomain;
}

JzCliResult JzCreateCommand::Execute(JzCliContext              &context,
                                     const std::vector<String> &args,
                                     JzCliOutputFormat          format)
{
    if (args.empty() || args.front() == "--help" || args.front() == "-h") {
        return JzCliResult::Ok(BuildHelp());
    }

    const auto                type = args.front();
    const std::vector<String> subArgs(args.begin() + 1, args.end());

    if (type == "shader") {
        return HandleCreateShader(context, subArgs, format);
    }
    if (type == "script") {
        return HandleCreateScript(context, subArgs, format);
    }
    if (type == "entity") {
        return HandleCreateEntity(context, subArgs, format);
    }
    if (type == "scene") {
        return HandleCreateScene(context, subArgs, format);
    }

    return JzCliResult::Error(
        JzCliExitCode::InvalidArguments,
        std::format("Unknown create type '{}'.\n\n{}", type, BuildHelp()));
}

String JzCreateCommand::GetHelp() const
{
    return "  create   Create project files (shader, script, entity, scene)";
}

} // namespace JzRE
