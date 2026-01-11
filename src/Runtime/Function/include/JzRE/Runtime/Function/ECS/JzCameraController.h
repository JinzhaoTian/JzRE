/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Function/ECS/JzCamera.h"

namespace JzRE {
/**
 * @brief Camera Controller
 */
class JzCameraController {
public:
    /**
     * @brief Constructor
     *
     * @param view The view
     * @param camera The camera
     */
    JzCameraController(JzCamera &camera);

    /**
     * @brief Handle the inputs
     *
     * @param deltaTime The delta time
     */
    void HandleInputs(F32 deltaTime);

    /**
     * @brief Check if the right mouse is pressed
     */
    Bool IsRightMousePressed() const;

private:
    JzCamera &m_camera;

    Bool m_leftMousePressed     = false;
    Bool m_middleMousePressed   = false;
    Bool m_rightMousePressed    = false;
    Bool m_firstMouse           = true;
    F64  m_lastMousePosX        = 0.0;
    F64  m_lastMousePosY        = 0.0;
    F32  m_mouseSensitivity     = 0.12f;
    F32  m_cameraDragSpeed      = 0.03f;
    F32  m_cameraOrbitSpeed     = 0.5f;
    F32  m_cameraMoveSpeed      = 15.0f;
    F32  m_focusDistance        = 15.0f;
    F32  m_focusLerpCoefficient = 8.0f;
};
} // namespace JzRE