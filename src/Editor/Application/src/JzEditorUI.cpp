/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Editor/JzEditorUI.h"
#include "JzRE/Editor/Panels/JzAssetView.h"
#include "JzRE/Editor/Panels/JzSceneView.h"
#include "JzRE/Editor/Panels/JzGameView.h"
#include "JzRE/Editor/Panels/JzAssetBrowser.h"
#include "JzRE/Editor/Panels/JzConsole.h"
#include "JzRE/Editor/Panels/JzHierarchy.h"
#include "JzRE/Editor/Panels/JzMaterialEditor.h"
#include "JzRE/Editor/Panels/JzMenuBar.h"
#include "JzRE/Runtime/JzRERuntime.h"
#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Runtime/Function/ECS/JzWindowSystem.h"
#include "JzRE/Editor/Core/JzEditorState.h"

JzRE::JzEditorUI::JzEditorUI(JzRE::JzRERuntime &runtime) :
    m_runtime(&runtime)
{
    auto &windowSystem = JzServiceContainer::Get<JzWindowSystem>();
    m_uiManager        = std::make_unique<JzUIManager>(windowSystem);
    m_canvas           = std::make_unique<JzCanvas>();
    m_panelsManager    = std::make_unique<JzPanelsManager>(*m_canvas);

    // Create and register editor state for selection management
    m_editorState = std::make_unique<JzEditorState>();
    JzServiceContainer::Provide(*m_editorState);

    m_canvas->SetDockspace(true);

    InitializePanels();
}

void JzRE::JzEditorUI::InitializePanels()
{
    auto &windowSystem = JzServiceContainer::Get<JzWindowSystem>();
    m_panelsManager->CreatePanel<JzMenuBar>("Menu Bar", windowSystem);
    m_panelsManager->CreatePanel<JzAssetBrowser>("Asset Browser", true);
    m_panelsManager->CreatePanel<JzSceneView>("Scene View", true);
    m_panelsManager->CreatePanel<JzAssetView>("Asset View", true);
    m_panelsManager->CreatePanel<JzGameView>("Game View", true);
    m_panelsManager->CreatePanel<JzHierarchy>("Hierarchy", true);
    m_panelsManager->CreatePanel<JzConsole>("Console", true);
    m_panelsManager->CreatePanel<JzMaterialEditor>("Material Editor", true);

    m_panelsManager->GetPanelAs<JzMenuBar>("Menu Bar").InitializeSettingsMenu();

    // Wire Asset Browser selection to Asset View preview
    auto &assetBrowser = m_panelsManager->GetPanelAs<JzAssetBrowser>("Asset Browser");
    auto &assetView    = m_panelsManager->GetPanelAs<JzAssetView>("Asset View");
    assetBrowser.AssetSelectedEvent += [&assetView](std::filesystem::path path) {
        assetView.PreviewAsset(path);
    };

    // Wire Hierarchy selection to EditorState and SceneView
    auto &hierarchy = m_panelsManager->GetPanelAs<JzHierarchy>("Hierarchy");
    auto &sceneView = m_panelsManager->GetPanelAs<JzSceneView>("Scene View");

    hierarchy.EntitySelectedEvent += [this](JzEntity entity) {
        m_editorState->SelectEntity(entity);
    };

    hierarchy.SelectionClearedEvent += [this]() {
        m_editorState->ClearSelection();
    };

    // Wire EditorState selection to SceneView for gizmo display
    m_editorState->EntitySelectedEvent += [&sceneView](JzEntity entity) {
        sceneView.SetSelectedEntity(entity);
    };

    m_editorState->SelectionClearedEvent += [&sceneView]() {
        sceneView.SetSelectedEntity(INVALID_ENTITY);
    };

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

JzRE::JzEditorUI::~JzEditorUI()
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

void JzRE::JzEditorUI::Update(JzRE::F32 deltaTime)
{
    // Editor logic updates only - no rendering
    HandleGlobalShortcuts();
    UpdateCurrentEditorMode(deltaTime);
    UpdateEditorPanels(deltaTime);

    // Update Hierarchy panel (entity list refresh)
    auto &hierarchy = m_panelsManager->GetPanelAs<JzHierarchy>("Hierarchy");
    if (hierarchy.IsOpened()) {
        hierarchy.Update(deltaTime);
    }

    // Update SceneView logic (camera control, input handling)
    auto &sceneView = m_panelsManager->GetPanelAs<JzSceneView>("Scene View");
    if (sceneView.IsOpened()) {
        sceneView.Update(deltaTime);
    }

    // Update AssetView logic (orbit camera for model preview)
    auto &assetView = m_panelsManager->GetPanelAs<JzAssetView>("Asset View");
    if (assetView.IsOpened()) {
        assetView.Update(deltaTime);
    }
}

void JzRE::JzEditorUI::Render(JzRE::F32 deltaTime)
{
    // Rendering phase:
    // Views are now rendered by RenderSystem during World::Update()
    // Here we only render the ImGui UI
    RenderEditorUI(deltaTime);
}

void JzRE::JzEditorUI::HandleGlobalShortcuts()
{
    // TODO
}

void JzRE::JzEditorUI::UpdateCurrentEditorMode(JzRE::F32 deltaTime)
{
    // TODO
}

void JzRE::JzEditorUI::UpdateEditorPanels(JzRE::F32 deltaTime)
{
    auto &menuBar = m_panelsManager->GetPanelAs<JzMenuBar>("Menu Bar");
    // auto &sceneView = m_panelsManager.GetPanelAs<JzSceneView>("Scene View");

    menuBar.HandleShortcuts(deltaTime);
}

void JzRE::JzEditorUI::RenderEditorUI(JzRE::F32 deltaTime)
{
    m_uiManager->Render();
}