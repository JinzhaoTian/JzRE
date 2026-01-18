/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <filesystem>
#include "JzRE/Runtime/Core/JzRETypes.h"

namespace JzRE {

class JzResourceManager;

/**
 * @brief Initialization states for the engine context
 */
enum class JzEContextState {
    Uninitialized,      // Context not initialized
    EngineInitialized,  // Engine resources ready (factories registered, engine paths set)
    ProjectInitialized, // Project resources ready (Editor mode with project paths)
    Error               // Initialization failed
};

/**
 * @brief Central manager for engine runtime resource initialization
 *
 * JzContext coordinates the initialization of engine and project resources.
 * It follows a two-phase initialization pattern:
 *
 * Phase 1 (Engine): Register all resource factories, setup engine search paths
 *   - Called automatically by JzRERuntime during construction
 *   - Required for both standalone runtime and editor modes
 *
 * Phase 2 (Project): Setup project-specific search paths using conventions
 *   - Called by JzREInstance (Editor) when opening a project
 *   - Optional for standalone runtime applications
 *
 * Usage:
 * @code
 * // Automatic usage via JzRERuntime (most common)
 * class MyApp : public JzRERuntime {
 *     // Engine init happens in JzRERuntime constructor
 * };
 *
 * // Manual project initialization (Editor or custom scenarios)
 * auto& ctx = JzContext::GetInstance();
 * ctx.InitializeProject(resourceManager, "/path/to/project");
 * @endcode
 */
class JzContext {
public:
    /**
     * @brief Get singleton instance
     *
     * @return JzContext&
     */
    static JzContext &GetInstance();

    /**
     * @brief Initialize engine resources (Phase 1)
     *
     * Registers all resource factories (Texture, Mesh, Model, Material, Shader, Font)
     * and sets up engine-level search paths (./icons, ./shaders).
     *
     * Called automatically by JzRERuntime constructor. Safe to call multiple times
     * (subsequent calls are no-ops if already initialized).
     *
     * @param resourceManager The resource manager to register factories with
     *
     * @return Bool True if initialization succeeded or already initialized
     */
    Bool InitializeEngine(JzResourceManager &resourceManager);

    /**
     * @brief Initialize project resources (Phase 2)
     *
     * Sets up project-specific search paths using convention-based detection.
     * Automatically adds these paths if they exist:
     *   - {projectPath}/assets/
     *   - {projectPath}/textures/
     *   - {projectPath}/models/
     *   - {projectPath}/shaders/
     *   - {projectPath}/materials/
     *
     * Requires engine initialization first. Safe to call multiple times
     * (subsequent calls update the project path and search paths).
     *
     * @param resourceManager The resource manager to add search paths to
     * @param projectPath Root directory of the project
     *
     * @return Bool True if initialization succeeded
     */
    Bool InitializeProject(JzResourceManager           &resourceManager,
                           const std::filesystem::path &projectPath);

    /**
     * @brief Check if engine is initialized
     *
     * @return Bool True if engine initialization completed successfully
     */
    Bool IsEngineInitialized() const;

    /**
     * @brief Check if project is initialized
     *
     * @return Bool True if project initialization completed successfully
     */
    Bool IsProjectInitialized() const;

    /**
     * @brief Get current initialization state
     *
     * @return JzEContextState Current state
     */
    JzEContextState GetState() const;

    /**
     * @brief Get engine working directory
     *
     * @return std::filesystem::path Path to engine working directory
     */
    std::filesystem::path GetEnginePath() const;

    /**
     * @brief Get project root directory
     *
     * @return std::filesystem::path Path to project root, empty if no project loaded
     */
    std::filesystem::path GetProjectPath() const;

    /**
     * @brief Shutdown and reset context state
     *
     * Resets the context to uninitialized state. Does not unload resources
     * from the ResourceManager (they are reference-counted).
     */
    void Shutdown();

private:
    JzContext()                             = default;
    ~JzContext()                            = default;
    JzContext(const JzContext &)            = delete;
    JzContext &operator=(const JzContext &) = delete;

    /**
     * @brief Register all resource factories
     *
     * @param resourceManager Target resource manager
     */
    void RegisterAllFactories(JzResourceManager &resourceManager);

    /**
     * @brief Setup engine-level search paths
     *
     * @param resourceManager Target resource manager
     */
    void SetupEngineSearchPaths(JzResourceManager &resourceManager);

    /**
     * @brief Setup project-specific search paths using conventions
     *
     * @param resourceManager Target resource manager
     * @param projectPath Project root directory
     */
    void SetupProjectSearchPaths(JzResourceManager           &resourceManager,
                                 const std::filesystem::path &projectPath);

private:
    JzEContextState       m_state = JzEContextState::Uninitialized;
    std::filesystem::path m_enginePath;  // Working directory
    std::filesystem::path m_projectPath; // Project directory (optional)
};

} // namespace JzRE
