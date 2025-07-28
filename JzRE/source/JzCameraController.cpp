#include "JzCameraController.h"

JzRE::JzCameraController::JzCameraController(JzRE::JzView &view, JzRE::JzCamera &camera) :
    m_inputManager(*EDITOR_CONTEXT(inputManager)),
    m_window(*EDITOR_CONTEXT(window)),
    m_view(view),
    m_camera(camera)
{
    m_camera.SetFov(60.0f);
}

void JzRE::JzCameraController::HandleInputs(JzRE::F32 deltaTime)
{
    // TODO
}

JzRE::Bool JzRE::JzCameraController::IsRightMousePressed() const
{
    return m_rightMousePressed;
}