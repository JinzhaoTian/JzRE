#include "JzGameView.h"

JzRE::JzGameView::JzGameView(const String &name, Bool is_opened) :
    JzRE::JzView(name, is_opened), m_sceneManager(EDITOR_CONTEXT(sceneManager)) { }

JzRE::JzCamera *JzRE::JzGameView::GetCamera()
{
    if (auto scene = m_sceneManager.GetCurrentScene()) {
        if (auto camera = scene->FindMainCamera()) {
            return camera; // TODO: ECS
        }
    }
    return nullptr;
}

JzRE::JzScene *JzRE::JzGameView::GetScene()
{
    return m_sceneManager.GetCurrentScene();
}