/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Core/JzServiceContainer.h"
#include "JzRE/Editor/JzSceneView.h"
#include "JzRE/Editor/JzInputManager.h"

JzRE::JzSceneView::JzSceneView(const JzRE::String &name, JzRE::Bool is_opened) :
    JzRE::JzView(name, is_opened) { }

void JzRE::JzSceneView::Update(JzRE::F32 deltaTime)
{
    JzView::Update(deltaTime);

    // auto &inputManager = JzServiceContainer::Get<JzInputManager>();
    // if (IsFocused()) {
    //     if (inputManager.IsKeyPressed(JzEInputKeyboardButton::KEY_W)) {
    //         SetGizmoOperation(JzEGizmoOperation::TRANSLATE);
    //     }

    //     if (inputManager.IsKeyPressed(JzEInputKeyboardButton::KEY_E)) {
    //         SetGizmoOperation(JzEGizmoOperation::ROTATE);
    //     }

    //     if (inputManager.IsKeyPressed(JzEInputKeyboardButton::KEY_R)) {
    //         SetGizmoOperation(JzEGizmoOperation::SCALE);
    //     }
    // }
}

void JzRE::JzSceneView::SetGizmoOperation(JzRE::JzEGizmoOperation operation)
{
    m_currentOperation = operation;
    // EDITOR_EVENT(EditorOperationChanged).Invoke(m_currentOperation);
}

JzRE::JzEGizmoOperation JzRE::JzSceneView::GetGizmoOperation() const
{
    return m_currentOperation;
}

void JzRE::JzSceneView::HandleActorPicking()
{
    auto &inputManager = JzServiceContainer::Get<JzInputManager>();
    if (inputManager.IsMouseButtonReleased(JzEInputMouseButton::MOUSE_BUTTON_LEFT)) {
        // m_gizmoOperations.StopPicking();
    }
}