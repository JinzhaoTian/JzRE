/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Editor/JzEditor.h"
#include "JzRE/Editor/JzAssetView.h"
#include "JzRE/Editor/JzSceneView.h"
#include "JzRE/Editor/JzGameView.h"
#include "JzRE/Editor/JzAssetBrowser.h"
#include "JzRE/Editor/JzConsole.h"
#include "JzRE/Editor/JzHierarchy.h"
#include "JzRE/Editor/JzMaterialEditor.h"

JzRE::JzEditor::JzEditor(JzRE::JzWindow &window) :
    m_window(window)
{
    m_uiManager     = std::make_unique<JzUIManager>(m_window);
    m_canvas        = std::make_unique<JzCanvas>();
    m_panelsManager = std::make_unique<JzPanelsManager>(*m_canvas);

    m_canvas->SetDockspace(true);

    m_panelsManager->CreatePanel<JzMenuBar>("Menu Bar", m_window);
    m_panelsManager->CreatePanel<JzAssetBrowser>("Asset Browser", true);
    m_panelsManager->CreatePanel<JzSceneView>("Scene View", true);
    m_panelsManager->CreatePanel<JzAssetView>("Asset View", true);
    m_panelsManager->CreatePanel<JzGameView>("Game View", true);
    m_panelsManager->CreatePanel<JzHierarchy>("Hierarchy", true);
    m_panelsManager->CreatePanel<JzConsole>("Console", true);
    m_panelsManager->CreatePanel<JzMaterialEditor>("Material Editor", true);

    m_panelsManager->GetPanelAs<JzMenuBar>("Menu Bar").InitializeSettingsMenu();

    const auto layoutConfigPath = std::filesystem::current_path() / "config" / "layout.ini";
    m_uiManager->ResetLayout(layoutConfigPath.string());
    m_uiManager->SetEditorLayoutSaveFilename("layout.ini");
    m_uiManager->EnableEditorLayoutSave(true);

    const auto fontPath = std::filesystem::current_path() / "fonts" / "SourceHanSansCN-Regular.otf";
    m_uiManager->LoadFont("sourcehansanscn-regular-18", fontPath.string(), 18);
    m_uiManager->LoadFont("sourcehansanscn-regular-16", fontPath.string(), 16);
    m_uiManager->LoadFont("sourcehansanscn-regular-14", fontPath.string(), 14);
    m_uiManager->UseFont("sourcehansanscn-regular-16");
    m_uiManager->SetDocking(true);

    m_uiManager->SetCanvas(*m_canvas);
}

JzRE::JzEditor::~JzEditor()
{
    if (m_uiManager) {
        m_uiManager.reset();
    }

    if (m_panelsManager) {
        m_panelsManager.reset();
    }

    if (m_canvas) {
        m_canvas.reset();
    }
}

void JzRE::JzEditor::PreUpdate() { }

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
    auto &sceneView = m_panelsManager->GetPanelAs<JzSceneView>("Scene View");

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
    auto &menuBar = m_panelsManager->GetPanelAs<JzMenuBar>("Menu Bar");
    // auto &sceneView = m_panelsManager.GetPanelAs<JzSceneView>("Scene View");

    menuBar.HandleShortcuts(deltaTime);
}

void JzRE::JzEditor::RenderEditorUI(JzRE::F32 deltaTime)
{
    m_uiManager->Render();
}