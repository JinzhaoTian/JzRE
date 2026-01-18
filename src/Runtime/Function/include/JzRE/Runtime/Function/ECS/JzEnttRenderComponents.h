/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Core/JzMatrix.h"
#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzVector.h"

namespace JzRE {

// ==================== Camera Components ====================

/**
 * @brief Extended camera component with full camera state.
 *
 * Contains position, rotation, projection parameters, and computed matrices.
 * The viewMatrix and projectionMatrix are updated by JzEnttCameraSystem.
 */
struct JzEnttCameraComponent {
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
struct JzEnttOrbitControllerComponent {
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

// ==================== Light Components ====================

/**
 * @brief Directional light component.
 *
 * Represents a light source with parallel rays (like the sun).
 */
struct JzEnttDirectionalLightComponent {
    JzVec3 direction{0.3f, -1.0f, -0.5f};
    JzVec3 color{1.0f, 1.0f, 1.0f};
    F32    intensity  = 1.0f;
    Bool   castShadow = false;
};

/**
 * @brief Point light component.
 *
 * Represents a light source that emits light in all directions from a point.
 */
struct JzEnttPointLightComponent {
    JzVec3 color{1.0f, 1.0f, 1.0f};
    F32    intensity = 1.0f;
    F32    range     = 10.0f;
    F32    constant  = 1.0f;
    F32    linear    = 0.09f;
    F32    quadratic = 0.032f;
};

/**
 * @brief Spot light component.
 *
 * Represents a light source that emits light in a cone shape.
 */
struct JzEnttSpotLightComponent {
    JzVec3 direction{0.0f, -1.0f, 0.0f};
    JzVec3 color{1.0f, 1.0f, 1.0f};
    F32    intensity   = 1.0f;
    F32    range       = 10.0f;
    F32    innerCutoff = 12.5f; ///< Inner cone angle in degrees
    F32    outerCutoff = 17.5f; ///< Outer cone angle in degrees
};

// ==================== Rendering Tags ====================

/**
 * @brief Tag to mark entities as renderable.
 */
struct JzRenderableTag { };

/**
 * @brief Tag for main camera entity.
 */
struct JzMainCameraTag { };

} // namespace JzRE
