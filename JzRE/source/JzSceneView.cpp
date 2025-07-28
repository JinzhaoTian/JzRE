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

    if (IsFocused() && !m_cameraController.IsRightMousePressed()) {
        if (EDITOR_CONTEXT(inputManager)->IsKeyPressed(JzEInputKey::KEY_W)) {
            SetGizmoOperation(JzEGizmoOperation::TRANSLATE);
        }

        if (EDITOR_CONTEXT(inputManager)->IsKeyPressed(JzEInputKey::KEY_E)) {
            SetGizmoOperation(JzEGizmoOperation::ROTATE);
        }

        if (EDITOR_CONTEXT(inputManager)->IsKeyPressed(JzEInputKey::KEY_R)) {
            SetGizmoOperation(JzEGizmoOperation::SCALE);
        }
    }
}

JzRE::JzScene *JzRE::JzSceneView::GetScene()
{
    return m_sceneManager.GetCurrentScene();
}

void JzRE::JzSceneView::SetGizmoOperation(JzRE::JzEGizmoOperation operation)
{
    m_currentOperation = operation;
    EDITOR_EVENT(EditorOperationChanged).Invoke(m_currentOperation);
}

JzRE::JzEGizmoOperation JzRE::JzSceneView::GetGizmoOperation() const
{
    return m_currentOperation;
}

void JzRE::JzSceneView::DrawFrame()
{
    JzViewControllable::DrawFrame();
    HandleActorPicking();
}

void JzRE::JzSceneView::HandleActorPicking()
{
    auto &inputManager = *EDITOR_CONTEXT(inputManager);

    if (inputManager.IsMouseButtonReleased(JzEInputMouseButton::MOUSE_BUTTON_LEFT)) {
        // m_gizmoOperations.StopPicking();
    }
}
