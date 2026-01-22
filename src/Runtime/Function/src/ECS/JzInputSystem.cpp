/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Function/ECS/JzInputSystem.h"

#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Runtime/Function/ECS/JzComponents.h"
#include "JzRE/Runtime/Function/Input/JzInputManager.h"

namespace JzRE {

void JzInputSystem::OnInit(JzWorld &world)
{
    // Initialize by ensuring at least one entity has input components
    // In a real game, you'd create input components for the player entity, UI, etc.
    // For now, we'll create them on-demand in Update
}

void JzInputSystem::Update(JzWorld &world, F32 delta)
{
    // Update input components from the raw input manager
    UpdateMouseInput(world);
    UpdateKeyboardInput(world);
    UpdateCameraInput(world);
}

void JzInputSystem::UpdateMouseInput(JzWorld &world)
{
    // Get input manager from service container
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
    // Get input manager from service container
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

        // Reset request (example: pressing R key)
        // For now, we'll leave this as false; can be extended with keyboard input
        cameraInput.resetRequested = false;
    }
}

} // namespace JzRE
