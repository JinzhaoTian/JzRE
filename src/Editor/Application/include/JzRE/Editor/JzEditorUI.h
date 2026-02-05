/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Editor/Panels/JzCanvas.h"
#include "JzRE/Editor/Panels/JzUIManager.h"
#include "JzRE/Editor/Panels/JzPanelsManager.h"

namespace JzRE {

// Forward declaration
class JzRERuntime;

/**
 * @brief Editor of JzRE
 *
 * The editor provides ImGui-based UI panels for scene editing, asset browsing,
 * and other development tools. It can work with JzRERuntime to provide a
 * complete editor experience.
 *
 * Usage:
 * JzEditorUI(runtime) - integrates with runtime's render loop.
 * JzWindowSystem is retrieved via JzServiceContainer (dependency injection).
 */
class JzEditorUI {
public:
    /**
     * @brief Construct editor with a runtime reference
     *
     * This constructor allows the editor to access runtime components
     * like the renderer, scene, and input manager directly.
     * JzWindowSystem is retrieved from JzServiceContainer.
     *
     * @param runtime Reference to the runtime
     */
    explicit JzEditorUI(JzRERuntime &runtime);

    /**
     * @brief Destroy the Editor object
     */
    ~JzEditorUI();

    /**
     * @brief Update editor logic (shortcuts, mode updates, panel updates)
     *
     * @param deltaTime Time since last frame in seconds
     */
    void Update(F32 deltaTime);

    /**
     * @brief Render editor views and UI
     *
     * This should be called after game systems have been updated and before Present().
     * Rendering order: Game scene (via RenderSystem) -> SceneView -> ImGui UI
     *
     * @param deltaTime Time since last frame in seconds
     */
    void Render(F32 deltaTime);

private:
    void InitializePanels();
    void HandleGlobalShortcuts();
    void UpdateCurrentEditorMode(F32 deltaTime);
    void UpdateEditorPanels(F32 deltaTime);
    void RenderEditorUI(F32 deltaTime);

private:
    JzRERuntime                     *m_runtime = nullptr;
    std::unique_ptr<JzCanvas>        m_canvas;
    std::unique_ptr<JzPanelsManager> m_panelsManager;
    std::unique_ptr<JzUIManager>     m_uiManager;
};
} // namespace JzRE