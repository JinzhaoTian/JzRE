/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/CLI/commands/JzShaderCommand.h"

#include <algorithm>
#include <cstdlib>
#include <format>
#include <sstream>

#include <nlohmann/json.hpp>

#include "JzRE/CLI/JzCliArgParser.h"

namespace JzRE {

namespace {

using Json = nlohmann::json;

const String kDomain = "shader";

String BuildHelp()
{
    std::ostringstream ss;
    ss << "shader commands:\n";
    ss << "  JzRE shader cook --input <manifest-or-dir> --output-dir <dir> [--tool <path-to-JzREShaderTool>]\n";
    ss << "  JzRE shader cook-project --project <file.jzreproject> [--tool <path-to-JzREShaderTool>]";
    return ss.str();
}

Bool IsManifestPath(const std::filesystem::path &path)
{
    auto lowered = path.filename().string();
    std::transform(lowered.begin(), lowered.end(), lowered.begin(), ::tolower);
    return lowered.ends_with(".jzshader.src.json");
}

String Quote(const String &value)
{
    String out = "\"";
    for (char c : value) {
        if (c == '\\' || c == '"') {
            out.push_back('\\');
        }
        out.push_back(c);
    }
    out.push_back('"');
    return out;
}

std::filesystem::path ResolveToolPath(const String *requestedPath)
{
#ifdef _WIN32
    constexpr const char *kToolName = "JzREShaderTool.exe";
#else
    constexpr const char *kToolName = "JzREShaderTool";
#endif

    if (requestedPath != nullptr && !requestedPath->empty()) {
        std::filesystem::path tool(*requestedPath);
        if (tool.is_relative()) {
            tool = std::filesystem::current_path() / tool;
        }
        return tool.lexically_normal();
    }

    if (const char *envTool = std::getenv("JzRE_SHADER_TOOL_PATH")) {
        if (*envTool != '\0') {
            return std::filesystem::path(envTool).lexically_normal();
        }
    }

    const auto candidate = (std::filesystem::current_path() / kToolName).lexically_normal();
    if (std::filesystem::exists(candidate)) {
        return candidate;
    }

    return std::filesystem::path(kToolName);
}

void CollectManifests(const std::filesystem::path &inputPath,
                      std::vector<std::filesystem::path> &outManifests)
{
    outManifests.clear();

    if (std::filesystem::is_regular_file(inputPath) && IsManifestPath(inputPath)) {
        outManifests.push_back(inputPath.lexically_normal());
        return;
    }

    if (!std::filesystem::is_directory(inputPath)) {
        return;
    }

    std::error_code ec;
    for (std::filesystem::recursive_directory_iterator it(inputPath,
                                                          std::filesystem::directory_options::skip_permission_denied,
                                                          ec),
         end;
         it != end;
         it.increment(ec)) {
        if (ec || !it->is_regular_file(ec)) {
            continue;
        }
        if (IsManifestPath(it->path())) {
            outManifests.push_back(it->path().lexically_normal());
        }
    }

    std::sort(outManifests.begin(), outManifests.end());
}

Bool CookOneManifest(const std::filesystem::path &toolPath,
                     const std::filesystem::path &manifestPath,
                     const std::filesystem::path &outputDir)
{
    std::ostringstream command;
    command << Quote(toolPath.string())
            << " --input " << Quote(manifestPath.string())
            << " --output-dir " << Quote(outputDir.string());

    return std::system(command.str().c_str()) == 0;
}

JzCliResult CookInputPath(const std::filesystem::path &toolPath,
                          const std::filesystem::path &inputPath,
                          const std::filesystem::path &outputDir,
                          JzCliOutputFormat            format)
{
    if (!std::filesystem::exists(inputPath)) {
        return JzCliResult::Error(JzCliExitCode::IoError,
                                  std::format("Input path does not exist: {}", inputPath.string()));
    }

    std::vector<std::filesystem::path> manifests;
    CollectManifests(inputPath, manifests);
    if (manifests.empty()) {
        return JzCliResult::Error(JzCliExitCode::IoError,
                                  std::format("No shader manifests found under {}", inputPath.string()));
    }

    std::error_code ec;
    std::filesystem::create_directories(outputDir, ec);
    if (ec) {
        return JzCliResult::Error(JzCliExitCode::IoError,
                                  std::format("Failed to create output directory '{}': {}",
                                              outputDir.string(), ec.message()));
    }

    Size cookedCount = 0;
    std::vector<String> failed;

    for (const auto &manifest : manifests) {
        if (CookOneManifest(toolPath, manifest, outputDir)) {
            ++cookedCount;
        } else {
            failed.push_back(manifest.string());
        }
    }

    if (format == JzCliOutputFormat::Json) {
        Json payload;
        payload["tool"]        = toolPath.string();
        payload["input"]       = inputPath.string();
        payload["output_dir"]  = outputDir.string();
        payload["cooked"]      = cookedCount;
        payload["total"]       = manifests.size();
        payload["failed_files"] = failed;

        if (!failed.empty()) {
            return JzCliResult::Error(JzCliExitCode::ToolError, payload.dump(2));
        }

        return JzCliResult::Ok(payload.dump(2));
    }

    std::ostringstream ss;
    ss << "Cooked " << cookedCount << "/" << manifests.size() << " shader manifests";
    if (!failed.empty()) {
        ss << "\nFailed manifests:";
        for (const auto &file : failed) {
            ss << "\n  - " << file;
        }
        return JzCliResult::Error(JzCliExitCode::ToolError, ss.str());
    }

    return JzCliResult::Ok(ss.str());
}

JzCliResult HandleCook(const std::vector<String> &args, JzCliOutputFormat format)
{
    auto parsed = JzCliArgParser::Parse(args);

    const auto *input = parsed.GetFirstValue("--input");
    if (input == nullptr || input->empty()) {
        return JzCliResult::Error(JzCliExitCode::InvalidArguments,
                                  "Missing required option: --input");
    }

    const auto *outputDir = parsed.GetFirstValue("--output-dir");
    if (outputDir == nullptr || outputDir->empty()) {
        return JzCliResult::Error(JzCliExitCode::InvalidArguments,
                                  "Missing required option: --output-dir");
    }

    const auto toolPath = ResolveToolPath(parsed.GetFirstValue("--tool"));
    return CookInputPath(toolPath,
                         std::filesystem::path(*input).lexically_normal(),
                         std::filesystem::path(*outputDir).lexically_normal(),
                         format);
}

JzCliResult HandleCookProject(JzCliContext              &context,
                              const std::vector<String> &args,
                              JzCliOutputFormat          format)
{
    auto parsed = JzCliArgParser::Parse(args);

    const auto *projectFile = parsed.GetFirstValue("--project");
    if (projectFile == nullptr || projectFile->empty()) {
        return JzCliResult::Error(JzCliExitCode::InvalidArguments,
                                  "Missing required option: --project");
    }

    const std::filesystem::path projectPath(*projectFile);
    const auto loadResult = context.LoadProject(projectPath);
    if (loadResult != JzEProjectResult::Success) {
        return JzCliResult::Error(JzCliExitCode::ProjectError,
                                  std::format("Failed to load project '{}': {}",
                                              projectPath.string(),
                                              static_cast<int>(loadResult)));
    }

    const auto &cfg = context.GetProjectManager().GetConfig();
    const auto sourceRoot = cfg.GetShaderSourcePath();
    const auto outputRoot = cfg.GetShaderCookedPath();

    const auto toolPath = ResolveToolPath(parsed.GetFirstValue("--tool"));
    return CookInputPath(toolPath, sourceRoot, outputRoot, format);
}

} // namespace

const String &JzShaderCommand::GetDomain() const
{
    return kDomain;
}

JzCliResult JzShaderCommand::Execute(JzCliContext              &context,
                                     const std::vector<String> &args,
                                     JzCliOutputFormat          format)
{
    if (args.empty() || args.front() == "--help" || args.front() == "-h") {
        return JzCliResult::Ok(BuildHelp());
    }

    const auto subcommand = args.front();
    const std::vector<String> subArgs(args.begin() + 1, args.end());

    if (subcommand == "cook") {
        return HandleCook(subArgs, format);
    }
    if (subcommand == "cook-project") {
        return HandleCookProject(context, subArgs, format);
    }

    return JzCliResult::Error(JzCliExitCode::InvalidArguments,
                              std::format("Unknown shader subcommand '{}'.\n\n{}",
                                          subcommand,
                                          BuildHelp()));
}

String JzShaderCommand::GetHelp() const
{
    return "  shader   Shader cooking operations";
}

} // namespace JzRE
