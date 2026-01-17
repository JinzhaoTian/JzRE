/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Function/ECS/EnTT/Systems/JzEnttCameraSystem.h"

#include <algorithm>
#include <cmath>

#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Runtime/Function/Input/JzInputManager.h"

namespace JzRE {

void JzEnttCameraSystem::OnInit(JzEnttWorld &world)
{
    // Nothing to initialize
}

void JzEnttCameraSystem::Update(JzEnttWorld &world, F32 delta)
{
    // Process all cameras
    auto view = world.View<JzEnttCameraComponent>();

    for (auto entity : view) {
        auto &camera = world.GetComponent<JzEnttCameraComponent>(entity);

        // Update aspect ratio
        camera.aspect = m_aspectRatio;

        // Handle orbit controller if present
        auto *orbit = world.TryGetComponent<JzEnttOrbitControllerComponent>(entity);
        if (orbit) {
            HandleOrbitController(camera, *orbit);
        }

        // Update matrices
        UpdateCameraMatrices(camera);

        // Cache main camera data
        if (camera.isMainCamera) {
            m_viewMatrix       = camera.viewMatrix;
            m_projectionMatrix = camera.projectionMatrix;
            m_cameraPosition   = camera.position;
            m_clearColor       = camera.clearColor;
        }
    }
}

void JzEnttCameraSystem::UpdateCameraMatrices(JzEnttCameraComponent &camera)
{
    // Get rotation (stored as pitch, yaw, roll, unused in x, y, z, w)
    F32 pitch = camera.rotation.x();
    F32 yaw   = camera.rotation.y();

    // Calculate forward direction from pitch and yaw
    F32 cosPitch = std::cos(pitch);
    F32 sinPitch = std::sin(pitch);
    F32 cosYaw   = std::cos(yaw);
    F32 sinYaw   = std::sin(yaw);

    JzVec3 forward(sinYaw * cosPitch, sinPitch, -cosYaw * cosPitch);
    JzVec3 cameraTarget = camera.position + forward;

    // Calculate right vector and correct up vector
    JzVec3 worldUp(0.0f, 1.0f, 0.0f);
    JzVec3 right    = forward.Cross(worldUp).Normalized();
    JzVec3 cameraUp = right.Cross(forward).Normalized();

    // Compute view matrix
    camera.viewMatrix = JzMat4x4::LookAt(camera.position, cameraTarget, cameraUp);

    // Compute projection matrix
    F32 fovRadians        = camera.fov * 3.14159265358979323846f / 180.0f;
    camera.projectionMatrix =
        JzMat4x4::Perspective(fovRadians, camera.aspect, camera.nearPlane, camera.farPlane);
}

void JzEnttCameraSystem::HandleOrbitController(JzEnttCameraComponent          &camera,
                                                JzEnttOrbitControllerComponent &orbit)
{
    // Get input manager from service container
    JzInputManager *inputManagerPtr = nullptr;
    try {
        inputManagerPtr = &JzServiceContainer::Get<JzInputManager>();
    } catch (...) {
        return;
    }

    auto &inputManager = *inputManagerPtr;

    // Get current mouse position
    JzVec2 currentMousePos = inputManager.GetMousePosition();

    // Calculate mouse delta
    F32 deltaX = 0.0f;
    F32 deltaY = 0.0f;
    if (!orbit.firstMouse) {
        deltaX = currentMousePos.x() - orbit.lastMousePos.x();
        deltaY = currentMousePos.y() - orbit.lastMousePos.y();
    }

    // Track button states
    Bool leftPressed =
        inputManager.GetMouseButtonState(JzEInputMouseButton::MOUSE_BUTTON_LEFT) ==
        JzEInputMouseButtonState::MOUSE_DOWN;
    Bool rightPressed =
        inputManager.GetMouseButtonState(JzEInputMouseButton::MOUSE_BUTTON_RIGHT) ==
        JzEInputMouseButtonState::MOUSE_DOWN;

    // Handle left mouse button - Orbit rotation
    if (leftPressed) {
        if (!orbit.leftMousePressed) {
            orbit.leftMousePressed = true;
            orbit.firstMouse       = true;
        } else if (!orbit.firstMouse) {
            HandleOrbitRotation(orbit, deltaX, deltaY);
        }
    } else {
        orbit.leftMousePressed = false;
    }

    // Handle right mouse button - Panning
    if (rightPressed) {
        if (!orbit.rightMousePressed) {
            orbit.rightMousePressed = true;
            orbit.firstMouse        = true;
        } else if (!orbit.firstMouse) {
            HandlePanning(orbit, deltaX, deltaY);
        }
    } else {
        orbit.rightMousePressed = false;
    }

    // Handle scroll wheel - Zoom
    JzVec2 scroll = inputManager.GetMouseScroll();
    if (std::abs(scroll.y()) > 0.001f) {
        HandleZoom(orbit, scroll.y());
    }

    // Update last mouse position
    orbit.lastMousePos = currentMousePos;
    orbit.firstMouse   = false;

    // Update camera from orbit parameters
    UpdateCameraFromOrbit(camera, orbit);
}

void JzEnttCameraSystem::HandleOrbitRotation(JzEnttOrbitControllerComponent &orbit, F32 deltaX,
                                              F32 deltaY)
{
    // Update yaw and pitch based on mouse movement (drag-object style)
    orbit.yaw -= deltaX * orbit.orbitSensitivity;
    orbit.pitch -= deltaY * orbit.orbitSensitivity;

    // Clamp pitch to avoid gimbal lock (between -89 and 89 degrees)
    constexpr F32 maxPitch = 1.55f; // ~89 degrees in radians
    orbit.pitch            = std::clamp(orbit.pitch, -maxPitch, maxPitch);
}

void JzEnttCameraSystem::HandlePanning(JzEnttOrbitControllerComponent &orbit, F32 deltaX,
                                        F32 deltaY)
{
    // Calculate the right and up vectors in world space based on current orientation
    F32 cosYaw   = std::cos(orbit.yaw);
    F32 sinYaw   = std::sin(orbit.yaw);
    F32 cosPitch = std::cos(orbit.pitch);
    F32 sinPitch = std::sin(orbit.pitch);

    // Right vector (perpendicular to the view direction in the horizontal plane)
    JzVec3 right(cosYaw, 0.0f, sinYaw);

    // Up vector (perpendicular to both right and forward)
    JzVec3 up(-sinYaw * sinPitch, cosPitch, cosYaw * sinPitch);

    // Calculate pan amount based on distance (further = larger pan)
    F32 panScale = orbit.distance * orbit.panSensitivity;

    // Move the target point
    orbit.target.x() -= right.x() * deltaX * panScale + up.x() * deltaY * panScale;
    orbit.target.y() += up.y() * deltaY * panScale;
    orbit.target.z() -= right.z() * deltaX * panScale + up.z() * deltaY * panScale;
}

void JzEnttCameraSystem::HandleZoom(JzEnttOrbitControllerComponent &orbit, F32 scrollY)
{
    // Adjust orbit distance based on scroll
    orbit.distance -= scrollY * orbit.zoomSensitivity;

    // Clamp distance to valid range
    orbit.distance = std::clamp(orbit.distance, orbit.minDistance, orbit.maxDistance);
}

void JzEnttCameraSystem::UpdateCameraFromOrbit(JzEnttCameraComponent          &camera,
                                                JzEnttOrbitControllerComponent &orbit)
{
    // Calculate camera position using spherical coordinates
    F32 cosPitch = std::cos(orbit.pitch);
    F32 sinPitch = std::sin(orbit.pitch);
    F32 cosYaw   = std::cos(orbit.yaw);
    F32 sinYaw   = std::sin(orbit.yaw);

    camera.position.x() = orbit.target.x() + orbit.distance * cosPitch * sinYaw;
    camera.position.y() = orbit.target.y() + orbit.distance * sinPitch;
    camera.position.z() = orbit.target.z() + orbit.distance * cosPitch * cosYaw;

    // Set camera rotation to look at target
    // Camera position uses: x = sin(yaw), z = cos(yaw)
    // So at yaw=0, camera is at +Z looking toward target at origin
    // Add PI to yaw so the forward vector points toward target
    camera.rotation.x() = -orbit.pitch;
    camera.rotation.y() = orbit.yaw + 3.14159265358979323846f;
    camera.rotation.z() = 0.0f;
    camera.rotation.w() = 0.0f;
}

} // namespace JzRE
