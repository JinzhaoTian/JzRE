/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Editor/JzEditor.h"
#include "JzRE/Editor/Panels/JzAssetView.h"
#include "JzRE/Editor/Panels/JzSceneView.h"
#include "JzRE/Editor/Panels/JzGameView.h"
#include "JzRE/Editor/Panels/JzAssetBrowser.h"
#include "JzRE/Editor/Panels/JzConsole.h"
#include "JzRE/Editor/Panels/JzHierarchy.h"
#include "JzRE/Editor/Panels/JzMaterialEditor.h"
#include "JzRE/Editor/Panels/JzMenuBar.h"
#include "JzRE/Runtime/JzRERuntime.h"

JzRE::JzEditor::JzEditor(JzRE::JzWindowSystem &windowSystem) :
    m_windowSystem(windowSystem),
    m_runtime(nullptr)
{
    m_uiManager     = std::make_unique<JzUIManager>(m_windowSystem);
    m_canvas        = std::make_unique<JzCanvas>();
    m_panelsManager = std::make_unique<JzPanelsManager>(*m_canvas);

    m_canvas->SetDockspace(true);

    InitializePanels();
}

JzRE::JzEditor::JzEditor(JzRE::JzRERuntime &runtime) :
    m_windowSystem(runtime.GetWindowSystem()),
    m_runtime(&runtime)
{
    m_uiManager     = std::make_unique<JzUIManager>(m_windowSystem);
    m_canvas        = std::make_unique<JzCanvas>();
    m_panelsManager = std::make_unique<JzPanelsManager>(*m_canvas);

    m_canvas->SetDockspace(true);

    InitializePanels();
}

void JzRE::JzEditor::InitializePanels()
{
    m_panelsManager->CreatePanel<JzMenuBar>("Menu Bar", m_windowSystem);
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
    m_uiManager->SetEditorLayoutSaveFilename(layoutConfigPath.string());
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

JzRE::JzPanelsManager &JzRE::JzEditor::GetPanelsManager()
{
    return *m_panelsManager;
}

JzRE::JzUIManager &JzRE::JzEditor::GetUIManager()
{
    return *m_uiManager;
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
    auto &sceneView = m_panelsManager->GetPanelAs<JzSceneView>("Scene View");

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