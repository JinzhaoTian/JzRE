/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Editor/JzWindow.h"
#include "JzRE/Editor/JzCanvas.h"
#include "JzRE/Editor/JzPanelsManager.h"
#include "JzRE/Editor/JzUIManager.h"

namespace JzRE {
/**
 * @brief Editor of JzRE
 */
class JzEditor {
public:
    /**
     * @brief Construct a new Editor object
     */
    JzEditor(JzWindow &window);

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
     * @param deltaTime
     */
    void Update(F32 deltaTime);

    /**
     * @brief Swapping the buffers
     */
    void PostUpdate();

private:
    void HandleGlobalShortcuts();
    void UpdateCurrentEditorMode(F32 deltaTime);
    void RenderViews(F32 deltaTime);
    void UpdateEditorPanels(F32 deltaTime);
    void RenderEditorUI(F32 deltaTime);

private:
    JzWindow                        &m_window;
    std::unique_ptr<JzCanvas>        m_canvas;
    std::unique_ptr<JzPanelsManager> m_panelsManager;
    std::unique_ptr<JzUIManager>     m_uiManager;
    U64                              m_elapsedFrames = 0;
};
} // namespace JzRE