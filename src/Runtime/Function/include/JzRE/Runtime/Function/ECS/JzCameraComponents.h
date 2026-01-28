/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzVector.h"
#include "JzRE/Runtime/Core/JzMatrix.h"

namespace JzRE {

// ==================== Camera Components ====================

/**
 * @brief Extended camera component with full camera state.
 *
 * Contains position, rotation, projection parameters, and computed matrices.
 * The viewMatrix and projectionMatrix are updated by JzCameraSystem.
 */
struct JzCameraComponent {
    JzVec3 position{0.0f, 0.0f, 10.0f};
    JzVec4 rotation{0.0f, 0.0f, 0.0f, 0.0f}; ///< pitch, yaw, roll, unused

    F32 fov       = 60.0f;
    F32 nearPlane = 0.1f;
    F32 farPlane  = 100.0f;
    F32 aspect    = 16.0f / 9.0f;

    JzVec3 clearColor{0.1f, 0.1f, 0.1f};
    Bool   isMainCamera = true;

    // Computed matrices (updated by CameraSystem)
    JzMat4 viewMatrix       = JzMat4x4::Identity();
    JzMat4 projectionMatrix = JzMat4x4::Identity();
};

/**
 * @brief Orbit camera controller component.
 *
 * Provides orbit camera behavior: rotate around a target point,
 * pan the target, and zoom in/out.
 */
struct JzOrbitControllerComponent {
    JzVec3 target{0.0f, 0.0f, 0.0f}; ///< Point the camera orbits around
    F32    yaw      = 0.0f;          ///< Horizontal orbit angle in radians
    F32    pitch    = 0.3f;          ///< Vertical orbit angle in radians
    F32    distance = 5.0f;          ///< Distance from target

    F32 orbitSensitivity = 0.005f; ///< Sensitivity for orbit rotation
    F32 panSensitivity   = 0.002f; ///< Sensitivity for panning
    F32 zoomSensitivity  = 0.5f;   ///< Sensitivity for zooming
    F32 minDistance      = 0.5f;   ///< Minimum orbit distance
    F32 maxDistance      = 100.0f; ///< Maximum orbit distance

    // Mouse tracking state
    Bool   leftMousePressed  = false;
    Bool   rightMousePressed = false;
    Bool   firstMouse        = true;
    JzVec2 lastMousePos{0.0f, 0.0f};
};

// ==================== Camera Tags ====================

/**
 * @brief Tag for main camera entity.
 */
struct JzMainCameraTag { };

// ==================== Legacy Camera Input Component ====================

/**
 * @brief Component for camera input control state.
 *
 * This component stores processed input state specifically for camera control.
 * It acts as an intermediate layer between raw input and camera behavior.
 */
struct JzCameraInputComponent {
    Bool   orbitActive{false};     ///< Orbit mode is active (left mouse button held)
    Bool   panActive{false};       ///< Pan mode is active (right mouse button held)
    JzVec2 mouseDelta{0.0f, 0.0f}; ///< Mouse movement delta for this frame
    F32    scrollDelta{0.0f};      ///< Scroll wheel delta for zoom
    Bool   resetRequested{false};  ///< Request to reset camera to default
};

} // namespace JzRE
