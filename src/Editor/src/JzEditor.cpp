/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Core/JzServiceContainer.h"
#include "JzRE/Editor/JzEditor.h"
#include "JzRE/Editor/JzPanelsManager.h"
#include "JzRE/Editor/JzSceneManager.h"
#include "JzRE/Editor/JzAssetBrowser.h"
#include "JzRE/Editor/JzAssetView.h"
#include "JzRE/Editor/JzConsole.h"
#include "JzRE/Editor/JzHierarchy.h"
#include "JzRE/Editor/JzMaterialEditor.h"
#include "JzRE/Editor/JzSceneView.h"
#include "JzRE/Editor/JzGameView.h"
#include "JzRE/Editor/JzUIManager.h"

JzRE::JzEditor::JzEditor(JzRE::JzWindow &window) :
    m_window(window),
    m_panelsManager(m_canvas)
{
    m_panelsManager.CreatePanel<JzMenuBar>("Menu Bar", m_window);

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

    auto &uiMgr = JzServiceContainer::Get<JzUIManager>();
    uiMgr.SetCanvas(m_canvas);

    auto &sceneMgr = JzServiceContainer::Get<JzSceneManager>();
    sceneMgr.LoadDefaultScene();
}

JzRE::JzEditor::~JzEditor()
{
    auto &sceneMgr = JzServiceContainer::Get<JzSceneManager>();
    sceneMgr.UnloadCurrentScene();
}

void JzRE::JzEditor::PreUpdate() { }

void JzRE::JzEditor::Update(JzRE::F32 deltaTime)
{
    HandleGlobalShortcuts();
    UpdateCurrentEditorMode(deltaTime);
    RenderViews(deltaTime);
    UpdateEditorPanels(deltaTime);
}

void JzRE::JzEditor::PostUpdate()
{
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

void JzRE::JzEditor::RenderEditorUI(JzRE::F32 deltaTime) { }