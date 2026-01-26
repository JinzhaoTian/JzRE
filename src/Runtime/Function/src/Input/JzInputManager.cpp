/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Function/Input/JzInputManager.h"

JzRE::JzInputManager::JzInputManager(JzWindowSystem &windowSystem) :
    m_windowSystem(windowSystem)
{
    m_keyboardButtonPressedListenerID = m_windowSystem.KeyboardButtonPressedEvent += [this](I32 keyboardButton) {
        m_keyboardButtonStates[static_cast<JzEInputKeyboardButton>(keyboardButton)] = JzEInputKeyboardButtonState::KEY_DOWN;
    };
    m_keyboardButtonReleasedListenerID = m_windowSystem.KeyboardButtonReleasedEvent += [this](I32 keyboardButton) {
        m_keyboardButtonStates[static_cast<JzEInputKeyboardButton>(keyboardButton)] = JzEInputKeyboardButtonState::KEY_UP;
    };
    m_mouseButtonPressedListenerID = m_windowSystem.MouseButtonPressedEvent += [this](I32 mouseButton) {
        m_mouseButtonStates[static_cast<JzEInputMouseButton>(mouseButton)] = JzEInputMouseButtonState::MOUSE_DOWN;
    };
    m_mouseButtonReleasedListenerID = m_windowSystem.MouseButtonReleasedEvent += [this](I32 mouseButton) {
        m_mouseButtonStates[static_cast<JzEInputMouseButton>(mouseButton)] = JzEInputMouseButtonState::MOUSE_UP;
    };
    m_mouseScrollListenerID = m_windowSystem.MouseScrolledEvent += [this](JzVec2 scroll) {
        m_mouseScroll = scroll;
    };
}

JzRE::JzInputManager::~JzInputManager()
{
    m_windowSystem.KeyboardButtonPressedEvent  -= m_keyboardButtonPressedListenerID;
    m_windowSystem.KeyboardButtonReleasedEvent -= m_keyboardButtonReleasedListenerID;
    m_windowSystem.MouseButtonPressedEvent     -= m_mouseButtonPressedListenerID;
    m_windowSystem.MouseButtonReleasedEvent    -= m_mouseButtonReleasedListenerID;
    m_windowSystem.MouseScrolledEvent          -= m_mouseScrollListenerID;
}

JzRE::JzEInputKeyboardButtonState JzRE::JzInputManager::GetKeyState(JzEInputKeyboardButton key) const
{
    return m_windowSystem.GetKeyState(static_cast<I32>(key))
        ? JzEInputKeyboardButtonState::KEY_DOWN
        : JzEInputKeyboardButtonState::KEY_UP;
}

JzRE::JzEInputMouseButtonState JzRE::JzInputManager::GetMouseButtonState(JzEInputMouseButton button) const
{
    return m_windowSystem.GetMouseButtonState(static_cast<I32>(button))
        ? JzEInputMouseButtonState::MOUSE_DOWN
        : JzEInputMouseButtonState::MOUSE_UP;
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
