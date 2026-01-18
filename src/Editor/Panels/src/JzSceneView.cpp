/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include <cmath>
#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Editor/Panels/JzSceneView.h"
#include "JzRE/Runtime/Function/Input/JzInputManager.h"
#include "JzRE/Runtime/Function/ECS/JzEnttWorld.h"
#include "JzRE/Runtime/Function/ECS/JzEnttRenderComponents.h"

JzRE::JzSceneView::JzSceneView(const JzRE::String &name, JzRE::Bool is_opened) :
    JzRE::JzView(name, is_opened)
{
    // Note: Camera initialization is done on first Update() since JzEnttWorld
    // is not yet registered in JzServiceContainer at construction time.
}

void JzRE::JzSceneView::Update(JzRE::F32 deltaTime)
{
    JzView::Update(deltaTime);

    // Initialize camera on first update when JzEnttWorld is available
    if (!m_cameraInitialized) {
        SyncOrbitFromCamera();
        m_cameraInitialized = true;
    }

    // Only process mouse input when the panel is hovered or focused
    if (!IsHovered() && !IsFocused()) {
        m_firstMouse = true;
        return;
    }

    auto &inputManager = JzServiceContainer::Get<JzInputManager>();

    // Get current mouse position
    JzVec2 currentMousePos = inputManager.GetMousePosition();

    // Calculate mouse delta
    F32 deltaX = 0.0f;
    F32 deltaY = 0.0f;
    if (!m_firstMouse) {
        deltaX = currentMousePos.x - m_lastMousePos.x;
        deltaY = currentMousePos.y - m_lastMousePos.y;
    }

    // Track button states - use GetMouseButtonState for real-time GLFW state
    // instead of IsMouseButtonPressed which is event-based and cleared each frame
    Bool leftPressed  = inputManager.GetMouseButtonState(JzEInputMouseButton::MOUSE_BUTTON_LEFT) == JzEInputMouseButtonState::MOUSE_DOWN;
    Bool rightPressed = inputManager.GetMouseButtonState(JzEInputMouseButton::MOUSE_BUTTON_RIGHT) == JzEInputMouseButtonState::MOUSE_DOWN;

    // Handle left mouse button - Orbit rotation
    if (leftPressed) {
        if (!m_leftMousePressed) {
            // Just started pressing left button
            m_leftMousePressed = true;
            m_firstMouse       = true;
        } else if (!m_firstMouse) {
            // Dragging with left button
            HandleOrbitRotation(deltaX, deltaY);
        }
    } else {
        m_leftMousePressed = false;
    }

    // Handle right mouse button - Panning
    if (rightPressed) {
        if (!m_rightMousePressed) {
            // Just started pressing right button
            m_rightMousePressed = true;
            m_firstMouse        = true;
        } else if (!m_firstMouse) {
            // Dragging with right button
            HandlePanning(deltaX, deltaY);
        }
    } else {
        m_rightMousePressed = false;
    }

    // Handle scroll wheel - Zoom
    JzVec2 scroll = inputManager.GetMouseScroll();
    if (std::abs(scroll.y) > 0.001f) {
        HandleZoom(scroll.y);
    }

    // Update last mouse position
    m_lastMousePos = currentMousePos;
    m_firstMouse   = false;

    // Keyboard shortcuts for gizmo operations
    if (IsFocused()) {
        if (inputManager.IsKeyPressed(JzEInputKeyboardButton::KEY_W)) {
            SetGizmoOperation(JzEGizmoOperation::TRANSLATE);
        }

        if (inputManager.IsKeyPressed(JzEInputKeyboardButton::KEY_E)) {
            SetGizmoOperation(JzEGizmoOperation::ROTATE);
        }

        if (inputManager.IsKeyPressed(JzEInputKeyboardButton::KEY_R)) {
            SetGizmoOperation(JzEGizmoOperation::SCALE);
        }
    }
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

void JzRE::JzSceneView::HandleOrbitRotation(JzRE::F32 deltaX, JzRE::F32 deltaY)
{
    // Update yaw and pitch based on mouse movement
    m_orbitYaw   -= deltaX * m_orbitSensitivity;
    m_orbitPitch -= deltaY * m_orbitSensitivity;

    // Clamp pitch to avoid gimbal lock (between -89 and 89 degrees)
    constexpr F32 maxPitch = 1.55f; // ~89 degrees in radians
    m_orbitPitch           = std::clamp(m_orbitPitch, -maxPitch, maxPitch);

    // Update camera position
    UpdateCameraFromOrbit();
}

void JzRE::JzSceneView::HandlePanning(JzRE::F32 deltaX, JzRE::F32 deltaY)
{
    // Calculate the right and up vectors in world space based on current orientation
    F32 cosYaw   = std::cos(m_orbitYaw);
    F32 sinYaw   = std::sin(m_orbitYaw);
    F32 cosPitch = std::cos(m_orbitPitch);
    F32 sinPitch = std::sin(m_orbitPitch);

    // Right vector (perpendicular to the view direction in the horizontal plane)
    JzVec3 right(cosYaw, 0.0f, sinYaw);

    // Up vector (perpendicular to both right and forward)
    JzVec3 forward(sinYaw * cosPitch, sinPitch, -cosYaw * cosPitch);
    JzVec3 up(-sinYaw * sinPitch, cosPitch, cosYaw * sinPitch);

    // Calculate pan amount based on distance (further = larger pan)
    F32 panScale = m_orbitDistance * m_panSensitivity;

    // Move the target point
    m_orbitTarget.x -= right.x * deltaX * panScale + up.x * deltaY * panScale;
    m_orbitTarget.y += up.y * deltaY * panScale;
    m_orbitTarget.z -= right.z * deltaX * panScale + up.z * deltaY * panScale;

    // Update camera position
    UpdateCameraFromOrbit();
}

void JzRE::JzSceneView::HandleZoom(JzRE::F32 scrollY)
{
    // Adjust orbit distance based on scroll
    m_orbitDistance -= scrollY * m_zoomSensitivity;

    // Clamp distance to valid range
    m_orbitDistance = std::clamp(m_orbitDistance, m_minDistance, m_maxDistance);

    // Update camera position
    UpdateCameraFromOrbit();
}

void JzRE::JzSceneView::SyncOrbitFromCamera()
{
    // Sync orbit parameters from the camera's OrbitControllerComponent
    auto &world = JzServiceContainer::Get<JzEnttWorld>();

    // Find the main camera entity with OrbitControllerComponent
    auto view = world.View<JzEnttCameraComponent, JzEnttOrbitControllerComponent>();
    for (auto entity : view) {
        auto &camera = world.GetComponent<JzEnttCameraComponent>(entity);
        if (camera.isMainCamera) {
            auto &orbit     = world.GetComponent<JzEnttOrbitControllerComponent>(entity);
            m_orbitTarget   = orbit.target;
            m_orbitYaw      = orbit.yaw;
            m_orbitPitch    = orbit.pitch;
            m_orbitDistance = orbit.distance;
            break;
        }
    }
}

void JzRE::JzSceneView::UpdateCameraFromOrbit()
{
    // Calculate camera position using spherical coordinates
    // x = r * cos(pitch) * sin(yaw)
    // y = r * sin(pitch)
    // z = r * cos(pitch) * cos(yaw)
    F32 cosPitch = std::cos(m_orbitPitch);
    F32 sinPitch = std::sin(m_orbitPitch);
    F32 cosYaw   = std::cos(m_orbitYaw);
    F32 sinYaw   = std::sin(m_orbitYaw);

    JzVec3 cameraPos;
    cameraPos.x = m_orbitTarget.x + m_orbitDistance * cosPitch * sinYaw;
    cameraPos.y = m_orbitTarget.y + m_orbitDistance * sinPitch;
    cameraPos.z = m_orbitTarget.z + m_orbitDistance * cosPitch * cosYaw;

    // Get the ECS world and update camera components
    auto &world = JzServiceContainer::Get<JzEnttWorld>();

    // Find the main camera entity and update its components
    auto view = world.View<JzEnttCameraComponent, JzEnttOrbitControllerComponent>();
    for (auto entity : view) {
        auto &camera = world.GetComponent<JzEnttCameraComponent>(entity);
        if (camera.isMainCamera) {
            // Update camera position and rotation
            camera.position   = cameraPos;
            camera.rotation.x = -m_orbitPitch; // Pitch
            camera.rotation.y = m_orbitYaw;    // Yaw
            camera.rotation.z = 0.0f;          // Roll
            camera.rotation.w = 0.0f;          // Unused

            // Update orbit controller component to stay in sync
            auto &orbit    = world.GetComponent<JzEnttOrbitControllerComponent>(entity);
            orbit.target   = m_orbitTarget;
            orbit.yaw      = m_orbitYaw;
            orbit.pitch    = m_orbitPitch;
            orbit.distance = m_orbitDistance;

            break;
        }
    }
}