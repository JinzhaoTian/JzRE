/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Function/Project/JzProjectManager.h"

#include <nlohmann/json.hpp>

#include <fstream>
#include <random>
#include <sstream>
#include <stdexcept>

namespace JzRE {

namespace {

// JSON keys for project file serialization
namespace Keys {
constexpr const char *ProjectName       = "project_name";
constexpr const char *ProjectId         = "project_id";
constexpr const char *EngineVersion     = "engine_version";
constexpr const char *ContentRoot       = "content_root";
constexpr const char *ConfigRoot        = "config_root";
constexpr const char *DefaultScene      = "default_scene";
constexpr const char *StartupMode       = "startup_mode";
constexpr const char *RenderAPI         = "render_api";
constexpr const char *TargetPlatforms   = "target_platforms";
constexpr const char *AssetRegistry     = "asset_registry";
constexpr const char *ShaderCache       = "shader_cache";
constexpr const char *ShaderSourceRoot  = "shader_source_root";
constexpr const char *ShaderCookedRoot  = "shader_cooked_root";
constexpr const char *ShaderAutoCook    = "shader_auto_cook";
constexpr const char *BuildOutput       = "build_output";
constexpr const char *ImportRules       = "import_rules";
constexpr const char *Modules           = "modules";
constexpr const char *Plugins           = "plugins";
constexpr const char *PluginSearchPaths = "plugin_search_paths";
constexpr const char *ProjectVersion    = "project_version";
constexpr const char *MinCompatVersion  = "min_compatible_version";
} // namespace Keys

String StartupModeToString(JzEStartupMode mode)
{
    switch (mode) {
        case JzEStartupMode::Authoring: return "Authoring";
        case JzEStartupMode::Runtime: return "Runtime";
    }
    return "Authoring";
}

JzEStartupMode StringToStartupMode(const String &str)
{
    if (str == "Authoring") return JzEStartupMode::Authoring;
    // Backward compatibility for older project files.
    if (str == "Editor") return JzEStartupMode::Authoring;
    if (str == "Runtime") return JzEStartupMode::Runtime;
    return JzEStartupMode::Authoring;
}

String RenderAPIToString(JzERenderAPI api)
{
    switch (api) {
        case JzERenderAPI::Auto: return "Auto";
        case JzERenderAPI::OpenGL: return "OpenGL";
        case JzERenderAPI::Vulkan: return "Vulkan";
        case JzERenderAPI::D3D12: return "D3D12";
        case JzERenderAPI::Metal: return "Metal";
    }
    return "Auto";
}

JzERenderAPI StringToRenderAPI(const String &str)
{
    if (str == "OpenGL") return JzERenderAPI::OpenGL;
    if (str == "Vulkan") return JzERenderAPI::Vulkan;
    if (str == "D3D12") return JzERenderAPI::D3D12;
    if (str == "Metal") return JzERenderAPI::Metal;
    return JzERenderAPI::Auto;
}

String PlatformToString(JzETargetPlatform platform)
{
    switch (platform) {
        case JzETargetPlatform::Windows: return "Windows";
        case JzETargetPlatform::Linux: return "Linux";
        case JzETargetPlatform::MacOS: return "MacOS";
        case JzETargetPlatform::Android: return "Android";
        case JzETargetPlatform::iOS: return "iOS";
        case JzETargetPlatform::Web: return "Web";
    }
    return "Windows";
}

JzETargetPlatform StringToPlatform(const String &str)
{
    if (str == "Linux") return JzETargetPlatform::Linux;
    if (str == "MacOS") return JzETargetPlatform::MacOS;
    if (str == "Android") return JzETargetPlatform::Android;
    if (str == "iOS") return JzETargetPlatform::iOS;
    if (str == "Web") return JzETargetPlatform::Web;
    return JzETargetPlatform::Windows;
}

} // anonymous namespace

// ============================================================================
// Constructor / Destructor
// ============================================================================

JzProjectManager::JzProjectManager() = default;

JzProjectManager::~JzProjectManager()
{
    if (m_config) {
        NotifyProjectClosing();
    }
}

JzProjectManager::JzProjectManager(JzProjectManager &&) noexcept = default;

JzProjectManager &JzProjectManager::operator=(JzProjectManager &&) noexcept = default;

// ============================================================================
// Project Operations
// ============================================================================

JzEProjectResult JzProjectManager::LoadProject(const std::filesystem::path &projectFilePath)
{
    if (!std::filesystem::exists(projectFilePath)) {
        return JzEProjectResult::FileNotFound;
    }

    if (m_config) {
        NotifyProjectClosing();
    }

    auto newConfig = std::make_unique<JzProjectConfig>();
    auto result    = ParseProjectFile(projectFilePath, *newConfig);

    if (result != JzEProjectResult::Success) {
        return result;
    }

    // Set the root path based on the project file location
    newConfig->rootPath = projectFilePath.parent_path();

    m_config          = std::move(newConfig);
    m_projectFilePath = projectFilePath;
    m_isDirty         = false;

    NotifyProjectLoaded();

    return JzEProjectResult::Success;
}

JzEProjectResult JzProjectManager::CreateProject(const std::filesystem::path &projectDirectory,
                                                 const String                &projectName)
{
    if (!std::filesystem::exists(projectDirectory)) {
        std::error_code ec;
        if (!std::filesystem::create_directories(projectDirectory, ec)) {
            return JzEProjectResult::InvalidPath;
        }
    }

    if (m_config) {
        NotifyProjectClosing();
    }

    // Create default configuration
    auto newConfig           = std::make_unique<JzProjectConfig>();
    newConfig->projectName   = projectName;
    newConfig->projectId     = GenerateProjectId();
    newConfig->engineVersion = "1.0.0";
    newConfig->rootPath      = std::filesystem::absolute(projectDirectory);
    newConfig->startupMode   = JzEStartupMode::Authoring;
    newConfig->renderAPI     = JzERenderAPI::Auto;

    // Set default target platform based on current OS
#ifdef _WIN32
    newConfig->targetPlatforms.push_back(JzETargetPlatform::Windows);
#elif __APPLE__
    newConfig->targetPlatforms.push_back(JzETargetPlatform::MacOS);
#else
    newConfig->targetPlatforms.push_back(JzETargetPlatform::Linux);
#endif

    // Create default directory structure
    if (!CreateProjectDirectories(projectDirectory)) {
        return JzEProjectResult::WriteError;
    }

    // Construct project file path
    String sanitizedName = projectName;
    // Replace spaces and special characters for filename
    for (char &c : sanitizedName) {
        if (c == ' ' || c == '/' || c == '\\' || c == ':') {
            c = '_';
        }
    }
    m_projectFilePath = projectDirectory / (sanitizedName + GetProjectFileExtension());

    // Write the project file
    auto result = WriteProjectFile(m_projectFilePath, *newConfig);
    if (result != JzEProjectResult::Success) {
        return result;
    }

    m_config  = std::move(newConfig);
    m_isDirty = false;

    NotifyProjectLoaded();

    return JzEProjectResult::Success;
}

JzEProjectResult JzProjectManager::SaveProject()
{
    if (!m_config) {
        return JzEProjectResult::NoProjectLoaded;
    }

    auto result = WriteProjectFile(m_projectFilePath, *m_config);
    if (result == JzEProjectResult::Success) {
        m_isDirty = false;
        NotifyProjectSaved();
    }

    return result;
}

JzEProjectResult JzProjectManager::SaveProjectAs(const std::filesystem::path &newProjectFilePath)
{
    if (!m_config) {
        return JzEProjectResult::NoProjectLoaded;
    }

    auto result = WriteProjectFile(newProjectFilePath, *m_config);
    if (result == JzEProjectResult::Success) {
        m_projectFilePath  = newProjectFilePath;
        m_config->rootPath = newProjectFilePath.parent_path();
        m_isDirty          = false;
        NotifyProjectSaved();
    }

    return result;
}

void JzProjectManager::CloseProject()
{
    if (m_config) {
        NotifyProjectClosing();
        m_config.reset();
        m_projectFilePath.clear();
        m_isDirty = false;
    }
}

// ============================================================================
// Query Methods
// ============================================================================

Bool JzProjectManager::HasLoadedProject() const
{
    return m_config != nullptr;
}

const JzProjectConfig &JzProjectManager::GetConfig() const
{
    if (!m_config) {
        throw std::runtime_error("No project is currently loaded");
    }
    return *m_config;
}

JzProjectConfig &JzProjectManager::GetConfig()
{
    if (!m_config) {
        throw std::runtime_error("No project is currently loaded");
    }
    return *m_config;
}

const std::filesystem::path &JzProjectManager::GetProjectFilePath() const
{
    return m_projectFilePath;
}

std::filesystem::path JzProjectManager::GetContentPath() const
{
    return GetConfig().GetContentPath();
}

Bool JzProjectManager::HasUnsavedChanges() const
{
    return m_isDirty;
}

void JzProjectManager::MarkDirty()
{
    m_isDirty = true;
}

// ============================================================================
// Workspace Settings
// ============================================================================

std::optional<JzProjectWorkspaceSettings> JzProjectManager::LoadWorkspaceSettings() const
{
    if (!m_config) {
        return std::nullopt;
    }

    auto workspacePath = m_projectFilePath;
    workspacePath.replace_extension(GetWorkspaceSettingsExtension());
    Bool loadedFromLegacyEditorFile = false;

    // Backward compatibility: load legacy ".editor" settings when ".workspace" does not exist.
    if (!std::filesystem::exists(workspacePath)) {
        auto legacyPath = m_projectFilePath;
        legacyPath.replace_extension(".editor");
        if (!std::filesystem::exists(legacyPath)) {
            return std::nullopt;
        }
        workspacePath              = std::move(legacyPath);
        loadedFromLegacyEditorFile = true;
    }

    try {
        std::ifstream file(workspacePath);
        if (!file.is_open()) {
            return std::nullopt;
        }

        nlohmann::json json;
        file >> json;

        JzProjectWorkspaceSettings settings;

        if (json.contains("workspace_layout")) {
            settings.workspaceLayout = json["workspace_layout"].get<String>();
        } else if (json.contains("editor_layout")) {
            settings.workspaceLayout = json["editor_layout"].get<String>();
        }
        if (json.contains("recent_scenes")) {
            settings.recentScenes = json["recent_scenes"].get<std::vector<String>>();
        }
        if (json.contains("workspace_settings_file")) {
            settings.workspaceSettingsFile = json["workspace_settings_file"].get<String>();
        } else if (json.contains("editor_settings_file")) {
            settings.workspaceSettingsFile = json["editor_settings_file"].get<String>();
        }

        if (loadedFromLegacyEditorFile) {
            // Migrate legacy editor settings to the new workspace format on first successful load.
            (void)SaveWorkspaceSettings(settings);
        }

        return settings;
    } catch (...) {
        return std::nullopt;
    }
}

JzEProjectResult JzProjectManager::SaveWorkspaceSettings(const JzProjectWorkspaceSettings &settings) const
{
    if (!m_config) {
        return JzEProjectResult::NoProjectLoaded;
    }

    auto workspacePath = m_projectFilePath;
    workspacePath.replace_extension(GetWorkspaceSettingsExtension());

    try {
        nlohmann::json json;
        json["workspace_layout"]        = settings.workspaceLayout.string();
        json["recent_scenes"]           = settings.recentScenes;
        json["workspace_settings_file"] = settings.workspaceSettingsFile.string();

        std::ofstream file(workspacePath);
        if (!file.is_open()) {
            return JzEProjectResult::WriteError;
        }

        file << json.dump(4);
        return JzEProjectResult::Success;
    } catch (...) {
        return JzEProjectResult::WriteError;
    }
}

// ============================================================================
// Event Callbacks
// ============================================================================

void JzProjectManager::OnProjectLoaded(JzProjectCallback callback)
{
    m_onLoadedCallbacks.push_back(std::move(callback));
}

void JzProjectManager::OnProjectClosing(JzProjectCallback callback)
{
    m_onClosingCallbacks.push_back(std::move(callback));
}

void JzProjectManager::OnProjectSaved(JzProjectCallback callback)
{
    m_onSavedCallbacks.push_back(std::move(callback));
}

// ============================================================================
// Static Utilities
// ============================================================================

String JzProjectManager::GenerateProjectId()
{
    static std::random_device                 rd;
    static std::mt19937_64                    gen(rd());
    static std::uniform_int_distribution<U64> dis;

    std::stringstream ss;
    ss << std::hex << dis(gen) << dis(gen);
    return ss.str();
}

JzEProjectResult JzProjectManager::ValidateProjectFile(const std::filesystem::path &projectFilePath)
{
    if (!std::filesystem::exists(projectFilePath)) {
        return JzEProjectResult::FileNotFound;
    }

    try {
        std::ifstream file(projectFilePath);
        if (!file.is_open()) {
            return JzEProjectResult::FileNotFound;
        }

        nlohmann::json json;
        file >> json;

        // Check for required fields
        if (!json.contains(Keys::ProjectName) || !json.contains(Keys::ProjectId)) {
            return JzEProjectResult::ParseError;
        }

        return JzEProjectResult::Success;
    } catch (const nlohmann::json::parse_error &) {
        return JzEProjectResult::ParseError;
    } catch (...) {
        return JzEProjectResult::ParseError;
    }
}

// ============================================================================
// Private Methods
// ============================================================================

JzEProjectResult JzProjectManager::ParseProjectFile(const std::filesystem::path &filePath,
                                                    JzProjectConfig             &outConfig)
{
    try {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            return JzEProjectResult::FileNotFound;
        }

        nlohmann::json json;
        file >> json;

        // Required fields
        if (!json.contains(Keys::ProjectName) || !json.contains(Keys::ProjectId)) {
            return JzEProjectResult::ParseError;
        }

        outConfig.projectName = json[Keys::ProjectName].get<String>();
        outConfig.projectId   = json[Keys::ProjectId].get<String>();

        // Optional fields with defaults
        if (json.contains(Keys::EngineVersion)) {
            outConfig.engineVersion = json[Keys::EngineVersion].get<String>();
        }
        if (json.contains(Keys::ContentRoot)) {
            outConfig.contentRoot = json[Keys::ContentRoot].get<String>();
        }
        if (json.contains(Keys::ConfigRoot)) {
            outConfig.configRoot = json[Keys::ConfigRoot].get<String>();
        }
        if (json.contains(Keys::DefaultScene)) {
            outConfig.defaultScene = json[Keys::DefaultScene].get<String>();
        }
        if (json.contains(Keys::StartupMode)) {
            outConfig.startupMode = StringToStartupMode(json[Keys::StartupMode].get<String>());
        }
        if (json.contains(Keys::RenderAPI)) {
            outConfig.renderAPI = StringToRenderAPI(json[Keys::RenderAPI].get<String>());
        }
        if (json.contains(Keys::TargetPlatforms)) {
            for (const auto &p : json[Keys::TargetPlatforms]) {
                outConfig.targetPlatforms.push_back(StringToPlatform(p.get<String>()));
            }
        }
        if (json.contains(Keys::AssetRegistry)) {
            outConfig.assetRegistry = json[Keys::AssetRegistry].get<String>();
        }
        if (json.contains(Keys::ShaderCache)) {
            outConfig.shaderCache = json[Keys::ShaderCache].get<String>();
        }
        if (json.contains(Keys::ShaderSourceRoot)) {
            outConfig.shaderSourceRoot = json[Keys::ShaderSourceRoot].get<String>();
        }
        if (json.contains(Keys::ShaderCookedRoot)) {
            outConfig.shaderCookedRoot = json[Keys::ShaderCookedRoot].get<String>();
        }
        if (json.contains(Keys::ShaderAutoCook)) {
            outConfig.shaderAutoCook = json[Keys::ShaderAutoCook].get<Bool>();
        }
        if (json.contains(Keys::BuildOutput)) {
            outConfig.buildOutput = json[Keys::BuildOutput].get<String>();
        }
        if (json.contains(Keys::ImportRules)) {
            for (const auto &rule : json[Keys::ImportRules]) {
                JzImportRule r;
                r.extension = rule["extension"].get<String>();
                r.factory   = rule["factory"].get<String>();
                outConfig.importRules.push_back(r);
            }
        }
        if (json.contains(Keys::Modules)) {
            outConfig.modules = json[Keys::Modules].get<std::vector<String>>();
        }
        if (json.contains(Keys::Plugins)) {
            for (const auto &plugin : json[Keys::Plugins]) {
                JzPluginEntry entry;
                entry.name = plugin["name"].get<String>();
                if (plugin.contains("version")) {
                    entry.version = plugin["version"].get<String>();
                }
                if (plugin.contains("enabled")) {
                    entry.enabled = plugin["enabled"].get<Bool>();
                }
                outConfig.plugins.push_back(entry);
            }
        }
        if (json.contains(Keys::PluginSearchPaths)) {
            for (const auto &path : json[Keys::PluginSearchPaths]) {
                outConfig.pluginSearchPaths.emplace_back(path.get<String>());
            }
        }
        if (json.contains(Keys::ProjectVersion)) {
            outConfig.projectVersion = json[Keys::ProjectVersion].get<U32>();
        }
        if (json.contains(Keys::MinCompatVersion)) {
            outConfig.minCompatibleVersion = json[Keys::MinCompatVersion].get<String>();
        }

        return JzEProjectResult::Success;
    } catch (const nlohmann::json::parse_error &) {
        return JzEProjectResult::ParseError;
    } catch (...) {
        return JzEProjectResult::ParseError;
    }
}

JzEProjectResult JzProjectManager::WriteProjectFile(const std::filesystem::path &filePath,
                                                    const JzProjectConfig       &config)
{
    try {
        nlohmann::json json;

        // Required fields
        json[Keys::ProjectName] = config.projectName;
        json[Keys::ProjectId]   = config.projectId;

        // Optional fields
        json[Keys::EngineVersion] = config.engineVersion;
        json[Keys::ContentRoot]   = config.contentRoot.string();
        json[Keys::ConfigRoot]    = config.configRoot.string();
        json[Keys::DefaultScene]  = config.defaultScene;
        json[Keys::StartupMode]   = StartupModeToString(config.startupMode);
        json[Keys::RenderAPI]     = RenderAPIToString(config.renderAPI);

        // Target platforms
        nlohmann::json platforms = nlohmann::json::array();
        for (const auto &p : config.targetPlatforms) {
            platforms.push_back(PlatformToString(p));
        }
        json[Keys::TargetPlatforms] = platforms;

        // Resource paths
        json[Keys::AssetRegistry] = config.assetRegistry.string();
        json[Keys::ShaderCache]   = config.shaderCache.string();
        json[Keys::ShaderSourceRoot] = config.shaderSourceRoot.string();
        json[Keys::ShaderCookedRoot] = config.shaderCookedRoot.string();
        json[Keys::ShaderAutoCook]   = config.shaderAutoCook;
        json[Keys::BuildOutput]   = config.buildOutput.string();

        // Import rules
        nlohmann::json rules = nlohmann::json::array();
        for (const auto &rule : config.importRules) {
            nlohmann::json r;
            r["extension"] = rule.extension;
            r["factory"]   = rule.factory;
            rules.push_back(r);
        }
        json[Keys::ImportRules] = rules;

        // Modules
        json[Keys::Modules] = config.modules;

        // Plugins
        nlohmann::json plugins = nlohmann::json::array();
        for (const auto &plugin : config.plugins) {
            nlohmann::json p;
            p["name"]    = plugin.name;
            p["version"] = plugin.version;
            p["enabled"] = plugin.enabled;
            plugins.push_back(p);
        }
        json[Keys::Plugins] = plugins;

        // Plugin search paths
        nlohmann::json paths = nlohmann::json::array();
        for (const auto &path : config.pluginSearchPaths) {
            paths.push_back(path.string());
        }
        json[Keys::PluginSearchPaths] = paths;

        // Version info
        json[Keys::ProjectVersion]   = config.projectVersion;
        json[Keys::MinCompatVersion] = config.minCompatibleVersion;

        // Write to file
        std::ofstream file(filePath);
        if (!file.is_open()) {
            return JzEProjectResult::WriteError;
        }

        file << json.dump(4);
        return JzEProjectResult::Success;
    } catch (...) {
        return JzEProjectResult::WriteError;
    }
}

Bool JzProjectManager::CreateProjectDirectories(const std::filesystem::path &projectRoot)
{
    std::error_code ec;

    // Create Content directory
    std::filesystem::create_directories(projectRoot / "Content", ec);
    if (ec) return false;

    // Create shader source and cooked output directories.
    std::filesystem::create_directories(projectRoot / "Content" / "Shaders" / "src", ec);
    if (ec) return false;
    std::filesystem::create_directories(projectRoot / "Content" / "Shaders", ec);
    if (ec) return false;

    // Create Config directory
    std::filesystem::create_directories(projectRoot / "Config", ec);
    if (ec) return false;

    // Create Intermediate directory for caches
    std::filesystem::create_directories(projectRoot / "Intermediate" / "ShaderCache", ec);
    if (ec) return false;

    // Create Build directory
    std::filesystem::create_directories(projectRoot / "Build", ec);
    if (ec) return false;

    return true;
}

void JzProjectManager::NotifyProjectLoaded()
{
    if (m_config) {
        for (const auto &callback : m_onLoadedCallbacks) {
            callback(*m_config);
        }
    }
}

void JzProjectManager::NotifyProjectClosing()
{
    if (m_config) {
        for (const auto &callback : m_onClosingCallbacks) {
            callback(*m_config);
        }
    }
}

void JzProjectManager::NotifyProjectSaved()
{
    if (m_config) {
        for (const auto &callback : m_onSavedCallbacks) {
            callback(*m_config);
        }
    }
}

} // namespace JzRE
