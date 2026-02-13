/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <filesystem>
#include "JzRE/Runtime/JzRERuntime.h"
#include "JzRE/Editor/JzEditorUI.h"

namespace JzRE {

/**
 * @brief JzRE Editor
 *
 * This class extends JzRERuntime to provide a complete editor experience.
 * It integrates the JzEditorUI with the runtime's rendering pipeline.
 *
 * The inheritance from JzRERuntime allows:
 * - Reusing all runtime functionality (window, device, renderer, scene, input)
 * - Injecting editor UI rendering via the OnRender hook
 * - Sharing the same architecture for both standalone and editor modes
 *
 * Project Loading:
 * - If openPath is a .jzreproject file, loads the project directly
 * - If openPath is a directory, searches for .jzreproject file in that directory
 * - Project configuration drives asset paths and runtime settings
 */
class JzREEditor : public JzRERuntime {
public:
    /**
     * @brief Constructor
     *
     * @param rhiType The RHI type to use for rendering
     * @param openPath Path to a .jzreproject file or a directory containing one
     */
    JzREEditor(JzERHIType rhiType, const std::filesystem::path &openPath);

    /**
     * @brief Destructor
     */
    ~JzREEditor() override;

    /**
     * @brief Get the editor UI instance
     *
     * @return JzEditorUI& Reference to the editor UI
     */
    JzEditorUI &GetEditorUI();

protected:
    /**
     * @brief Called before the main loop starts
     *
     * Initializes editor-specific components and registers SceneView render passes.
     */
    void OnStart() override;

    /**
     * @brief Called during the update phase for editor logic
     *
     * @param deltaTime Time elapsed since the last frame in seconds
     *
     * Updates editor logic (shortcuts, mode updates, panel updates).
     * Called before UpdateSystems() in the main loop.
     */
    void OnUpdate(F32 deltaTime) override;

    /**
     * @brief Called during the render phase for editor rendering
     *
     * @param deltaTime Time elapsed since the last frame in seconds
     *
     * Renders editor views and ImGui UI.
     * Called after UpdateSystems() and before Present().
     */
    void OnRender(F32 deltaTime) override;

    /**
     * @brief Called after the main loop ends
     *
     * Cleans up editor-specific components and render pass resources.
     */
    void OnStop() override;

public:
    /**
     * @brief Find a .jzreproject file in the given path
     *
     * @param path Directory or file path
     * @return std::filesystem::path Path to project file, or empty if not found
     */
    static std::filesystem::path FindProjectFile(const std::filesystem::path &path);

private:
    /**
     * @brief GPU resources owned by editor render passes.
     */
    struct JzEditorRenderPassResources;

    /**
     * @brief Build and register SceneView render passes (skybox/grid/axis).
     */
    void InitializeEditorRenderPasses();

    /**
     * @brief Unregister render passes and release owned GPU resources.
     */
    void ReleaseEditorRenderPasses();

    std::filesystem::path       m_openPath;
    std::unique_ptr<JzEditorUI> m_editorUI;
    std::unique_ptr<JzEditorRenderPassResources> m_editorRenderPassResources;
};
} // namespace JzRE
