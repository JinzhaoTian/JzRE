/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Function/Project/JzProjectConfig.h"

#include <functional>
#include <memory>
#include <optional>

namespace JzRE {

/**
 * @brief Result of a project operation.
 */
enum class JzEProjectResult : U8 {
    Success,
    FileNotFound,
    ParseError,
    WriteError,
    VersionMismatch,
    InvalidPath,
    AlreadyLoaded,
    NoProjectLoaded
};

/**
 * @brief Callback type for project lifecycle events.
 */
using JzProjectCallback = std::function<void(const JzProjectConfig &)>;

/**
 * @brief Manages project lifecycle: load, save, create, and switch projects.
 *
 * JzProjectManager is the central authority for project operations in the runtime.
 * It handles:
 * - Loading and parsing .jzreproject files
 * - Creating new projects with default configuration
 * - Saving project configuration changes
 * - Project switching with proper cleanup
 * - Notifying listeners of project state changes
 *
 * Usage:
 * @code
 * JzProjectManager manager;
 *
 * // Load an existing project
 * auto result = manager.LoadProject("path/to/MyGame.jzreproject");
 * if (result == JzEProjectResult::Success) {
 *     const auto& config = manager.GetConfig();
 *     // Use project configuration...
 * }
 *
 * // Create a new project
 * result = manager.CreateProject("path/to/NewProject", "My New Game");
 * @endcode
 */
class JzProjectManager {
public:
    JzProjectManager();
    ~JzProjectManager();

    // Non-copyable, movable
    JzProjectManager(const JzProjectManager &)            = delete;
    JzProjectManager &operator=(const JzProjectManager &) = delete;
    JzProjectManager(JzProjectManager &&) noexcept;
    JzProjectManager &operator=(JzProjectManager &&) noexcept;

    // === Project Operations ===

    /**
     * @brief Load a project from a .jzreproject file.
     * @param projectFilePath Path to the .jzreproject file
     * @return Result indicating success or failure reason
     */
    JzEProjectResult LoadProject(const std::filesystem::path &projectFilePath);

    /**
     * @brief Create a new project at the specified location.
     * @param projectDirectory Directory where the project will be created
     * @param projectName Human-readable name for the project
     * @return Result indicating success or failure reason
     */
    JzEProjectResult CreateProject(const std::filesystem::path &projectDirectory,
                                   const String                &projectName);

    /**
     * @brief Save the current project configuration to disk.
     * @return Result indicating success or failure reason
     */
    JzEProjectResult SaveProject();

    /**
     * @brief Save the current project to a new location.
     * @param newProjectFilePath New path for the project file
     * @return Result indicating success or failure reason
     */
    JzEProjectResult SaveProjectAs(const std::filesystem::path &newProjectFilePath);

    /**
     * @brief Close the currently loaded project.
     */
    void CloseProject();

    // === Query Methods ===

    /**
     * @brief Check if a project is currently loaded.
     */
    [[nodiscard]] Bool HasLoadedProject() const;

    /**
     * @brief Get the current project configuration (const).
     * @throws std::runtime_error if no project is loaded
     */
    [[nodiscard]] const JzProjectConfig &GetConfig() const;

    /**
     * @brief Get the current project configuration (mutable).
     * @throws std::runtime_error if no project is loaded
     */
    [[nodiscard]] JzProjectConfig &GetConfig();

    /**
     * @brief Get the path to the currently loaded project file.
     */
    [[nodiscard]] const std::filesystem::path &GetProjectFilePath() const;

    /**
     * @brief Get the absolute path to the project's content/asset directory.
     * @throws std::runtime_error if no project is loaded
     */
    [[nodiscard]] std::filesystem::path GetContentPath() const;

    /**
     * @brief Check if the project has unsaved changes.
     */
    [[nodiscard]] Bool HasUnsavedChanges() const;

    /**
     * @brief Mark the project as having unsaved changes.
     */
    void MarkDirty();

    // === Workspace Settings ===

    /**
     * @brief Load workspace settings for the current project.
     * @return Optional containing settings if found, empty otherwise
     */
    std::optional<JzProjectWorkspaceSettings> LoadWorkspaceSettings() const;

    /**
     * @brief Save workspace settings for the current project.
     * @param settings The workspace settings to save
     * @return Result indicating success or failure reason
     */
    JzEProjectResult SaveWorkspaceSettings(const JzProjectWorkspaceSettings &settings) const;

    // === Event Callbacks ===

    /**
     * @brief Register a callback for when a project is loaded.
     */
    void OnProjectLoaded(JzProjectCallback callback);

    /**
     * @brief Register a callback for when a project is about to be closed.
     */
    void OnProjectClosing(JzProjectCallback callback);

    /**
     * @brief Register a callback for when a project is saved.
     */
    void OnProjectSaved(JzProjectCallback callback);

    // === Static Utilities ===

    /**
     * @brief Generate a new unique project ID (UUID string).
     */
    static String GenerateProjectId();

    /**
     * @brief Get the project file extension.
     */
    static constexpr const char *GetProjectFileExtension()
    {
        return ".jzreproject";
    }

    /**
     * @brief Get the workspace settings file extension.
     */
    static constexpr const char *GetWorkspaceSettingsExtension()
    {
        return ".workspace";
    }

    /**
     * @brief Validate a project file without fully loading it.
     * @param projectFilePath Path to the project file
     * @return Result indicating if the file is valid
     */
    static JzEProjectResult ValidateProjectFile(const std::filesystem::path &projectFilePath);

private:
    /**
     * @brief Parse a project file and populate config.
     */
    JzEProjectResult ParseProjectFile(const std::filesystem::path &filePath,
                                      JzProjectConfig             &outConfig);

    /**
     * @brief Serialize config to JSON and write to file.
     */
    JzEProjectResult WriteProjectFile(const std::filesystem::path &filePath,
                                      const JzProjectConfig       &config);

    /**
     * @brief Create default directory structure for a new project.
     */
    Bool CreateProjectDirectories(const std::filesystem::path &projectRoot);

    /**
     * @brief Notify all registered callbacks.
     */
    void NotifyProjectLoaded();
    void NotifyProjectClosing();
    void NotifyProjectSaved();

private:
    std::unique_ptr<JzProjectConfig> m_config;
    std::filesystem::path            m_projectFilePath;
    Bool                             m_isDirty{false};

    std::vector<JzProjectCallback> m_onLoadedCallbacks;
    std::vector<JzProjectCallback> m_onClosingCallbacks;
    std::vector<JzProjectCallback> m_onSavedCallbacks;
};

} // namespace JzRE
