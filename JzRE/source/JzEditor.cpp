#include "JzEditor.h"

JzRE::JzEditor::JzEditor(JzContext &context) :
    m_context(context),
    m_panelsManager(m_canvas)
{
    SetUI();

    m_context.sceneManager.LoadDefaultScene();
}

JzRE::JzEditor::~JzEditor()
{
    m_context.sceneManager.UnloadScene();
}

void JzRE::JzEditor::PreUpdate()
{
    m_context.device->PollEvents();
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

void JzRE::JzEditor::SetUI()
{
    m_panelsManager.CreatePanel<JzMenuBar>("Menu Bar");
    m_panelsManager.CreatePanel<JzAssetBrowser>("Asset Browser");
    m_panelsManager.CreatePanel<JzSceneView>("Scene View");
    m_panelsManager.CreatePanel<JzAssetView>("Asset View");

    m_canvas.SetDockspace(true);
    m_context.uiManager->SetCanvas(m_canvas);
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
    auto &assetView = m_panelsManager.GetPanelAs<JzAssetView>("Asset View");
    auto &sceneView = m_panelsManager.GetPanelAs<JzSceneView>("Scene View");

    assetView.Update(deltaTime);
    sceneView.Update(deltaTime);

    assetView.Render();
    sceneView.Render();
}

void JzRE::JzEditor::UpdateEditorPanels(JzRE::F32 deltaTime)
{
    auto &menuBar   = m_panelsManager.GetPanelAs<JzMenuBar>("Menu Bar");
    auto &sceneView = m_panelsManager.GetPanelAs<JzSceneView>("Scene View");

    menuBar.HandleShortcuts(deltaTime);
}

void JzRE::JzEditor::RenderEditorUI(JzRE::F32 deltaTime)
{
    m_context.uiManager->Render();
}