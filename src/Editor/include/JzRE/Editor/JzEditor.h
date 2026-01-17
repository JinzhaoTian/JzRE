/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Function/Window/JzWindow.h"
#include "JzRE/Editor/JzCanvas.h"
#include "JzRE/Editor/JzPanelsManager.h"
#include "JzRE/Editor/JzUIManager.h"

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
 * 1. Standalone with window: JzEditor(window) - manages its own update loop
 * 2. With runtime: JzEditor(runtime) - integrates with runtime's render loop
 */
class JzEditor {
public:
    /**
     * @brief Construct editor with a window reference
     *
     * @param window Reference to the window
     */
    explicit JzEditor(JzWindow &window);

    /**
     * @brief Construct editor with a runtime reference
     *
     * This constructor allows the editor to access runtime components
     * like the renderer, scene, and input manager directly.
     *
     * @param runtime Reference to the runtime
     */
    explicit JzEditor(JzRERuntime &runtime);

    /**
     * @brief Destroy the Editor object
     */
    ~JzEditor();

    /**
     * @brief Prepare the frame
     */
    void PreUpdate();

    /**
     * @brief Editor main loop
     *
     * @param deltaTime Time since last frame in seconds
     */
    void Update(F32 deltaTime);

    /**
     * @brief Swapping the buffers
     */
    void PostUpdate();

    /**
     * @brief Get the panels manager
     *
     * @return JzPanelsManager& Reference to the panels manager
     */
    JzPanelsManager &GetPanelsManager();

    /**
     * @brief Get the UI manager
     *
     * @return JzUIManager& Reference to the UI manager
     */
    JzUIManager &GetUIManager();

private:
    void InitializePanels();
    void HandleGlobalShortcuts();
    void UpdateCurrentEditorMode(F32 deltaTime);
    void RenderViews(F32 deltaTime);
    void UpdateEditorPanels(F32 deltaTime);
    void RenderEditorUI(F32 deltaTime);

private:
    JzWindow                        &m_window;
    JzRERuntime                     *m_runtime = nullptr;
    std::unique_ptr<JzCanvas>        m_canvas;
    std::unique_ptr<JzPanelsManager> m_panelsManager;
    std::unique_ptr<JzUIManager>     m_uiManager;
    U64                              m_elapsedFrames = 0;
};
} // namespace JzRE