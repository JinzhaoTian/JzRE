/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#include "JzRE/CLI/commands/JzRunCommand.h"

#include <cstdio>
#include <format>
#include <optional>
#include <unordered_set>

#include <nlohmann/json.hpp>

#include "JzRE/CLI/JzCliArgParser.h"
#include "JzRE/CLI/commands/JzBuildCommand.h"
#include "JzRE/Runtime/Function/Project/JzProjectManager.h"
#include "JzRE/Runtime/JzRERuntime.h"

namespace JzRE {

namespace {

using Json = nlohmann::json;

const String kDomain     = "run";
const String kBuildStamp = ".jzre-built";

class JzCliRuntime final : public JzRERuntime {
public:
    using JzRERuntime::JzRERuntime;
};

String BuildHelp()
{
    return "run command:\n"
           "  JzRE run [path] [--project <file.jzreproject>] [--rhi auto|opengl|vulkan]\n"
           "           [--width <n>] [--height <n>] [--title <name>] [--skip-build]\n"
           "\n"
           "  path          Project directory; searches it and parent directories for a\n"
           "                .jzreproject file. Omit to use the current working directory\n"
           "                (current directory only, no parent traversal).\n"
           "  --project     Explicit path to .jzreproject file.\n"
           "  --rhi         Render API to use at runtime (default: auto).\n"
           "  --width       Window width in pixels.\n"
           "  --height      Window height in pixels.\n"
           "  --title       Window title.\n"
           "  --skip-build  Skip automatic build even if project has not been built yet.";
}

std::optional<I32> ParseInteger(const String &value)
{
    try {
        return std::stoi(value);
    } catch (...) {
        return std::nullopt;
    }
}

JzERHIType ParseRhiOrDefault(const String &value)
{
    if (value == "opengl") return JzERHIType::OpenGL;
    if (value == "vulkan") return JzERHIType::Vulkan;
    return JzERHIType::Unknown;
}

String RhiToString(JzERHIType type)
{
    if (type == JzERHIType::OpenGL) return "opengl";
    if (type == JzERHIType::Vulkan) return "vulkan";
    return "auto";
}

std::optional<std::filesystem::path> FindProjectFileIn(const std::filesystem::path &dir)
{
    if (!std::filesystem::is_directory(dir)) {
        return std::nullopt;
    }

    std::error_code ec;
    for (const auto &entry : std::filesystem::directory_iterator(dir, ec)) {
        if (ec) {
            break;
        }
        if (entry.is_regular_file() && entry.path().extension() == ".jzreproject") {
            return entry.path().lexically_normal();
        }
    }

    return std::nullopt;
}

std::optional<std::filesystem::path> DiscoverProjectFile(const std::filesystem::path &startDir)
{
    auto dir = startDir.lexically_normal();
    while (true) {
        if (auto found = FindProjectFileIn(dir)) {
            return found;
        }
        auto parent = dir.parent_path();
        if (parent == dir) {
            break;
        }
        dir = parent;
    }
    return std::nullopt;
}

Bool IsProjectBuilt(const std::filesystem::path &projectPath)
{
    const auto stampPath = (projectPath.parent_path() / kBuildStamp).lexically_normal();
    return std::filesystem::exists(stampPath);
}

} // namespace

const String &JzRunCommand::GetDomain() const
{
    return kDomain;
}

JzCliResult JzRunCommand::Execute(JzCliContext              &context,
                                  const std::vector<String> &args,
                                  JzCliOutputFormat          format)
{
    if (!args.empty() && (args.front() == "--help" || args.front() == "-h")) {
        return JzCliResult::Ok(BuildHelp());
    }

    const std::unordered_set<String> flags  = {"--skip-build"};
    auto                             parsed = JzCliArgParser::Parse(args, flags);

    // Resolve project file
    std::filesystem::path projectPath;

    if (auto *projectFile = parsed.GetFirstValue("--project")) {
        projectPath = std::filesystem::path(*projectFile).lexically_normal();
    } else if (!parsed.positionals.empty()) {
        auto startDir = std::filesystem::path(parsed.positionals.front());
        if (startDir.is_relative()) {
            startDir = std::filesystem::current_path() / startDir;
        }
        startDir = startDir.lexically_normal();

        if (startDir.extension() == ".jzreproject") {
            projectPath = startDir;
        } else {
            auto found = DiscoverProjectFile(startDir);
            if (!found.has_value()) {
                return JzCliResult::Error(
                    JzCliExitCode::ProjectError,
                    std::format("No .jzreproject file found in '{}' or its parent directories.\n"
                                "Use --project to specify the project file explicitly.",
                                startDir.string()));
            }
            projectPath = *found;
        }
    } else {
        // Default: look for a .jzreproject file in the current working directory only
        const auto cwd   = std::filesystem::current_path();
        auto       found = FindProjectFileIn(cwd);
        if (!found.has_value()) {
            return JzCliResult::Error(
                JzCliExitCode::ProjectError,
                std::format("No .jzreproject file found in '{}'.\n"
                            "Use --project to specify the project file explicitly.",
                            cwd.string()));
        }
        projectPath = *found;
    }

    if (JzProjectManager::ValidateProjectFile(projectPath) != JzEProjectResult::Success) {
        return JzCliResult::Error(JzCliExitCode::ProjectError,
                                  std::format("Invalid project file: {}", projectPath.string()));
    }

    // Auto-build if stamp is absent and --skip-build is not set
    if (!parsed.HasOption("--skip-build") && !IsProjectBuilt(projectPath)) {
        const auto buildResult = JzBuildCommand::BuildProject(context, projectPath, format);
        if (!buildResult.IsSuccess()) {
            return buildResult;
        }
        // Print build output before proceeding to run
        if (!buildResult.message.empty() && format != JzCliOutputFormat::Json) {
            // The caller in main.cpp will print the final result; for intermediate output
            // we write directly to stdout to keep the flow clear
            std::fputs((buildResult.message + "\n").c_str(), stdout);
        }
    }

    JzRERuntimeSettings settings;
    settings.projectFile = projectPath;

    if (auto *rhi = parsed.GetFirstValue("--rhi")) {
        settings.rhiType = ParseRhiOrDefault(*rhi);
    }

    if (auto *widthValue = parsed.GetFirstValue("--width")) {
        auto width = ParseInteger(*widthValue);
        if (!width.has_value() || *width <= 0) {
            return JzCliResult::Error(JzCliExitCode::InvalidArguments,
                                      std::format("Invalid width: {}", *widthValue));
        }
        settings.windowSize.x = *width;
    }

    if (auto *heightValue = parsed.GetFirstValue("--height")) {
        auto height = ParseInteger(*heightValue);
        if (!height.has_value() || *height <= 0) {
            return JzCliResult::Error(JzCliExitCode::InvalidArguments,
                                      std::format("Invalid height: {}", *heightValue));
        }
        settings.windowSize.y = *height;
    }

    if (auto *title = parsed.GetFirstValue("--title")) {
        settings.windowTitle = *title;
    }

    try {
        JzCliRuntime runtime(settings);
        runtime.Run();
    } catch (const std::exception &e) {
        return JzCliResult::Error(JzCliExitCode::RuntimeError,
                                  std::format("Runtime launch failed: {}", e.what()));
    }

    if (format == JzCliOutputFormat::Json) {
        Json payload;
        payload["ok"]      = true;
        payload["project"] = projectPath.string();
        payload["rhi"]     = RhiToString(settings.rhiType);
        payload["width"]   = settings.windowSize.x;
        payload["height"]  = settings.windowSize.y;
        payload["title"]   = settings.windowTitle;
        return JzCliResult::Ok(payload.dump(2));
    }

    return JzCliResult::Ok(std::format("Runtime finished: {}", projectPath.string()));
}

String JzRunCommand::GetHelp() const
{
    return "  run      Run the game project (auto-builds if needed)";
}

} // namespace JzRE
