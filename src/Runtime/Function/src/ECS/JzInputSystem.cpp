/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Function/ECS/JzInputSystem.h"

#include "JzRE/Runtime/Function/ECS/JzCameraComponents.h"
#include "JzRE/Runtime/Function/ECS/JzWindowComponents.h"
#include "JzRE/Runtime/Function/Event/JzEventSystem.h"
#include "JzRE/Runtime/Function/Event/JzInputEvents.h"

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
    // Sync legacy components from ECS input state
    SyncLegacyComponentsFromInputState(world);

    // Update input actions
    UpdateInputActions(world, delta);

    // Update camera input
    UpdateCameraInput(world);

    // Emit typed ECS events through the event dispatcher
    EmitKeyboardEvents(world);
    EmitMouseEvents(world);
    EmitActionEvents(world);
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

JzInputStateComponent *JzInputSystem::GetPrimaryInputState(JzWorld &world)
{
    // Try cached entity first
    if (world.IsValid(m_primaryWindowEntity) && world.HasComponent<JzInputStateComponent>(m_primaryWindowEntity)) {
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
        return;
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
        keyInput.shift = inputState.keyboard.IsKeyPressed(JzEKeyCode::LeftShift) || inputState.keyboard.IsKeyPressed(JzEKeyCode::RightShift);
        keyInput.ctrl  = inputState.keyboard.IsKeyPressed(JzEKeyCode::LeftControl) || inputState.keyboard.IsKeyPressed(JzEKeyCode::RightControl);
        keyInput.alt   = inputState.keyboard.IsKeyPressed(JzEKeyCode::LeftAlt) || inputState.keyboard.IsKeyPressed(JzEKeyCode::RightAlt);

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
        cameraState.zoomActive  = inputState.mouse.IsButtonPressed(JzEMouseButton::Middle) || (inputState.mouse.scrollDelta.y != 0.0f);

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

        cameraState.speedBoost     = inputState.keyboard.IsKeyPressed(JzEKeyCode::LeftShift);
        cameraState.resetRequested = inputState.keyboard.IsKeyDown(JzEKeyCode::R);
    }
}

void JzInputSystem::UpdateCameraInput(JzWorld &world)
{
    // Find entities with camera input component and mouse input component
    auto view = world.View<JzCameraInputComponent, JzMouseInputComponent>();

    for (auto entity : view) {
        if (world.HasComponent<JzEditorCameraInputOverrideTag>(entity)) {
            continue;
        }
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

F32 JzInputSystem::GetBindingValue(const JzInputStateComponent           &input,
                                   const JzInputActionComponent::Binding &binding)
{
    switch (binding.type) {
        case JzInputActionComponent::BindingType::Key:
            return input.keyboard.IsKeyPressed(binding.key) ? 1.0f : 0.0f;

        case JzInputActionComponent::BindingType::MouseButton:
            return input.mouse.IsButtonPressed(binding.mouseButton) ? 1.0f : 0.0f;

        case JzInputActionComponent::BindingType::MouseAxis:
        {
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
                    F32 value  = pad.axes[static_cast<Size>(binding.gamepadAxis)].x;
                    value     *= binding.sensitivity;
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

void JzInputSystem::EmitKeyboardEvents(JzWorld &world)
{
    JzInputStateComponent *inputState = GetPrimaryInputState(world);
    if (!inputState) return;

    JzEventSystem **dispatcherPtr = world.TryGetContext<JzEventSystem*>();
    if (!dispatcherPtr || !*dispatcherPtr) return;
    JzEventSystem *dispatcher = *dispatcherPtr;

    const auto &keyboard = inputState->keyboard;

    for (I32 key = 0; key < static_cast<I32>(JzInputStateComponent::KeyboardState::KEY_COUNT); ++key) {
        Bool currentPressed = keyboard.keysPressed[static_cast<Size>(key)];
        Bool wasPressed     = m_prevKeysPressed[static_cast<Size>(key)];

        if (currentPressed && !wasPressed) {
            JzKeyEvent event;
            event.source = m_primaryWindowEntity;
            event.key    = static_cast<JzEKeyCode>(key);
            event.action = JzEKeyAction::Pressed;
            dispatcher->Send(std::move(event));
        } else if (!currentPressed && wasPressed) {
            JzKeyEvent event;
            event.source = m_primaryWindowEntity;
            event.key    = static_cast<JzEKeyCode>(key);
            event.action = JzEKeyAction::Released;
            dispatcher->Send(std::move(event));
        }
    }

    m_prevKeysPressed = keyboard.keysPressed;
}

void JzInputSystem::EmitMouseEvents(JzWorld &world)
{
    JzInputStateComponent *inputState = GetPrimaryInputState(world);
    if (!inputState) return;

    JzEventSystem **dispatcherPtr = world.TryGetContext<JzEventSystem*>();
    if (!dispatcherPtr || !*dispatcherPtr) return;
    JzEventSystem *dispatcher = *dispatcherPtr;

    const auto &mouse = inputState->mouse;

    // Mouse button events
    for (I32 button = 0; button < static_cast<I32>(JzInputStateComponent::MouseState::BUTTON_COUNT); ++button) {
        Bool currentPressed = mouse.buttonsPressed[static_cast<Size>(button)];
        Bool wasPressed     = m_prevButtonsPressed[static_cast<Size>(button)];

        if (currentPressed && !wasPressed) {
            JzMouseButtonEvent event;
            event.source   = m_primaryWindowEntity;
            event.button   = static_cast<JzEMouseButton>(button);
            event.action   = JzEKeyAction::Pressed;
            event.position = mouse.position;
            dispatcher->Send(std::move(event));
        } else if (!currentPressed && wasPressed) {
            JzMouseButtonEvent event;
            event.source   = m_primaryWindowEntity;
            event.button   = static_cast<JzEMouseButton>(button);
            event.action   = JzEKeyAction::Released;
            event.position = mouse.position;
            dispatcher->Send(std::move(event));
        }
    }
    m_prevButtonsPressed = mouse.buttonsPressed;

    // Mouse move event (only if delta is non-zero)
    if (mouse.positionDelta.x != 0.0f || mouse.positionDelta.y != 0.0f) {
        JzMouseMoveEvent event;
        event.source   = m_primaryWindowEntity;
        event.position = mouse.position;
        event.delta    = mouse.positionDelta;
        dispatcher->Send(std::move(event));
    }

    // Mouse scroll event (only if scroll delta is non-zero)
    if (mouse.scrollDelta.x != 0.0f || mouse.scrollDelta.y != 0.0f) {
        JzMouseScrollEvent event;
        event.source   = m_primaryWindowEntity;
        event.offset   = mouse.scrollDelta;
        event.position = mouse.position;
        dispatcher->Send(std::move(event));
    }
}

void JzInputSystem::EmitActionEvents(JzWorld &world)
{
    JzEventSystem **dispatcherPtr = world.TryGetContext<JzEventSystem*>();
    if (!dispatcherPtr || !*dispatcherPtr) return;
    JzEventSystem *dispatcher = *dispatcherPtr;

    auto view = world.View<JzInputActionComponent>();
    for (auto entity : view) {
        const auto &actions = world.GetComponent<JzInputActionComponent>(entity);

        for (const auto &[name, action] : actions.actions) {
            if (action.triggered) {
                JzInputActionTriggeredEvent event;
                event.source     = entity;
                event.actionName = name;
                event.value      = action.value;
                dispatcher->Send(std::move(event));
            }
            if (action.released) {
                JzInputActionReleasedEvent event;
                event.source     = entity;
                event.actionName = name;
                event.duration   = action.pressedDuration;
                dispatcher->Send(std::move(event));
            }
        }
    }
}

} // namespace JzRE
