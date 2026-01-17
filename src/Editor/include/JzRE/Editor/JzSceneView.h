/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzVector.h"
#include "JzView.h"

namespace JzRE {

/**
 * @brief Enums of Gizmo Operations
 */
enum class JzEGizmoOperation : U8 {
    TRANSLATE,
    ROTATE,
    SCALE
};

/**
 * @brief Scene View Panel Window
 */
class JzSceneView : public JzView {
public:
    /**
     * @brief Constructor
     *
     * @param name
     * @param is_opened
     */
    JzSceneView(const String &name, Bool is_opened);

    /**
     * @brief Update the scene view
     *
     * @param deltaTime
     */
    void Update(F32 deltaTime) override;

    /**
     * @brief Set the gizmo operation
     */
    void SetGizmoOperation(JzEGizmoOperation operation);

    /**
     * @brief Get the gizmo operation
     *
     * @return The gizmo operation
     */
    JzEGizmoOperation GetGizmoOperation() const;

private:
    void HandleActorPicking();

    /**
     * @brief Handle mouse rotation (left mouse button drag)
     *
     * @param deltaX Mouse delta X
     * @param deltaY Mouse delta Y
     */
    void HandleOrbitRotation(F32 deltaX, F32 deltaY);

    /**
     * @brief Handle mouse panning (right mouse button drag)
     *
     * @param deltaX Mouse delta X
     * @param deltaY Mouse delta Y
     */
    void HandlePanning(F32 deltaX, F32 deltaY);

    /**
     * @brief Handle mouse zoom (scroll wheel)
     *
     * @param scrollY Scroll delta Y
     */
    void HandleZoom(F32 scrollY);

    /**
     * @brief Update the camera position based on orbit parameters
     */
    void UpdateCameraFromOrbit();

    /**
     * @brief Sync orbit parameters from the ECS camera component
     */
    void SyncOrbitFromCamera();

private:
    JzEGizmoOperation m_currentOperation = JzEGizmoOperation::TRANSLATE;

    // Mouse interaction state
    Bool   m_leftMousePressed  = false;
    Bool   m_rightMousePressed = false;
    JzVec2 m_lastMousePos      = JzVec2(0.0f, 0.0f);
    Bool   m_firstMouse        = true;
    Bool   m_cameraInitialized = false;

    // Orbit camera parameters
    F32    m_orbitYaw      = 0.0f;                     ///< Horizontal rotation angle (radians)
    F32    m_orbitPitch    = 0.3f;                     ///< Vertical rotation angle (radians)
    F32    m_orbitDistance = 10.0f;                    ///< Distance from target
    JzVec3 m_orbitTarget   = JzVec3(0.0f, 0.0f, 0.0f); ///< Orbit target point

    // Camera control sensitivity
    F32 m_orbitSensitivity = 0.005f; ///< Orbit rotation sensitivity
    F32 m_panSensitivity   = 0.02f;  ///< Panning sensitivity
    F32 m_zoomSensitivity  = 1.0f;   ///< Zoom sensitivity
    F32 m_minDistance      = 1.0f;   ///< Minimum orbit distance
    F32 m_maxDistance      = 100.0f; ///< Maximum orbit distance
};

} // namespace JzRE