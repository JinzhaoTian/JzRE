#pragma once

#include "CommonTypes.h"
#include "JzCamera.h"
#include "JzEditorActions.h"
#include "JzInputManager.h"
#include "JzView.h"
#include "JzWindow.h"

namespace JzRE {
class JzCameraController {
public:
    JzCameraController(JzView &view, JzCamera &camera);

    void HandleInputs(F32 deltaTime);

    Bool IsRightMousePressed() const;

private:
    JzInputManager &m_inputManager;
    JzWindow       &m_window;
    JzView         &m_view;
    JzCamera       &m_camera;

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