/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/ECS/JzCameraController.h"
#include "JzRE/Core/JzEvent.h"

JzRE::JzCameraController::JzCameraController(JzRE::JzCamera &camera) :
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