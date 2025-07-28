#include "JzEditorActions.h"

JzRE::JzEditorActions::JzEditorActions(JzRE::JzContext &context, JzRE::JzPanelsManager &panelsManager) :
    m_context(context), m_panelsManager(panelsManager)
{
    JzServiceContainer::Provide<JzEditorActions>(*this);

    m_context.window->SetTitle(m_context.windowSettings.title);
}

JzRE::JzEditorActions::~JzEditorActions() { }

JzRE::JzContext &JzRE::JzEditorActions::GetContext()
{
    return m_context;
}

JzRE::JzPanelsManager &JzRE::JzEditorActions::GetPanelsManager()
{
    return m_panelsManager;
}

void JzRE::JzEditorActions::LoadEmptyScene()
{
    m_context.sceneManager.LoadDefaultScene();
}
