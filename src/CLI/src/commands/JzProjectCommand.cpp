/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/CLI/commands/JzProjectCommand.h"

#include <format>
#include <optional>
#include <sstream>

#include <nlohmann/json.hpp>

#include "JzRE/CLI/JzCliArgParser.h"

namespace JzRE {

namespace {

using Json = nlohmann::json;

const String kDomain = "project";

String BuildHelp()
{
    std::ostringstream ss;
    ss << "project commands:\n";
    ss << "  JzRE project create --name <name> --dir <dir> [--render-api auto|opengl|vulkan|d3d12|metal] [--startup-mode runtime|host]\n";
    ss << "  JzRE project validate --project <file.jzreproject>\n";
    ss << "  JzRE project info --project <file.jzreproject>\n";
    ss << "  JzRE project set --project <file.jzreproject> --default-scene <path> [--render-api ...] [--shader-auto-cook on|off] [--startup-mode runtime|host]";
    return ss.str();
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
    if (value == "runtime") {
        return JzEStartupMode::Runtime;
    }

    if (value == "host") {
        return JzEStartupMode::Authoring;
    }

    return std::nullopt;
}

String StartupModeToString(JzEStartupMode mode)
{
    if (mode == JzEStartupMode::Runtime) {
        return "runtime";
    }
    return "host";
}

JzCliResult BuildProjectError(const std::filesystem::path &projectPath, JzEProjectResult result)
{
    return JzCliResult::Error(
        JzCliExitCode::ProjectError,
        std::format("Project operation failed: {} ({})",
                    projectPath.string(),
                    ProjectResultToString(result)));
}

JzCliResult HandleValidate(const std::vector<String> &args, JzCliOutputFormat format)
{
    auto parsed       = JzCliArgParser::Parse(args);
    auto *projectFile = parsed.GetFirstValue("--project");
    if (projectFile == nullptr || projectFile->empty()) {
        return JzCliResult::Error(JzCliExitCode::InvalidArguments,
                                  "Missing required option: --project");
    }

    const std::filesystem::path projectPath(*projectFile);
    auto result = JzProjectManager::ValidateProjectFile(projectPath);
    if (result != JzEProjectResult::Success) {
        return BuildProjectError(projectPath, result);
    }

    if (format == JzCliOutputFormat::Json) {
        Json payload;
        payload["ok"]      = true;
        payload["project"] = projectPath.string();
        payload["result"]  = "success";
        return JzCliResult::Ok(payload.dump(2));
    }

    return JzCliResult::Ok(std::format("Project file is valid: {}", projectPath.string()));
}

JzCliResult HandleCreate(JzCliContext              &context,
                         const std::vector<String> &args,
                         JzCliOutputFormat          format)
{
    auto parsed = JzCliArgParser::Parse(args);
    auto *name  = parsed.GetFirstValue("--name");
    auto *dir   = parsed.GetFirstValue("--dir");

    if (name == nullptr || name->empty()) {
        return JzCliResult::Error(JzCliExitCode::InvalidArguments,
                                  "Missing required option: --name");
    }
    if (dir == nullptr || dir->empty()) {
        return JzCliResult::Error(JzCliExitCode::InvalidArguments,
                                  "Missing required option: --dir");
    }

    auto &manager = context.GetProjectManager();
    auto  result  = manager.CreateProject(*dir, *name);
    if (result != JzEProjectResult::Success) {
        return BuildProjectError(*dir, result);
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
        return BuildProjectError(manager.GetProjectFilePath(), result);
    }

    if (format == JzCliOutputFormat::Json) {
        const auto &cfg = manager.GetConfig();
        Json payload;
        payload["ok"]           = true;
        payload["project"]      = manager.GetProjectFilePath().string();
        payload["project_name"] = cfg.projectName;
        payload["render_api"]   = RenderApiToString(cfg.renderAPI);
        payload["startup_mode"] = StartupModeToString(cfg.startupMode);
        return JzCliResult::Ok(payload.dump(2));
    }

    return JzCliResult::Ok(std::format("Created project: {}", manager.GetProjectFilePath().string()));
}

JzCliResult HandleInfo(JzCliContext              &context,
                       const std::vector<String> &args,
                       JzCliOutputFormat          format)
{
    auto parsed       = JzCliArgParser::Parse(args);
    auto *projectFile = parsed.GetFirstValue("--project");
    if (projectFile == nullptr || projectFile->empty()) {
        return JzCliResult::Error(JzCliExitCode::InvalidArguments,
                                  "Missing required option: --project");
    }

    const std::filesystem::path projectPath(*projectFile);
    const auto                  result = context.LoadProject(projectPath);
    if (result != JzEProjectResult::Success) {
        return BuildProjectError(projectPath, result);
    }

    const auto &cfg = context.GetProjectManager().GetConfig();

    if (format == JzCliOutputFormat::Json) {
        Json payload;
        payload["project_name"]      = cfg.projectName;
        payload["project_file"]      = context.GetProjectManager().GetProjectFilePath().string();
        payload["project_id"]        = cfg.projectId;
        payload["engine_version"]    = cfg.engineVersion;
        payload["content_root"]      = cfg.contentRoot.string();
        payload["config_root"]       = cfg.configRoot.string();
        payload["default_scene"]     = cfg.defaultScene;
        payload["render_api"]        = RenderApiToString(cfg.renderAPI);
        payload["startup_mode"]      = StartupModeToString(cfg.startupMode);
        payload["shader_auto_cook"]  = cfg.shaderAutoCook;
        payload["shader_source_root"] = cfg.shaderSourceRoot.string();
        payload["shader_cooked_root"] = cfg.shaderCookedRoot.string();
        payload["project_version"]    = cfg.projectVersion;
        return JzCliResult::Ok(payload.dump(2));
    }

    std::ostringstream ss;
    ss << "Project: " << cfg.projectName << "\n";
    ss << "  File: " << context.GetProjectManager().GetProjectFilePath().string() << "\n";
    ss << "  ID: " << cfg.projectId << "\n";
    ss << "  Engine Version: " << cfg.engineVersion << "\n";
    ss << "  Render API: " << RenderApiToString(cfg.renderAPI) << "\n";
    ss << "  Startup Mode: " << StartupModeToString(cfg.startupMode) << "\n";
    ss << "  Default Scene: " << cfg.defaultScene << "\n";
    ss << "  Content Path: " << cfg.GetContentPath().string() << "\n";
    ss << "  Shader Auto Cook: " << (cfg.shaderAutoCook ? "on" : "off");

    return JzCliResult::Ok(ss.str());
}

JzCliResult HandleSet(JzCliContext              &context,
                      const std::vector<String> &args,
                      JzCliOutputFormat          format)
{
    auto parsed       = JzCliArgParser::Parse(args);
    auto *projectFile = parsed.GetFirstValue("--project");
    if (projectFile == nullptr || projectFile->empty()) {
        return JzCliResult::Error(JzCliExitCode::InvalidArguments,
                                  "Missing required option: --project");
    }

    const std::filesystem::path projectPath(*projectFile);
    auto                        loadResult = context.LoadProject(projectPath);
    if (loadResult != JzEProjectResult::Success) {
        return BuildProjectError(projectPath, loadResult);
    }

    auto &cfg = context.GetProjectManager().GetConfig();
    Bool  changed = false;

    if (auto *scene = parsed.GetFirstValue("--default-scene")) {
        cfg.defaultScene = *scene;
        changed          = true;
    }

    if (auto *renderApiValue = parsed.GetFirstValue("--render-api")) {
        auto api = ParseRenderApi(*renderApiValue);
        if (!api.has_value()) {
            return JzCliResult::Error(JzCliExitCode::InvalidArguments,
                                      std::format("Invalid render API: {}", *renderApiValue));
        }
        cfg.renderAPI = *api;
        changed       = true;
    }

    if (auto *autoCook = parsed.GetFirstValue("--shader-auto-cook")) {
        if (*autoCook == "on") {
            cfg.shaderAutoCook = true;
        } else if (*autoCook == "off") {
            cfg.shaderAutoCook = false;
        } else {
            return JzCliResult::Error(JzCliExitCode::InvalidArguments,
                                      std::format("Invalid shader auto-cook value: {}", *autoCook));
        }
        changed = true;
    }

    if (auto *startupModeValue = parsed.GetFirstValue("--startup-mode")) {
        auto mode = ParseStartupMode(*startupModeValue);
        if (!mode.has_value()) {
            return JzCliResult::Error(JzCliExitCode::InvalidArguments,
                                      std::format("Invalid startup mode: {}", *startupModeValue));
        }
        cfg.startupMode = *mode;
        changed         = true;
    }

    if (!changed) {
        return JzCliResult::Error(JzCliExitCode::InvalidArguments,
                                  "No project fields provided to update");
    }

    auto saveResult = context.GetProjectManager().SaveProject();
    if (saveResult != JzEProjectResult::Success) {
        return BuildProjectError(context.GetProjectManager().GetProjectFilePath(), saveResult);
    }

    if (format == JzCliOutputFormat::Json) {
        Json payload;
        payload["ok"]            = true;
        payload["project"]       = context.GetProjectManager().GetProjectFilePath().string();
        payload["default_scene"] = cfg.defaultScene;
        payload["render_api"]    = RenderApiToString(cfg.renderAPI);
        payload["startup_mode"]  = StartupModeToString(cfg.startupMode);
        payload["shader_auto_cook"] = cfg.shaderAutoCook;
        return JzCliResult::Ok(payload.dump(2));
    }

    return JzCliResult::Ok(std::format("Updated project: {}",
                                       context.GetProjectManager().GetProjectFilePath().string()));
}

} // namespace

const String &JzProjectCommand::GetDomain() const
{
    return kDomain;
}

JzCliResult JzProjectCommand::Execute(JzCliContext              &context,
                                      const std::vector<String> &args,
                                      JzCliOutputFormat          format)
{
    if (args.empty() || args.front() == "--help" || args.front() == "-h") {
        return JzCliResult::Ok(BuildHelp());
    }

    const auto subcommand = args.front();
    const std::vector<String> subArgs(args.begin() + 1, args.end());

    if (subcommand == "create") {
        return HandleCreate(context, subArgs, format);
    }
    if (subcommand == "validate") {
        return HandleValidate(subArgs, format);
    }
    if (subcommand == "info") {
        return HandleInfo(context, subArgs, format);
    }
    if (subcommand == "set") {
        return HandleSet(context, subArgs, format);
    }

    return JzCliResult::Error(JzCliExitCode::InvalidArguments,
                              std::format("Unknown project subcommand '{}'.\n\n{}",
                                          subcommand,
                                          BuildHelp()));
}

String JzProjectCommand::GetHelp() const
{
    return "  project  Project lifecycle operations";
}

} // namespace JzRE
