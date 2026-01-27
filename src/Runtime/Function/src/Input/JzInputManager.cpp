/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Function/Input/JzInputManager.h"

#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Runtime/Function/Event/JzEventDispatcherSystem.h"
#include "JzRE/Runtime/Function/Event/JzInputEvents.h"

JzRE::JzInputManager::JzInputManager(JzWindowSystem &windowSystem) :
    m_windowSystem(windowSystem)
{
    // Register ECS event handlers on the event dispatcher
    try {
        auto &dispatcher = JzServiceContainer::Get<JzEventDispatcherSystem>();

        // Keyboard events: update button state from JzKeyEvent
        m_keyEventHandler = dispatcher.RegisterHandler<JzKeyEvent>(
            [this](const JzKeyEvent &event) {
                auto key = static_cast<JzEInputKeyboardButton>(static_cast<I32>(event.key));
                if (event.action == JzEKeyAction::Pressed) {
                    m_keyboardButtonStates[key] = JzEInputKeyboardButtonState::KEY_DOWN;
                } else if (event.action == JzEKeyAction::Released) {
                    m_keyboardButtonStates[key] = JzEInputKeyboardButtonState::KEY_UP;
                }
            });

        // Mouse button events: update button state from JzMouseButtonEvent
        m_mouseButtonEventHandler = dispatcher.RegisterHandler<JzMouseButtonEvent>(
            [this](const JzMouseButtonEvent &event) {
                auto button = static_cast<JzEInputMouseButton>(static_cast<I32>(event.button));
                if (event.action == JzEKeyAction::Pressed) {
                    m_mouseButtonStates[button] = JzEInputMouseButtonState::MOUSE_DOWN;
                } else if (event.action == JzEKeyAction::Released) {
                    m_mouseButtonStates[button] = JzEInputMouseButtonState::MOUSE_UP;
                }
            });

        // Mouse scroll events: update scroll from JzMouseScrollEvent
        m_mouseScrollEventHandler = dispatcher.RegisterHandler<JzMouseScrollEvent>(
            [this](const JzMouseScrollEvent &event) {
                m_mouseScroll = event.offset;
            });
    } catch (...) {
        // JzEventDispatcherSystem not yet registered; handlers remain null
    }
}

JzRE::JzInputManager::~JzInputManager()
{
    try {
        auto &dispatcher = JzServiceContainer::Get<JzEventDispatcherSystem>();
        if (m_keyEventHandler) dispatcher.RemoveHandler(m_keyEventHandler);
        if (m_mouseButtonEventHandler) dispatcher.RemoveHandler(m_mouseButtonEventHandler);
        if (m_mouseScrollEventHandler) dispatcher.RemoveHandler(m_mouseScrollEventHandler);
    } catch (...) {
        // Dispatcher already destroyed; nothing to clean up
    }
}

JzRE::JzEInputKeyboardButtonState JzRE::JzInputManager::GetKeyState(JzEInputKeyboardButton key) const
{
    return m_windowSystem.GetKeyState(static_cast<I32>(key)) ? JzEInputKeyboardButtonState::KEY_DOWN : JzEInputKeyboardButtonState::KEY_UP;
}

JzRE::JzEInputMouseButtonState JzRE::JzInputManager::GetMouseButtonState(JzEInputMouseButton button) const
{
    return m_windowSystem.GetMouseButtonState(static_cast<I32>(button)) ? JzEInputMouseButtonState::MOUSE_DOWN : JzEInputMouseButtonState::MOUSE_UP;
}

JzRE::Bool JzRE::JzInputManager::IsKeyPressed(JzEInputKeyboardButton key) const
{
    return m_keyboardButtonStates.find(key) != m_keyboardButtonStates.end() && m_keyboardButtonStates.at(key) == JzEInputKeyboardButtonState::KEY_DOWN;
}

JzRE::Bool JzRE::JzInputManager::IsKeyReleased(JzEInputKeyboardButton key) const
{
    return m_keyboardButtonStates.find(key) != m_keyboardButtonStates.end() && m_keyboardButtonStates.at(key) == JzEInputKeyboardButtonState::KEY_UP;
}

JzRE::Bool JzRE::JzInputManager::IsMouseButtonPressed(JzEInputMouseButton button) const
{
    return m_mouseButtonStates.find(button) != m_mouseButtonStates.end() && m_mouseButtonStates.at(button) == JzEInputMouseButtonState::MOUSE_DOWN;
}

JzRE::Bool JzRE::JzInputManager::IsMouseButtonReleased(JzEInputMouseButton button) const
{
    return m_mouseButtonStates.find(button) != m_mouseButtonStates.end() && m_mouseButtonStates.at(button) == JzEInputMouseButtonState::MOUSE_UP;
}

JzRE::JzVec2 JzRE::JzInputManager::GetMousePosition() const
{
    return m_windowSystem.GetCursorPosition();
}

JzRE::JzVec2 JzRE::JzInputManager::GetMouseScroll() const
{
    return m_mouseScroll;
}

void JzRE::JzInputManager::ClearEvents()
{
    m_keyboardButtonStates.clear();
    m_mouseButtonStates.clear();
    m_mouseScroll = JzVec2(0.0f, 0.0f);
}
