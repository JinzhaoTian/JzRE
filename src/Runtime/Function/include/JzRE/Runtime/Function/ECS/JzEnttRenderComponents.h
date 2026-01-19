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

// ==================== Input Components ====================

/**
 * @brief Component for storing mouse input state.
 *
 * This component is updated by JzEnttInputSystem and read by other systems
 * (like CameraSystem) to process mouse input.
 */
struct JzEnttMouseInputComponent {
    JzVec2 position{0.0f, 0.0f};      ///< Current mouse position
    JzVec2 positionDelta{0.0f, 0.0f}; ///< Mouse movement since last frame
    JzVec2 scroll{0.0f, 0.0f};        ///< Scroll wheel delta this frame

    Bool leftButtonDown{false};   ///< Left mouse button state
    Bool rightButtonDown{false};  ///< Right mouse button state
    Bool middleButtonDown{false}; ///< Middle mouse button state

    Bool leftButtonPressed{false};   ///< Left button pressed this frame
    Bool rightButtonPressed{false};  ///< Right button pressed this frame
    Bool middleButtonPressed{false}; ///< Middle button pressed this frame

    Bool leftButtonReleased{false};   ///< Left button released this frame
    Bool rightButtonReleased{false};  ///< Right button released this frame
    Bool middleButtonReleased{false}; ///< Middle button released this frame
};

/**
 * @brief Component for storing keyboard input state.
 *
 * This component is updated by JzEnttInputSystem and provides
 * high-level key state queries.
 */
struct JzEnttKeyboardInputComponent {
    // Common key states for quick access
    Bool w{false}, a{false}, s{false}, d{false};
    Bool space{false}, shift{false}, ctrl{false}, alt{false};
    Bool escape{false}, enter{false}, tab{false};

    // Arrow keys
    Bool up{false}, down{false}, left{false}, right{false};

    // Function keys
    Bool f1{false}, f2{false}, f3{false}, f4{false};
};

/**
 * @brief Component for camera input control state.
 *
 * This component stores processed input state specifically for camera control.
 * It acts as an intermediate layer between raw input and camera behavior.
 */
struct JzEnttCameraInputComponent {
    Bool   orbitActive{false};     ///< Orbit mode is active (left mouse button held)
    Bool   panActive{false};       ///< Pan mode is active (right mouse button held)
    JzVec2 mouseDelta{0.0f, 0.0f}; ///< Mouse movement delta for this frame
    F32    scrollDelta{0.0f};      ///< Scroll wheel delta for zoom
    Bool   resetRequested{false};  ///< Request to reset camera to default
};

} // namespace JzRE
