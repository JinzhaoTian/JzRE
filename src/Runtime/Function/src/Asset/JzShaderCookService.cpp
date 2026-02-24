/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#include "JzRE/Runtime/Function/Asset/JzShaderCookService.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <sstream>

#include <nlohmann/json.hpp>

#include "JzRE/Runtime/Core/JzLogger.h"
#include "JzRE/Runtime/Function/ECS/JzAssetSystem.h"

namespace JzRE {

namespace {

using Json = nlohmann::json;

Bool IsShaderSourceFile(const std::filesystem::path &path)
{
    auto ext = path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext == ".hlsl" || ext == ".hlsli";
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

} // namespace

JzShaderCookService::JzShaderCookService(const JzShaderCookServiceConfig &config) :
    m_config(config)
{ }

Bool JzShaderCookService::Initialize()
{
    namespace fs = std::filesystem;

    if (m_config.sourceRoot.empty()) {
        JzRE_LOG_WARN("JzShaderCookService: sourceRoot is empty");
        return false;
    }
    if (m_config.outputRoot.empty()) {
        JzRE_LOG_WARN("JzShaderCookService: outputRoot is empty");
        return false;
    }

    std::error_code ec;
    if (!fs::exists(m_config.sourceRoot, ec) || !fs::is_directory(m_config.sourceRoot, ec)) {
        JzRE_LOG_WARN("JzShaderCookService: sourceRoot does not exist: {}",
                      m_config.sourceRoot.string());
        return false;
    }

    fs::create_directories(m_config.outputRoot, ec);
    if (ec) {
        JzRE_LOG_WARN("JzShaderCookService: failed to create outputRoot '{}': {}",
                      m_config.outputRoot.string(), ec.message());
        return false;
    }

    m_resolvedShaderToolPath = ResolveShaderToolPath();
    if (m_resolvedShaderToolPath.empty() || !fs::exists(m_resolvedShaderToolPath, ec)) {
        JzRE_LOG_WARN("JzShaderCookService: JzREShaderTool not found");
        return false;
    }

    m_manifestTimestamps.clear();
    m_timeSinceLastScan = m_config.scanIntervalSeconds;
    m_initialized       = true;
    JzRE_LOG_INFO("JzShaderCookService: watching '{}' -> '{}'",
                  m_config.sourceRoot.string(), m_config.outputRoot.string());
    return true;
}

void JzShaderCookService::Shutdown()
{
    m_manifestTimestamps.clear();
    m_resolvedShaderToolPath.clear();
    m_timeSinceLastScan = 0.0f;
    m_initialized       = false;
}

void JzShaderCookService::Update(F32 deltaSeconds, JzAssetSystem &assetSystem)
{
    if (!m_initialized) {
        return;
    }

    m_timeSinceLastScan += deltaSeconds;
    if (m_timeSinceLastScan < std::max(0.05f, m_config.scanIntervalSeconds)) {
        return;
    }
    m_timeSinceLastScan = 0.0f;

    std::vector<std::filesystem::path> manifests;
    if (!ScanShaderManifests(manifests)) {
        return;
    }

    Bool anyCooked = false;
    for (const auto &manifestPath : manifests) {
        if (!ShouldRecookManifest(manifestPath)) {
            continue;
        }

        if (CookManifest(manifestPath)) {
            m_manifestTimestamps[manifestPath.lexically_normal().string()] =
                ComputeManifestDependencyTimestamp(manifestPath);
            anyCooked = true;
        }
    }

    if (anyCooked) {
        assetSystem.ForceHotReloadCheck();
    }
}

Bool JzShaderCookService::IsInitialized() const
{
    return m_initialized;
}

Bool JzShaderCookService::ScanShaderManifests(std::vector<std::filesystem::path> &outManifests) const
{
    namespace fs = std::filesystem;
    outManifests.clear();

    std::error_code ec;
    if (!fs::exists(m_config.sourceRoot, ec)) {
        return false;
    }

    for (fs::recursive_directory_iterator it(m_config.sourceRoot, fs::directory_options::skip_permission_denied, ec), end;
         it != end; it.increment(ec)) {
        if (ec) {
            continue;
        }
        if (!it->is_regular_file(ec)) {
            continue;
        }

        const auto filePath = it->path();
        auto       lowered  = filePath.filename().string();
        std::transform(lowered.begin(), lowered.end(), lowered.begin(), ::tolower);
        if (lowered.size() >= 18 && lowered.ends_with(".jzshader.src.json")) {
            outManifests.push_back(filePath.lexically_normal());
        }
    }

    std::sort(outManifests.begin(), outManifests.end());
    return true;
}

Bool JzShaderCookService::ShouldRecookManifest(const std::filesystem::path &manifestPath)
{
    const auto latestTimestamp = ComputeManifestDependencyTimestamp(manifestPath);
    const auto key             = manifestPath.lexically_normal().string();

    auto iter = m_manifestTimestamps.find(key);
    if (iter == m_manifestTimestamps.end()) {
        return true;
    }

    return latestTimestamp > iter->second;
}

Bool JzShaderCookService::CookManifest(const std::filesystem::path &manifestPath) const
{
    namespace fs = std::filesystem;

    std::error_code ec;
    fs::create_directories(m_config.outputRoot, ec);
    if (ec) {
        JzRE_LOG_ERROR("JzShaderCookService: failed to create output dir '{}': {}",
                       m_config.outputRoot.string(), ec.message());
        return false;
    }

    std::ostringstream command;
    command << Quote(m_resolvedShaderToolPath.string())
            << " --input " << Quote(manifestPath.string())
            << " --output-dir " << Quote(m_config.outputRoot.string());

    const auto commandString = command.str();
    JzRE_LOG_INFO("JzShaderCookService: cooking {}", manifestPath.string());
    const int result = std::system(commandString.c_str());
    if (result != 0) {
        JzRE_LOG_ERROR("JzShaderCookService: cook failed for '{}' (exit code {})",
                       manifestPath.string(), result);
        return false;
    }

    return true;
}

std::filesystem::file_time_type JzShaderCookService::ComputeManifestDependencyTimestamp(
    const std::filesystem::path &manifestPath) const
{
    namespace fs = std::filesystem;

    std::error_code ec;
    auto            latestTimestamp = fs::file_time_type::min();

    const auto updateLatest = [&latestTimestamp](const fs::path &filePath) {
        std::error_code localEc;
        if (!fs::exists(filePath, localEc) || !fs::is_regular_file(filePath, localEc)) {
            return;
        }
        const auto fileTime = fs::last_write_time(filePath, localEc);
        if (!localEc && fileTime > latestTimestamp) {
            latestTimestamp = fileTime;
        }
    };

    updateLatest(manifestPath);

    Json manifestJson;
    {
        std::ifstream stream(manifestPath);
        if (!stream.is_open()) {
            return latestTimestamp;
        }
        try {
            stream >> manifestJson;
        } catch (...) {
            return latestTimestamp;
        }
    }

    const auto manifestDir = manifestPath.parent_path();

    if (manifestJson.contains("stages") && manifestJson["stages"].is_object()) {
        for (auto it = manifestJson["stages"].begin(); it != manifestJson["stages"].end(); ++it) {
            if (!it.value().is_object() || !it.value().contains("file") || !it.value()["file"].is_string()) {
                continue;
            }

            fs::path stageFile = it.value()["file"].get<String>();
            if (stageFile.is_relative()) {
                stageFile = manifestDir / stageFile;
            }
            updateLatest(stageFile.lexically_normal());
        }
    }

    std::vector<fs::path> includeRoots;
    includeRoots.push_back(manifestDir);
    if (manifestJson.contains("includeDirs") && manifestJson["includeDirs"].is_array()) {
        for (const auto &entry : manifestJson["includeDirs"]) {
            if (!entry.is_string()) {
                continue;
            }

            fs::path includeDir = entry.get<String>();
            if (includeDir.is_relative()) {
                includeDir = manifestDir / includeDir;
            }
            includeRoots.push_back(includeDir.lexically_normal());
        }
    }

    for (const auto &root : includeRoots) {
        if (!fs::exists(root, ec) || !fs::is_directory(root, ec)) {
            continue;
        }

        for (fs::recursive_directory_iterator it(root, fs::directory_options::skip_permission_denied, ec), end;
             it != end; it.increment(ec)) {
            if (ec || !it->is_regular_file(ec)) {
                continue;
            }
            if (!IsShaderSourceFile(it->path())) {
                continue;
            }
            updateLatest(it->path());
        }
    }

    return latestTimestamp;
}

std::filesystem::path JzShaderCookService::ResolveShaderToolPath() const
{
    namespace fs = std::filesystem;

    if (!m_config.shaderToolPath.empty()) {
        return m_config.shaderToolPath.lexically_normal();
    }

    if (const char *envPath = std::getenv("JzRE_SHADER_TOOL_PATH")) {
        if (*envPath != '\0') {
            return fs::path(envPath).lexically_normal();
        }
    }

#ifdef _WIN32
    const char *toolName = "JzREShaderTool.exe";
#else
    const char *toolName = "JzREShaderTool";
#endif

    return (fs::current_path() / toolName).lexically_normal();
}

} // namespace JzRE
