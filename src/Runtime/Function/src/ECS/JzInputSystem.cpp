/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Function/ECS/JzInputSystem.h"

#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Runtime/Function/ECS/JzComponents.h"
#include "JzRE/Runtime/Function/ECS/JzWindowComponents.h"
#include "JzRE/Runtime/Function/Input/JzInputManager.h"

namespace JzRE {

void JzInputSystem::OnInit(JzWorld &world)
{
    // Find primary window entity for caching
    auto windowView = world.View<JzWindowStateComponent, JzInputStateComponent>();
    for (auto entity : windowView) {
        m_primaryWindowEntity = entity;
        break;
    }
}

void JzInputSystem::OnShutdown(JzWorld &world)
{
    // Clear frame state on shutdown
    ClearFrameState(world);
}

void JzInputSystem::Update(JzWorld &world, F32 delta)
{
    // Sync legacy components from ECS input state (if available)
    SyncLegacyComponentsFromInputState(world);

    // Update input components from the raw input manager (fallback)
    UpdateMouseInput(world);
    UpdateKeyboardInput(world);

    // Update input actions
    UpdateInputActions(world, delta);

    // Update camera input
    UpdateCameraInput(world);
}

void JzInputSystem::ClearFrameState(JzWorld &world)
{
    // Clear per-frame state in all JzInputStateComponent
    auto view = world.View<JzInputStateComponent>();
    for (auto entity : view) {
        auto &input = world.GetComponent<JzInputStateComponent>(entity);
        input.ClearFrameState();
    }
}

JzInputStateComponent* JzInputSystem::GetPrimaryInputState(JzWorld &world)
{
    // Try cached entity first
    if (world.IsValid(m_primaryWindowEntity) &&
        world.HasComponent<JzInputStateComponent>(m_primaryWindowEntity)) {
        return &world.GetComponent<JzInputStateComponent>(m_primaryWindowEntity);
    }

    // Find primary window entity
    auto windowView = world.View<JzWindowStateComponent, JzInputStateComponent>();
    for (auto entity : windowView) {
        m_primaryWindowEntity = entity;
        return &world.GetComponent<JzInputStateComponent>(entity);
    }

    return nullptr;
}

void JzInputSystem::SyncLegacyComponentsFromInputState(JzWorld &world)
{
    // Get primary input state
    JzInputStateComponent *primaryInput = GetPrimaryInputState(world);
    if (!primaryInput) {
        return; // Will fall back to legacy JzInputManager path
    }

    const auto &inputState = *primaryInput;

    // Sync JzMouseInputComponent from JzInputStateComponent
    auto mouseView = world.View<JzMouseInputComponent>();
    for (auto entity : mouseView) {
        auto &mouseInput = world.GetComponent<JzMouseInputComponent>(entity);

        mouseInput.position      = inputState.mouse.position;
        mouseInput.positionDelta = inputState.mouse.positionDelta;
        mouseInput.scroll        = inputState.mouse.scrollDelta;

        mouseInput.leftButtonDown   = inputState.mouse.IsButtonPressed(JzEMouseButton::Left);
        mouseInput.rightButtonDown  = inputState.mouse.IsButtonPressed(JzEMouseButton::Right);
        mouseInput.middleButtonDown = inputState.mouse.IsButtonPressed(JzEMouseButton::Middle);

        mouseInput.leftButtonPressed   = inputState.mouse.IsButtonDown(JzEMouseButton::Left);
        mouseInput.rightButtonPressed  = inputState.mouse.IsButtonDown(JzEMouseButton::Right);
        mouseInput.middleButtonPressed = inputState.mouse.IsButtonDown(JzEMouseButton::Middle);

        mouseInput.leftButtonReleased   = inputState.mouse.IsButtonUp(JzEMouseButton::Left);
        mouseInput.rightButtonReleased  = inputState.mouse.IsButtonUp(JzEMouseButton::Right);
        mouseInput.middleButtonReleased = inputState.mouse.IsButtonUp(JzEMouseButton::Middle);
    }

    // Sync JzKeyboardInputComponent from JzInputStateComponent
    auto keyView = world.View<JzKeyboardInputComponent>();
    for (auto entity : keyView) {
        auto &keyInput = world.GetComponent<JzKeyboardInputComponent>(entity);

        keyInput.w = inputState.keyboard.IsKeyPressed(JzEKeyCode::W);
        keyInput.a = inputState.keyboard.IsKeyPressed(JzEKeyCode::A);
        keyInput.s = inputState.keyboard.IsKeyPressed(JzEKeyCode::S);
        keyInput.d = inputState.keyboard.IsKeyPressed(JzEKeyCode::D);

        keyInput.space = inputState.keyboard.IsKeyPressed(JzEKeyCode::Space);
        keyInput.shift = inputState.keyboard.IsKeyPressed(JzEKeyCode::LeftShift) ||
                         inputState.keyboard.IsKeyPressed(JzEKeyCode::RightShift);
        keyInput.ctrl  = inputState.keyboard.IsKeyPressed(JzEKeyCode::LeftControl) ||
                         inputState.keyboard.IsKeyPressed(JzEKeyCode::RightControl);
        keyInput.alt   = inputState.keyboard.IsKeyPressed(JzEKeyCode::LeftAlt) ||
                         inputState.keyboard.IsKeyPressed(JzEKeyCode::RightAlt);

        keyInput.escape = inputState.keyboard.IsKeyPressed(JzEKeyCode::Escape);
        keyInput.enter  = inputState.keyboard.IsKeyPressed(JzEKeyCode::Enter);
        keyInput.tab    = inputState.keyboard.IsKeyPressed(JzEKeyCode::Tab);

        keyInput.up    = inputState.keyboard.IsKeyPressed(JzEKeyCode::Up);
        keyInput.down  = inputState.keyboard.IsKeyPressed(JzEKeyCode::Down);
        keyInput.left  = inputState.keyboard.IsKeyPressed(JzEKeyCode::Left);
        keyInput.right = inputState.keyboard.IsKeyPressed(JzEKeyCode::Right);

        keyInput.f1 = inputState.keyboard.IsKeyPressed(JzEKeyCode::F1);
        keyInput.f2 = inputState.keyboard.IsKeyPressed(JzEKeyCode::F2);
        keyInput.f3 = inputState.keyboard.IsKeyPressed(JzEKeyCode::F3);
        keyInput.f4 = inputState.keyboard.IsKeyPressed(JzEKeyCode::F4);
    }

    // Sync JzCameraInputStateComponent from JzInputStateComponent
    auto cameraStateView = world.View<JzCameraInputStateComponent>();
    for (auto entity : cameraStateView) {
        auto &cameraState = world.GetComponent<JzCameraInputStateComponent>(entity);

        cameraState.orbitActive = inputState.mouse.IsButtonPressed(JzEMouseButton::Left);
        cameraState.panActive   = inputState.mouse.IsButtonPressed(JzEMouseButton::Right);
        cameraState.zoomActive  = inputState.mouse.IsButtonPressed(JzEMouseButton::Middle) ||
                                  (inputState.mouse.scrollDelta.y != 0.0f);

        if (cameraState.orbitActive || cameraState.panActive) {
            cameraState.mouseDelta = inputState.mouse.positionDelta;
        } else {
            cameraState.mouseDelta = JzVec2(0.0f, 0.0f);
        }

        cameraState.scrollDelta = inputState.mouse.scrollDelta.y;

        // Movement from keyboard
        cameraState.movementInput = JzVec3(0.0f);
        if (inputState.keyboard.IsKeyPressed(JzEKeyCode::W)) cameraState.movementInput.z -= 1.0f;
        if (inputState.keyboard.IsKeyPressed(JzEKeyCode::S)) cameraState.movementInput.z += 1.0f;
        if (inputState.keyboard.IsKeyPressed(JzEKeyCode::A)) cameraState.movementInput.x -= 1.0f;
        if (inputState.keyboard.IsKeyPressed(JzEKeyCode::D)) cameraState.movementInput.x += 1.0f;
        if (inputState.keyboard.IsKeyPressed(JzEKeyCode::Space)) cameraState.movementInput.y += 1.0f;
        if (inputState.keyboard.IsKeyPressed(JzEKeyCode::LeftControl)) cameraState.movementInput.y -= 1.0f;

        cameraState.speedBoost = inputState.keyboard.IsKeyPressed(JzEKeyCode::LeftShift);
        cameraState.resetRequested = inputState.keyboard.IsKeyDown(JzEKeyCode::R);
    }
}

void JzInputSystem::UpdateMouseInput(JzWorld &world)
{
    // Skip if we have ECS input state (already handled in SyncLegacyComponents)
    if (GetPrimaryInputState(world) != nullptr) {
        return;
    }

    // Legacy path: Get input manager from service container
    JzInputManager *inputManagerPtr = nullptr;
    try {
        inputManagerPtr = &JzServiceContainer::Get<JzInputManager>();
    } catch (...) {
        return;
    }

    auto &inputManager = *inputManagerPtr;

    // Get current mouse state
    JzVec2 currentMousePos = inputManager.GetMousePosition();
    JzVec2 scrollDelta     = inputManager.GetMouseScroll();

    // Calculate mouse delta
    JzVec2 mouseDelta{0.0f, 0.0f};
    if (!m_firstFrame) {
        mouseDelta = currentMousePos - m_previousMousePosition;
    }

    // Get mouse button states
    Bool leftDown   = inputManager.GetMouseButtonState(JzEInputMouseButton::MOUSE_BUTTON_LEFT) == JzEInputMouseButtonState::MOUSE_DOWN;
    Bool rightDown  = inputManager.GetMouseButtonState(JzEInputMouseButton::MOUSE_BUTTON_RIGHT) == JzEInputMouseButtonState::MOUSE_DOWN;
    Bool middleDown = inputManager.GetMouseButtonState(JzEInputMouseButton::MOUSE_BUTTON_MIDDLE) == JzEInputMouseButtonState::MOUSE_DOWN;

    // Update all entities with mouse input component
    auto view = world.View<JzMouseInputComponent>();
    for (auto entity : view) {
        auto &mouseInput = world.GetComponent<JzMouseInputComponent>(entity);

        // Store previous button states to detect press/release
        Bool prevLeftDown   = mouseInput.leftButtonDown;
        Bool prevRightDown  = mouseInput.rightButtonDown;
        Bool prevMiddleDown = mouseInput.middleButtonDown;

        // Update current state
        mouseInput.position      = currentMousePos;
        mouseInput.positionDelta = mouseDelta;
        mouseInput.scroll        = scrollDelta;

        mouseInput.leftButtonDown   = leftDown;
        mouseInput.rightButtonDown  = rightDown;
        mouseInput.middleButtonDown = middleDown;

        // Detect press/release events
        mouseInput.leftButtonPressed   = leftDown && !prevLeftDown;
        mouseInput.rightButtonPressed  = rightDown && !prevRightDown;
        mouseInput.middleButtonPressed = middleDown && !prevMiddleDown;

        mouseInput.leftButtonReleased   = !leftDown && prevLeftDown;
        mouseInput.rightButtonReleased  = !rightDown && prevRightDown;
        mouseInput.middleButtonReleased = !middleDown && prevMiddleDown;
    }

    // Update cached state
    m_previousMousePosition = currentMousePos;
    m_firstFrame            = false;
}

void JzInputSystem::UpdateKeyboardInput(JzWorld &world)
{
    // Skip if we have ECS input state (already handled in SyncLegacyComponents)
    if (GetPrimaryInputState(world) != nullptr) {
        return;
    }

    // Legacy path: Get input manager from service container
    JzInputManager *inputManagerPtr = nullptr;
    try {
        inputManagerPtr = &JzServiceContainer::Get<JzInputManager>();
    } catch (...) {
        return;
    }

    auto &inputManager = *inputManagerPtr;

    // Update all entities with keyboard input component
    auto view = world.View<JzKeyboardInputComponent>();
    for (auto entity : view) {
        auto &keyInput = world.GetComponent<JzKeyboardInputComponent>(entity);

        // Update common keys (WASD)
        keyInput.w = inputManager.GetKeyState(JzEInputKeyboardButton::KEY_W) == JzEInputKeyboardButtonState::KEY_DOWN;
        keyInput.a = inputManager.GetKeyState(JzEInputKeyboardButton::KEY_A) == JzEInputKeyboardButtonState::KEY_DOWN;
        keyInput.s = inputManager.GetKeyState(JzEInputKeyboardButton::KEY_S) == JzEInputKeyboardButtonState::KEY_DOWN;
        keyInput.d = inputManager.GetKeyState(JzEInputKeyboardButton::KEY_D) == JzEInputKeyboardButtonState::KEY_DOWN;

        // Modifier keys
        keyInput.space = inputManager.GetKeyState(JzEInputKeyboardButton::KEY_SPACE) == JzEInputKeyboardButtonState::KEY_DOWN;
        keyInput.shift = inputManager.GetKeyState(JzEInputKeyboardButton::KEY_LEFT_SHIFT) == JzEInputKeyboardButtonState::KEY_DOWN || inputManager.GetKeyState(JzEInputKeyboardButton::KEY_RIGHT_SHIFT) == JzEInputKeyboardButtonState::KEY_DOWN;
        keyInput.ctrl  = inputManager.GetKeyState(JzEInputKeyboardButton::KEY_LEFT_CONTROL) == JzEInputKeyboardButtonState::KEY_DOWN || inputManager.GetKeyState(JzEInputKeyboardButton::KEY_RIGHT_CONTROL) == JzEInputKeyboardButtonState::KEY_DOWN;
        keyInput.alt   = inputManager.GetKeyState(JzEInputKeyboardButton::KEY_LEFT_ALT) == JzEInputKeyboardButtonState::KEY_DOWN || inputManager.GetKeyState(JzEInputKeyboardButton::KEY_RIGHT_ALT) == JzEInputKeyboardButtonState::KEY_DOWN;

        // Special keys
        keyInput.escape = inputManager.GetKeyState(JzEInputKeyboardButton::KEY_ESCAPE) == JzEInputKeyboardButtonState::KEY_DOWN;
        keyInput.enter  = inputManager.GetKeyState(JzEInputKeyboardButton::KEY_ENTER) == JzEInputKeyboardButtonState::KEY_DOWN;
        keyInput.tab    = inputManager.GetKeyState(JzEInputKeyboardButton::KEY_TAB) == JzEInputKeyboardButtonState::KEY_DOWN;

        // Arrow keys
        keyInput.up    = inputManager.GetKeyState(JzEInputKeyboardButton::KEY_UP) == JzEInputKeyboardButtonState::KEY_DOWN;
        keyInput.down  = inputManager.GetKeyState(JzEInputKeyboardButton::KEY_DOWN) == JzEInputKeyboardButtonState::KEY_DOWN;
        keyInput.left  = inputManager.GetKeyState(JzEInputKeyboardButton::KEY_LEFT) == JzEInputKeyboardButtonState::KEY_DOWN;
        keyInput.right = inputManager.GetKeyState(JzEInputKeyboardButton::KEY_RIGHT) == JzEInputKeyboardButtonState::KEY_DOWN;

        // Function keys
        keyInput.f1 = inputManager.GetKeyState(JzEInputKeyboardButton::KEY_F1) == JzEInputKeyboardButtonState::KEY_DOWN;
        keyInput.f2 = inputManager.GetKeyState(JzEInputKeyboardButton::KEY_F2) == JzEInputKeyboardButtonState::KEY_DOWN;
        keyInput.f3 = inputManager.GetKeyState(JzEInputKeyboardButton::KEY_F3) == JzEInputKeyboardButtonState::KEY_DOWN;
        keyInput.f4 = inputManager.GetKeyState(JzEInputKeyboardButton::KEY_F4) == JzEInputKeyboardButtonState::KEY_DOWN;
    }
}

void JzInputSystem::UpdateCameraInput(JzWorld &world)
{
    // Find entities with camera input component and mouse input component
    auto view = world.View<JzCameraInputComponent, JzMouseInputComponent>();

    for (auto entity : view) {
        auto &cameraInput = world.GetComponent<JzCameraInputComponent>(entity);
        auto &mouseInput  = world.GetComponent<JzMouseInputComponent>(entity);

        // Update orbit/pan active states based on mouse buttons
        cameraInput.orbitActive = mouseInput.leftButtonDown;
        cameraInput.panActive   = mouseInput.rightButtonDown;

        // Update mouse delta (only if actively controlling)
        if (cameraInput.orbitActive || cameraInput.panActive) {
            cameraInput.mouseDelta = mouseInput.positionDelta;
        } else {
            cameraInput.mouseDelta = JzVec2(0.0f, 0.0f);
        }

        // Update scroll delta
        cameraInput.scrollDelta = mouseInput.scroll.y;

        // Reset request (pressing R key)
        if (world.HasComponent<JzKeyboardInputComponent>(entity)) {
            auto &keyInput = world.GetComponent<JzKeyboardInputComponent>(entity);
            // Note: Would need to track key down vs pressed for proper "just pressed" detection
            cameraInput.resetRequested = false;
        } else {
            cameraInput.resetRequested = false;
        }
    }
}

void JzInputSystem::UpdateInputActions(JzWorld &world, F32 delta)
{
    // Get primary input state
    JzInputStateComponent *inputState = GetPrimaryInputState(world);
    if (!inputState) {
        return;
    }

    // Update all action components
    auto view = world.View<JzInputActionComponent>();
    for (auto entity : view) {
        auto &actions = world.GetComponent<JzInputActionComponent>(entity);

        // Update each action
        for (auto &[name, action] : actions.actions) {
            // Calculate action value from all bindings (take max)
            F32 maxValue = 0.0f;

            for (const auto &binding : action.bindings) {
                F32 bindingValue = GetBindingValue(*inputState, binding);

                if (std::abs(bindingValue) > std::abs(maxValue)) {
                    maxValue = bindingValue;
                }
            }

            action.value = maxValue;
            action.Update(delta);
        }
    }
}

F32 JzInputSystem::GetBindingValue(const JzInputStateComponent &input,
                                   const JzInputActionComponent::Binding &binding)
{
    switch (binding.type) {
        case JzInputActionComponent::BindingType::Key:
            return input.keyboard.IsKeyPressed(binding.key) ? 1.0f : 0.0f;

        case JzInputActionComponent::BindingType::MouseButton:
            return input.mouse.IsButtonPressed(binding.mouseButton) ? 1.0f : 0.0f;

        case JzInputActionComponent::BindingType::MouseAxis: {
            F32 value = 0.0f;
            switch (binding.mouseAxis) {
                case 0: value = input.mouse.positionDelta.x; break; // X
                case 1: value = input.mouse.positionDelta.y; break; // Y
                case 2: value = input.mouse.scrollDelta.x; break;   // ScrollX
                case 3: value = input.mouse.scrollDelta.y; break;   // ScrollY
            }
            value *= binding.sensitivity;
            if (binding.invert) value = -value;
            return ApplyDeadzone(value, binding.deadzone);
        }

        case JzInputActionComponent::BindingType::GamepadButton:
            if (binding.gamepadId < static_cast<I32>(input.gamepad.pads.size())) {
                const auto &pad = input.gamepad.pads[static_cast<Size>(binding.gamepadId)];
                if (binding.gamepadButton < static_cast<I32>(pad.buttons.size())) {
                    return pad.buttons[static_cast<Size>(binding.gamepadButton)];
                }
            }
            return 0.0f;

        case JzInputActionComponent::BindingType::GamepadAxis:
            if (binding.gamepadId < static_cast<I32>(input.gamepad.pads.size())) {
                const auto &pad = input.gamepad.pads[static_cast<Size>(binding.gamepadId)];
                if (binding.gamepadAxis < static_cast<I32>(pad.axes.size())) {
                    F32 value = pad.axes[static_cast<Size>(binding.gamepadAxis)].x;
                    value *= binding.sensitivity;
                    if (binding.invert) value = -value;
                    return ApplyDeadzone(value, binding.deadzone);
                }
            }
            return 0.0f;

        default:
            return 0.0f;
    }
}

F32 JzInputSystem::ApplyDeadzone(F32 value, F32 deadzone)
{
    if (std::abs(value) < deadzone) {
        return 0.0f;
    }

    if (value > 0) {
        return (value - deadzone) / (1.0f - deadzone);
    } else {
        return (value + deadzone) / (1.0f - deadzone);
    }
}

} // namespace JzRE
