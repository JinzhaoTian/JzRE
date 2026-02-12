/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include <cmath>
#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Editor/Panels/JzSceneView.h"
#include "JzRE/Editor/Core/JzEditorState.h"
#include "JzRE/Runtime/Function/ECS/JzWorld.h"
#include "JzRE/Runtime/Function/ECS/JzCameraComponents.h"
#include "JzRE/Runtime/Function/ECS/JzInputComponents.h"
#include "JzRE/Runtime/Function/ECS/JzWindowComponents.h"
#include "JzRE/Runtime/Function/ECS/JzRenderSystem.h"

JzRE::JzSceneView::JzSceneView(const JzRE::String &name, JzRE::Bool is_opened) :
    JzRE::JzView(name, is_opened)
{
    // Note: Camera initialization is done on first Update() since JzWorld
    // is not yet registered in JzServiceContainer at construction time.
}

void JzRE::JzSceneView::Update(JzRE::F32 deltaTime)
{
    JzView::Update(deltaTime);

    // Ensure the render target is registered when RenderSystem becomes available.
    if (IsOpened() && m_viewHandle == JzRenderSystem::INVALID_VIEW_HANDLE && JzServiceContainer::Has<JzRenderSystem>()) {
        RegisterRenderTarget();
    }

    // Keep the view camera binding up to date (main camera can be recreated).
    if (m_viewHandle != JzRenderSystem::INVALID_VIEW_HANDLE && JzServiceContainer::Has<JzRenderSystem>()) {
        auto &renderSystem = JzServiceContainer::Get<JzRenderSystem>();
        renderSystem.UpdateViewCamera(m_viewHandle, GetCameraEntity());
    }

    if (!JzServiceContainer::Has<JzWorld>()) {
        return;
    }

    auto &world = JzServiceContainer::Get<JzWorld>();

    // Update aspect ratio from view size
    auto viewSize = GetSafeSize();
    if (viewSize.x > 0 && viewSize.y > 0) {
        auto camera = GetCameraEntity();
        if (IsValidEntity(camera) && world.HasComponent<JzCameraComponent>(camera)) {
            if (viewSize.x != m_lastViewSize.x || viewSize.y != m_lastViewSize.y) {
                m_lastViewSize = viewSize;
            }
            auto &cam  = world.GetComponent<JzCameraComponent>(camera);
            cam.aspect = static_cast<F32>(viewSize.x) / static_cast<F32>(viewSize.y);
        }
    }

    // Initialize camera on first update when JzWorld is available
    if (!m_cameraInitialized) {
        FindEditorCamera();
        EnsureCameraInputComponent();
        CreateSceneInputState();
        m_cameraInitialized = true;
    }

    // Re-find camera if it became invalid (e.g., scene reload)
    if (m_editorCamera != INVALID_ENTITY && !world.IsValid(m_editorCamera)) {
        FindEditorCamera();
    }

    // Get camera entity and its input component
    auto cameraEntity = GetCameraEntity();
    if (!IsValidEntity(cameraEntity)) {
        return;
    }

    auto *cameraInput = world.TryGetComponent<JzCameraInputComponent>(cameraEntity);
    if (!cameraInput) {
        return;
    }

    // Reset input state each frame
    cameraInput->orbitActive    = false;
    cameraInput->panActive      = false;
    cameraInput->mouseDelta     = JzVec2(0.0f, 0.0f);
    cameraInput->scrollDelta    = 0.0f;
    cameraInput->resetRequested = false;

    // Only process mouse input when the panel is hovered or focused
    if (!IsHovered() && !IsFocused()) {
        m_firstMouse = true;
        return;
    }

    // Ensure dedicated input state exists and is up to date
    CreateSceneInputState();
    UpdateSceneInputState();

    // Use dedicated scene input state (isolated from asset view)
    JzInputStateComponent *inputState = nullptr;
    if (m_sceneInputState != INVALID_ENTITY && world.IsValid(m_sceneInputState)) {
        inputState = world.TryGetComponent<JzInputStateComponent>(m_sceneInputState);
    }
    if (!inputState) return;

    const auto &input = *inputState;

    // Get current mouse position
    JzVec2 currentMousePos = input.mouse.position;

    // Calculate mouse delta
    F32 deltaX = 0.0f;
    F32 deltaY = 0.0f;
    if (!m_firstMouse) {
        deltaX = currentMousePos.x - m_lastMousePos.x;
        deltaY = currentMousePos.y - m_lastMousePos.y;
    }

    // Track button states and write to camera input component
    Bool leftPressed  = input.mouse.IsButtonPressed(JzEMouseButton::Left);
    Bool rightPressed = input.mouse.IsButtonPressed(JzEMouseButton::Right);

    // Left mouse button — Orbit rotation
    if (leftPressed) {
        if (!m_leftMousePressed) {
            m_leftMousePressed = true;
            m_firstMouse       = true;
        } else if (!m_firstMouse) {
            cameraInput->orbitActive = true;
            cameraInput->mouseDelta  = JzVec2(deltaX, deltaY);
        }
    } else {
        m_leftMousePressed = false;
    }

    // Right mouse button — Panning
    if (rightPressed) {
        if (!m_rightMousePressed) {
            m_rightMousePressed = true;
            m_firstMouse        = true;
        } else if (!m_firstMouse) {
            cameraInput->panActive  = true;
            cameraInput->mouseDelta = JzVec2(deltaX, deltaY);
        }
    } else {
        m_rightMousePressed = false;
    }

    // Scroll wheel — Zoom
    JzVec2 scroll = input.mouse.scrollDelta;
    if (std::abs(scroll.y) > 0.001f) {
        cameraInput->scrollDelta = scroll.y;
    }

    // Update last mouse position
    m_lastMousePos = currentMousePos;
    m_firstMouse   = false;

    // Keyboard shortcuts for gizmo operations
    if (IsFocused()) {
        if (input.keyboard.IsKeyDown(JzEKeyCode::W)) {
            SetGizmoOperation(JzEGizmoOperation::TRANSLATE);
        }

        if (input.keyboard.IsKeyDown(JzEKeyCode::E)) {
            SetGizmoOperation(JzEGizmoOperation::ROTATE);
        }

        if (input.keyboard.IsKeyDown(JzEKeyCode::R)) {
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

void JzRE::JzSceneView::SetSelectedEntity(JzRE::JzEntity entity)
{
    m_selectedEntity = entity;
}

JzRE::JzEntity JzRE::JzSceneView::GetSelectedEntity() const
{
    return m_selectedEntity;
}

void JzRE::JzSceneView::HandleActorPicking()
{
    if (!JzServiceContainer::Has<JzWorld>()) {
        return;
    }

    auto &world = JzServiceContainer::Get<JzWorld>();

    // Use dedicated scene input state
    JzInputStateComponent *inputState = nullptr;
    if (m_sceneInputState != INVALID_ENTITY && world.IsValid(m_sceneInputState)) {
        inputState = world.TryGetComponent<JzInputStateComponent>(m_sceneInputState);
    }
    if (!inputState) return;

    if (inputState->mouse.IsButtonUp(JzEMouseButton::Left)) {
        // m_gizmoOperations.StopPicking();
    }
}

void JzRE::JzSceneView::EnsureCameraInputComponent()
{
    if (!JzServiceContainer::Has<JzWorld>()) {
        return;
    }

    auto &world  = JzServiceContainer::Get<JzWorld>();
    auto  camera = GetCameraEntity();

    if (!IsValidEntity(camera)) {
        return;
    }

    // Add JzCameraInputComponent if not present
    if (!world.HasComponent<JzCameraInputComponent>(camera)) {
        world.AddComponent<JzCameraInputComponent>(camera);
    }

    // Tag editor camera so global input sync doesn't override panel input.
    if (!world.HasComponent<JzEditorCameraInputOverrideTag>(camera)) {
        world.AddComponent<JzEditorCameraInputOverrideTag>(camera);
    }
}

JzRE::JzEntity JzRE::JzSceneView::GetCameraEntity()
{
    return m_editorCamera;
}

JzRE::JzRenderViewFeatures JzRE::JzSceneView::GetRenderFeatures() const
{
    JzRenderViewFeatures features =
        JzRenderViewFeatures::Skybox | JzRenderViewFeatures::Axis | JzRenderViewFeatures::Grid;

    if (!JzServiceContainer::Has<JzEditorState>()) {
        return features;
    }

    const auto &editorState = JzServiceContainer::Get<JzEditorState>();

    if (!editorState.sceneSkyboxEnabled) {
        features = static_cast<JzRenderViewFeatures>(static_cast<U32>(features) &
                                                     ~static_cast<U32>(JzRenderViewFeatures::Skybox));
    }

    if (!editorState.sceneAxisEnabled) {
        features = static_cast<JzRenderViewFeatures>(static_cast<U32>(features) &
                                                     ~static_cast<U32>(JzRenderViewFeatures::Axis));
    }

    if (!editorState.sceneGridEnabled) {
        features = static_cast<JzRenderViewFeatures>(static_cast<U32>(features) &
                                                     ~static_cast<U32>(JzRenderViewFeatures::Grid));
    }

    return features;
}

void JzRE::JzSceneView::FindEditorCamera()
{
    if (!JzServiceContainer::Has<JzWorld>()) {
        return;
    }

    auto &world = JzServiceContainer::Get<JzWorld>();

    // Find the main camera with orbit controller (editor camera)
    auto view = world.View<JzCameraComponent, JzOrbitControllerComponent>();
    for (auto entity : view) {
        auto &camera = world.GetComponent<JzCameraComponent>(entity);
        if (camera.isMainCamera) {
            m_editorCamera = entity;
            return;
        }
    }

    m_editorCamera = INVALID_ENTITY;
}

void JzRE::JzSceneView::CreateSceneInputState()
{
    if (m_sceneInputState != INVALID_ENTITY) {
        return;
    }

    if (!JzServiceContainer::Has<JzWorld>()) {
        return;
    }

    auto &world       = JzServiceContainer::Get<JzWorld>();
    m_sceneInputState = world.CreateEntity();

    // Add dedicated input state component for scene view
    world.AddComponent<JzInputStateComponent>(m_sceneInputState);

    // Initialize mouse position to avoid large delta on first frame
    auto *inputState = world.TryGetComponent<JzInputStateComponent>(m_sceneInputState);
    if (inputState) {
        // Get current mouse position from primary window
        JzInputStateComponent *primaryInput = nullptr;
        auto                   inputView    = world.View<JzInputStateComponent, JzPrimaryWindowTag>();
        for (auto entity : inputView) {
            primaryInput = &world.GetComponent<JzInputStateComponent>(entity);
            break;
        }
        if (primaryInput) {
            inputState->mouse.position     = primaryInput->mouse.position;
            inputState->mouse.lastPosition = primaryInput->mouse.position;
            m_lastMousePos                 = primaryInput->mouse.position;
        }
    }
}

void JzRE::JzSceneView::UpdateSceneInputState()
{
    if (m_sceneInputState == INVALID_ENTITY || !JzServiceContainer::Has<JzWorld>()) {
        return;
    }

    auto &world = JzServiceContainer::Get<JzWorld>();

    if (!world.IsValid(m_sceneInputState)) {
        m_sceneInputState = INVALID_ENTITY;
        return;
    }

    auto *sceneInput = world.TryGetComponent<JzInputStateComponent>(m_sceneInputState);
    if (!sceneInput) {
        return;
    }

    // Copy primary window input state to scene input state
    JzInputStateComponent *primaryInput = nullptr;
    auto                   inputView    = world.View<JzInputStateComponent, JzPrimaryWindowTag>();
    for (auto entity : inputView) {
        primaryInput = &world.GetComponent<JzInputStateComponent>(entity);
        break;
    }
    if (!primaryInput) {
        return;
    }

    // Only update when panel is hovered/focused
    if (IsHovered() || IsFocused()) {
        sceneInput->mouse.position       = primaryInput->mouse.position;
        sceneInput->mouse.scrollDelta    = primaryInput->mouse.scrollDelta;
        sceneInput->mouse.buttonsPressed = primaryInput->mouse.buttonsPressed;
        sceneInput->mouse.buttonsDown    = primaryInput->mouse.buttonsDown;
        sceneInput->mouse.buttonsUp      = primaryInput->mouse.buttonsUp;
        sceneInput->keyboard             = primaryInput->keyboard;
    } else {
        // Clear input state when not focused
        sceneInput->mouse.buttonsPressed.reset();
        sceneInput->mouse.buttonsDown.reset();
        sceneInput->mouse.buttonsUp.reset();
        sceneInput->mouse.scrollDelta = JzVec2(0.0f, 0.0f);
    }
}
