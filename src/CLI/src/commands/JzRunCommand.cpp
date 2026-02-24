/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/CLI/commands/JzRunCommand.h"

#include <format>
#include <optional>
#include <sstream>

#include <nlohmann/json.hpp>

#include "JzRE/CLI/JzCliArgParser.h"
#include "JzRE/Runtime/Function/Project/JzProjectManager.h"
#include "JzRE/Runtime/JzRERuntime.h"

namespace JzRE {

namespace {

using Json = nlohmann::json;

const String kDomain = "run";

class JzCliRuntime final : public JzRERuntime {
public:
    using JzRERuntime::JzRERuntime;
};

String BuildHelp()
{
    return "run command:\n"
           "  JzRE run --project <file.jzreproject> [--rhi auto|opengl|vulkan] [--width <n>] [--height <n>] [--title <name>]";
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

} // namespace

const String &JzRunCommand::GetDomain() const
{
    return kDomain;
}

JzCliResult JzRunCommand::Execute(JzCliContext              &,
                                  const std::vector<String> &args,
                                  JzCliOutputFormat          format)
{
    if (args.empty() || args.front() == "--help" || args.front() == "-h") {
        return JzCliResult::Ok(BuildHelp());
    }

    auto parsed = JzCliArgParser::Parse(args);

    auto *projectFile = parsed.GetFirstValue("--project");
    if (projectFile == nullptr || projectFile->empty()) {
        return JzCliResult::Error(JzCliExitCode::InvalidArguments,
                                  "Missing required option: --project");
    }

    const std::filesystem::path projectPath(*projectFile);
    if (JzProjectManager::ValidateProjectFile(projectPath) != JzEProjectResult::Success) {
        return JzCliResult::Error(JzCliExitCode::ProjectError,
                                  std::format("Invalid project file: {}", projectPath.string()));
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
    return "  run      Launch runtime from project";
}

} // namespace JzRE
