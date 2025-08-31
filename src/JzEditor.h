#pragma once

#include "JzAssetBrowser.h"
#include "JzAssetView.h"
#include "JzCanvas.h"
#include "JzConsole.h"
#include "JzContext.h"
#include "JzGameView.h"
#include "JzHierarchy.h"
#include "JzMaterialEditor.h"
#include "JzMenuBar.h"
#include "JzPanelWindowSettings.h"
#include "JzPanelsManager.h"
#include "JzSceneView.h"

namespace JzRE {
/**
 * @brief Editor of JzRE
 */
class JzEditor {
public:
    /**
     * @brief Construct a new Editor object
     */
    JzEditor();

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
    U64             m_elapsedFrames = 0;
    JzContext      &m_context;
    JzCanvas        m_canvas;
    JzPanelsManager m_panelsManager;
};
} // namespace JzRE