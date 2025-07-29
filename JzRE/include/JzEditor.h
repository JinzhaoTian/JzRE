#pragma once

#include "JzAssetBrowser.h"
#include "JzAssetView.h"
#include "JzCanvas.h"
#include "JzConsole.h"
#include "JzContext.h"
#include "JzEditorActions.h"
#include "JzGameView.h"
#include "JzHierarchy.h"
#include "JzMaterialEditor.h"
#include "JzMenuBar.h"
#include "JzPanelsManager.h"
#include "JzSceneView.h"

namespace JzRE {
/**
 * @brief Handle the editor's logic
 */
class JzEditor {
public:
    /**
     * @brief Construct a new Editor object
     *
     * @param context
     */
    JzEditor(JzContext &context);

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

    /**
     * @brief Set UI manager
     */
    void SetUI();

    /**
     * @brief Handle the global shortcuts
     */
    void HandleGlobalShortcuts();

    /**
     * @brief Update the current editor mode
     */
    void UpdateCurrentEditorMode(F32 deltaTime);

    /**
     * @brief Render the views
     */
    void RenderViews(F32 deltaTime);

    /**
     * @brief Update the editor panels
     *
     * @param deltaTime
     */
    void UpdateEditorPanels(F32 deltaTime);

    /**
     * @brief Render the editor UI
     */
    void RenderEditorUI(F32 deltaTime);

private:
    U64             m_elapsedFrames = 0;
    JzContext      &m_context;
    JzCanvas        m_canvas;
    JzPanelsManager m_panelsManager;
    JzEditorActions m_editorActions;
};
} // namespace JzRE