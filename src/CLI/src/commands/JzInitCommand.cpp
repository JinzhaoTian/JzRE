/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#include "JzRE/CLI/commands/JzInitCommand.h"

#include <format>
#include <optional>

#include <nlohmann/json.hpp>

#include "JzRE/CLI/JzCliArgParser.h"

namespace JzRE {

namespace {

using Json = nlohmann::json;

const String kDomain = "init";

String BuildHelp()
{
    return "init command:\n"
           "  JzRE init [path] [--name <name>] [--render-api auto|opengl|vulkan|d3d12|metal] [--startup-mode runtime|host]\n"
           "\n"
           "  path           Directory to initialize (default: current working directory).\n"
           "  --name         Project name (default: directory name).\n"
           "  --render-api   Preferred render API (default: auto).\n"
           "  --startup-mode Startup mode: runtime or host (default: runtime).";
}

std::optional<JzERenderAPI> ParseRenderApi(const String &value)
{
    if (value == "auto") return JzERenderAPI::Auto;
    if (value == "opengl") return JzERenderAPI::OpenGL;
    if (value == "vulkan") return JzERenderAPI::Vulkan;
    if (value == "d3d12") return JzERenderAPI::D3D12;
    if (value == "metal") return JzERenderAPI::Metal;
    return std::nullopt;
}

String RenderApiToString(JzERenderAPI api)
{
    switch (api) {
        case JzERenderAPI::Auto: return "auto";
        case JzERenderAPI::OpenGL: return "opengl";
        case JzERenderAPI::Vulkan: return "vulkan";
        case JzERenderAPI::D3D12: return "d3d12";
        case JzERenderAPI::Metal: return "metal";
    }
    return "auto";
}

std::optional<JzEStartupMode> ParseStartupMode(const String &value)
{
    if (value == "runtime") return JzEStartupMode::Runtime;
    if (value == "host") return JzEStartupMode::Authoring;
    return std::nullopt;
}

String StartupModeToString(JzEStartupMode mode)
{
    return (mode == JzEStartupMode::Runtime) ? "runtime" : "host";
}

String ProjectResultToString(JzEProjectResult result)
{
    switch (result) {
        case JzEProjectResult::Success: return "success";
        case JzEProjectResult::FileNotFound: return "file_not_found";
        case JzEProjectResult::ParseError: return "parse_error";
        case JzEProjectResult::WriteError: return "write_error";
        case JzEProjectResult::VersionMismatch: return "version_mismatch";
        case JzEProjectResult::InvalidPath: return "invalid_path";
        case JzEProjectResult::AlreadyLoaded: return "already_loaded";
        case JzEProjectResult::NoProjectLoaded: return "no_project_loaded";
    }
    return "unknown";
}

} // namespace

const String &JzInitCommand::GetDomain() const
{
    return kDomain;
}

JzCliResult JzInitCommand::Execute(JzCliContext              &context,
                                   const std::vector<String> &args,
                                   JzCliOutputFormat          format)
{
    if (args.empty() || args.front() == "--help" || args.front() == "-h") {
        return JzCliResult::Ok(BuildHelp());
    }

    auto parsed = JzCliArgParser::Parse(args);

    // Resolve target directory from positional arg or CWD
    std::filesystem::path dir;
    if (!parsed.positionals.empty()) {
        dir = std::filesystem::path(parsed.positionals.front());
        if (dir.is_relative()) {
            dir = std::filesystem::current_path() / dir;
        }
        dir = dir.lexically_normal();
    } else {
        dir = std::filesystem::current_path();
    }

    // Determine project name
    String name;
    if (auto *nameValue = parsed.GetFirstValue("--name")) {
        name = *nameValue;
    } else {
        name = dir.filename().string();
    }

    if (name.empty()) {
        return JzCliResult::Error(JzCliExitCode::InvalidArguments,
                                  "Could not determine project name; use --name to specify it.");
    }

    auto &manager = context.GetProjectManager();
    auto  result  = manager.CreateProject(dir.string(), name);
    if (result != JzEProjectResult::Success) {
        return JzCliResult::Error(JzCliExitCode::ProjectError,
                                  std::format("Failed to initialize project at '{}': {}",
                                              dir.string(),
                                              ProjectResultToString(result)));
    }

    if (auto *renderApiValue = parsed.GetFirstValue("--render-api")) {
        auto api = ParseRenderApi(*renderApiValue);
        if (!api.has_value()) {
            return JzCliResult::Error(JzCliExitCode::InvalidArguments,
                                      std::format("Invalid render API: {}", *renderApiValue));
        }
        manager.GetConfig().renderAPI = *api;
    }

    if (auto *startupModeValue = parsed.GetFirstValue("--startup-mode")) {
        auto mode = ParseStartupMode(*startupModeValue);
        if (!mode.has_value()) {
            return JzCliResult::Error(JzCliExitCode::InvalidArguments,
                                      std::format("Invalid startup mode: {}", *startupModeValue));
        }
        manager.GetConfig().startupMode = *mode;
    }

    result = manager.SaveProject();
    if (result != JzEProjectResult::Success) {
        return JzCliResult::Error(JzCliExitCode::ProjectError,
                                  std::format("Failed to save project: {}",
                                              ProjectResultToString(result)));
    }

    if (format == JzCliOutputFormat::Json) {
        const auto &cfg = manager.GetConfig();
        Json        payload;
        payload["ok"]           = true;
        payload["project"]      = manager.GetProjectFilePath().string();
        payload["project_name"] = cfg.projectName;
        payload["render_api"]   = RenderApiToString(cfg.renderAPI);
        payload["startup_mode"] = StartupModeToString(cfg.startupMode);
        return JzCliResult::Ok(payload.dump(2));
    }

    return JzCliResult::Ok(
        std::format("Initialized project: {}", manager.GetProjectFilePath().string()));
}

String JzInitCommand::GetHelp() const
{
    return "  init     Initialize a new game project";
}

} // namespace JzRE
