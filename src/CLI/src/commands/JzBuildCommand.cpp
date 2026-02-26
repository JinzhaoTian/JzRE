/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#include "JzRE/CLI/commands/JzBuildCommand.h"

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <format>
#include <fstream>
#include <sstream>

#include <nlohmann/json.hpp>

#include "JzRE/CLI/JzCliArgParser.h"

namespace JzRE {

namespace {

using Json = nlohmann::json;

const String kDomain     = "build";
const String kBuildStamp = ".jzre-built";

String BuildHelp()
{
    return "build command:\n"
           "  JzRE build [path] [--project <file.jzreproject>] [--shaders-only] [--tool <path>]\n"
           "\n"
           "  path          Project directory (default: current working directory).\n"
           "  --project     Explicit path to .jzreproject file.\n"
           "  --shaders-only Cook shaders only, skip other build steps.\n"
           "  --tool        Path to JzREShaderTool (overrides JzRE_SHADER_TOOL_PATH env var).";
}

// ---------------------------------------------------------------------------
// Shader cooking helpers (shared with JzBuildCommand::BuildProject)
// ---------------------------------------------------------------------------

Bool IsManifestPath(const std::filesystem::path &path)
{
    auto name = path.filename().string();
    std::transform(name.begin(), name.end(), name.begin(), ::tolower);
    return name.ends_with(".jzshader.src.json");
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

void CollectManifests(const std::filesystem::path        &inputPath,
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
    for (std::filesystem::recursive_directory_iterator it(
             inputPath, std::filesystem::directory_options::skip_permission_denied, ec),
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

struct CookResult {
    Size                cookedCount{0};
    Size                totalCount{0};
    std::vector<String> failedFiles;
};

CookResult CookShaders(const std::filesystem::path &toolPath,
                       const std::filesystem::path &sourceRoot,
                       const std::filesystem::path &outputRoot)
{
    CookResult cookResult;

    std::vector<std::filesystem::path> manifests;
    CollectManifests(sourceRoot, manifests);
    cookResult.totalCount = manifests.size();

    if (manifests.empty()) {
        return cookResult;
    }

    std::error_code ec;
    std::filesystem::create_directories(outputRoot, ec);

    for (const auto &manifest : manifests) {
        if (CookOneManifest(toolPath, manifest, outputRoot)) {
            ++cookResult.cookedCount;
        } else {
            cookResult.failedFiles.push_back(manifest.string());
        }
    }

    return cookResult;
}

void WriteStampFile(const std::filesystem::path &projectDir)
{
    const auto    stampPath = (projectDir / kBuildStamp).lexically_normal();
    std::ofstream ofs(stampPath);
    if (ofs) {
        const auto now = std::chrono::system_clock::now();
        ofs << std::format("{:%Y-%m-%dT%H:%M:%SZ}", now) << "\n";
    }
}

// ---------------------------------------------------------------------------
// Project file auto-discovery
// ---------------------------------------------------------------------------

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

} // namespace

// ---------------------------------------------------------------------------
// Static helper used by JzRunCommand
// ---------------------------------------------------------------------------

JzCliResult JzBuildCommand::BuildProject(JzCliContext                &context,
                                         const std::filesystem::path &projectPath,
                                         JzCliOutputFormat            format,
                                         const String               *toolOverride)
{
    const auto loadResult = context.LoadProject(projectPath);
    if (loadResult != JzEProjectResult::Success) {
        return JzCliResult::Error(JzCliExitCode::ProjectError,
                                  std::format("Failed to load project '{}': {}",
                                              projectPath.string(),
                                              static_cast<int>(loadResult)));
    }

    const auto &cfg        = context.GetProjectManager().GetConfig();
    const auto  sourceRoot = cfg.GetShaderSourcePath();
    const auto  outputRoot = cfg.GetShaderCookedPath();
    const auto  toolPath   = ResolveToolPath(toolOverride);

    auto cookResult = CookShaders(toolPath, sourceRoot, outputRoot);

    if (!cookResult.failedFiles.empty()) {
        std::ostringstream ss;
        ss << "Build failed: " << cookResult.failedFiles.size() << " shader(s) could not be cooked\n";
        for (const auto &f : cookResult.failedFiles) {
            ss << "  - " << f << "\n";
        }
        if (format == JzCliOutputFormat::Json) {
            Json payload;
            payload["ok"]           = false;
            payload["cooked"]       = cookResult.cookedCount;
            payload["total"]        = cookResult.totalCount;
            payload["failed_files"] = cookResult.failedFiles;
            return JzCliResult::Error(JzCliExitCode::ToolError, payload.dump(2));
        }
        return JzCliResult::Error(JzCliExitCode::ToolError, ss.str());
    }

    WriteStampFile(projectPath.parent_path());

    if (format == JzCliOutputFormat::Json) {
        Json payload;
        payload["ok"]      = true;
        payload["project"] = projectPath.string();
        payload["cooked"]  = cookResult.cookedCount;
        payload["total"]   = cookResult.totalCount;
        return JzCliResult::Ok(payload.dump(2));
    }

    return JzCliResult::Ok(
        std::format("Build complete: {}/{} shaders cooked", cookResult.cookedCount, cookResult.totalCount));
}

// ---------------------------------------------------------------------------
// JzCliDomainCommand interface
// ---------------------------------------------------------------------------

const String &JzBuildCommand::GetDomain() const
{
    return kDomain;
}

JzCliResult JzBuildCommand::Execute(JzCliContext              &context,
                                    const std::vector<String> &args,
                                    JzCliOutputFormat          format)
{
    if (args.empty() || args.front() == "--help" || args.front() == "-h") {
        return JzCliResult::Ok(BuildHelp());
    }

    auto parsed = JzCliArgParser::Parse(args);

    // Resolve project file
    std::filesystem::path projectPath;

    if (auto *projectFile = parsed.GetFirstValue("--project")) {
        projectPath = std::filesystem::path(*projectFile).lexically_normal();
    } else {
        // Try positional path first, then walk ancestors of CWD
        std::filesystem::path startDir = std::filesystem::current_path();
        if (!parsed.positionals.empty()) {
            startDir = std::filesystem::path(parsed.positionals.front());
            if (startDir.is_relative()) {
                startDir = std::filesystem::current_path() / startDir;
            }
            startDir = startDir.lexically_normal();
        }

        // If startDir looks like a .jzreproject file, use it directly
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
    }

    return BuildProject(context, projectPath, format, parsed.GetFirstValue("--tool"));
}

String JzBuildCommand::GetHelp() const
{
    return "  build    Cook project content (shaders, etc.)";
}

} // namespace JzRE
