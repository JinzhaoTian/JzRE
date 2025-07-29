#include "JzSceneView.h"

JzRE::JzSceneView::JzSceneView(const JzRE::String &name, JzRE::Bool is_opened) :
    JzRE::JzViewControllable(name, is_opened),
    m_sceneManager(EDITOR_CONTEXT(sceneManager))
{
    m_camera.SetFar(5000.0f);
}

void JzRE::JzSceneView::Update(JzRE::F32 deltaTime)
{
    JzViewControllable::Update(deltaTime);
}

JzRE::JzScene *JzRE::JzSceneView::GetScene()
{
    return m_sceneManager.GetCurrentScene();
}

void JzRE::JzSceneView::DrawFrame()
{
    JzViewControllable::DrawFrame();
}
