/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzEditor.h"
#include "JzMenuBar.h"
#include "JzSceneView.h"
#include "JzConsole.h"
#include "JzHierarchy.h"
#include "JzMaterialEditor.h"
#include "JzAssetBrowser.h"

JzRE::JzEditor::JzEditor() :
    m_context(JzRE_CONTEXT()), // injection
    m_panelsManager(m_canvas)
{
    m_panelsManager.CreatePanel<JzMenuBar>("Menu Bar");
    m_panelsManager.CreatePanel<JzAssetBrowser>("Asset Browser", true);
    m_panelsManager.CreatePanel<JzSceneView>("Scene View", true);
    // m_panelsManager.CreatePanel<JzAssetView>("Asset View", true);
    // m_panelsManager.CreatePanel<JzGameView>("Game View", true);
    m_panelsManager.CreatePanel<JzHierarchy>("Hierarchy", true);
    m_panelsManager.CreatePanel<JzConsole>("Console", true);
    m_panelsManager.CreatePanel<JzMaterialEditor>("Material Editor", true);

    // Initialize menu bar after service registration
    m_panelsManager.GetPanelAs<JzMenuBar>("Menu Bar").InitializeSettingsMenu();

    m_canvas.SetDockspace(true);
    m_context.uiManager->SetCanvas(m_canvas);

    m_context.sceneManager.LoadDefaultScene();
}

JzRE::JzEditor::~JzEditor()
{
    m_context.sceneManager.UnloadCurrentScene();
}

void JzRE::JzEditor::PreUpdate()
{
    m_context.window->PollEvents();
}

void JzRE::JzEditor::Update(JzRE::F32 deltaTime)
{
    HandleGlobalShortcuts();
    UpdateCurrentEditorMode(deltaTime);
    RenderViews(deltaTime);
    UpdateEditorPanels(deltaTime);
    RenderEditorUI(deltaTime);
}

void JzRE::JzEditor::PostUpdate()
{
    m_context.window->SwapBuffers();
    m_context.inputManager->ClearEvents();
    ++m_elapsedFrames;
}

void JzRE::JzEditor::HandleGlobalShortcuts()
{
    // TODO
}

void JzRE::JzEditor::UpdateCurrentEditorMode(JzRE::F32 deltaTime)
{
    // TODO
}

void JzRE::JzEditor::RenderViews(JzRE::F32 deltaTime)
{
    // auto &assetView = m_panelsManager.GetPanelAs<JzAssetView>("Asset View");
    auto &sceneView = m_panelsManager.GetPanelAs<JzSceneView>("Scene View");

    // assetView.Update(deltaTime);
    if (sceneView.IsOpened()) {
        sceneView.Update(deltaTime);
    }

    if (sceneView.IsOpened() && sceneView.IsVisible()) {
        sceneView.Render();
    }
}

void JzRE::JzEditor::UpdateEditorPanels(JzRE::F32 deltaTime)
{
    auto &menuBar = m_panelsManager.GetPanelAs<JzMenuBar>("Menu Bar");
    // auto &sceneView = m_panelsManager.GetPanelAs<JzSceneView>("Scene View");

    menuBar.HandleShortcuts(deltaTime);
}

void JzRE::JzEditor::RenderEditorUI(JzRE::F32 deltaTime)
{
    m_context.uiManager->Render();
}