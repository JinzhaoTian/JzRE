/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Editor/JzInputManager.h"

JzRE::JzInputManager::JzInputManager(JzWindow &window) :
    m_window(window)
{
    m_keyPressedListenerID          = m_window.KeyPressedEvent.AddListener(std::bind(&JzInputManager::OnKeyPressed, this, std::placeholders::_1));
    m_keyReleasedListenerID         = m_window.KeyReleasedEvent.AddListener(std::bind(&JzInputManager::OnKeyReleased, this, std::placeholders::_1));
    m_mouseButtonPressedListenerID  = m_window.MouseButtonPressedEvent.AddListener(std::bind(&JzInputManager::OnMouseButtonPressed, this, std::placeholders::_1));
    m_mouseButtonReleasedListenerID = m_window.MouseButtonReleasedEvent.AddListener(std::bind(&JzInputManager::OnMouseButtonReleased, this, std::placeholders::_1));
}

JzRE::JzInputManager::~JzInputManager()
{
    m_window.KeyPressedEvent.RemoveListener(m_keyPressedListenerID);
    m_window.KeyReleasedEvent.RemoveListener(m_keyReleasedListenerID);
    m_window.MouseButtonPressedEvent.RemoveListener(m_mouseButtonPressedListenerID);
    m_window.MouseButtonReleasedEvent.RemoveListener(m_mouseButtonReleasedListenerID);
}

JzRE::JzEInputKeyState JzRE::JzInputManager::GetKeyState(JzEInputKey key) const
{
    switch (glfwGetKey(m_window.GetGLFWWindow(), static_cast<I32>(key))) {
        case GLFW_PRESS:
            return JzEInputKeyState::KEY_DOWN;
        case GLFW_RELEASE:
            return JzEInputKeyState::KEY_UP;
        default:
            return JzEInputKeyState::KEY_UP;
    }
}

JzRE::JzEInputMouseButtonState JzRE::JzInputManager::GetMouseButtonState(JzEInputMouseButton button) const
{
    switch (glfwGetMouseButton(m_window.GetGLFWWindow(), static_cast<I32>(button))) {
        case GLFW_PRESS:
            return JzEInputMouseButtonState::MOUSE_DOWN;
        case GLFW_RELEASE:
            return JzEInputMouseButtonState::MOUSE_UP;
        default:
            return JzEInputMouseButtonState::MOUSE_UP;
    }
}

JzRE::Bool JzRE::JzInputManager::IsKeyPressed(JzEInputKey key) const
{
    return m_keyStates.find(key) != m_keyStates.end() && m_keyStates.at(key) == JzEInputKeyState::KEY_DOWN;
}

JzRE::Bool JzRE::JzInputManager::IsKeyReleased(JzEInputKey key) const
{
    return m_keyStates.find(key) != m_keyStates.end() && m_keyStates.at(key) == JzEInputKeyState::KEY_UP;
}

JzRE::Bool JzRE::JzInputManager::IsMouseButtonPressed(JzEInputMouseButton button) const
{
    return m_mouseButtonStates.find(button) != m_mouseButtonStates.end() && m_mouseButtonStates.at(button) == JzEInputMouseButtonState::MOUSE_DOWN;
}

JzRE::Bool JzRE::JzInputManager::IsMouseButtonReleased(JzEInputMouseButton button) const
{
    return m_mouseButtonStates.find(button) != m_mouseButtonStates.end() && m_mouseButtonStates.at(button) == JzEInputMouseButtonState::MOUSE_UP;
}

std::pair<JzRE::F64, JzRE::F64> JzRE::JzInputManager::GetMousePosition() const
{
    std::pair<JzRE::F64, JzRE::F64> mousePosition;
    glfwGetCursorPos(m_window.GetGLFWWindow(), &mousePosition.first, &mousePosition.second);
    return mousePosition;
}

std::pair<JzRE::F64, JzRE::F64> JzRE::JzInputManager::GetMouseScroll() const
{
    return m_mouseScroll;
}

void JzRE::JzInputManager::ClearEvents()
{
    m_keyStates.clear();
    m_mouseButtonStates.clear();
}

void JzRE::JzInputManager::OnKeyPressed(I32 key)
{
    m_keyStates[static_cast<JzEInputKey>(key)] = JzEInputKeyState::KEY_DOWN;
}

void JzRE::JzInputManager::OnKeyReleased(I32 key)
{
    m_keyStates[static_cast<JzEInputKey>(key)] = JzEInputKeyState::KEY_UP;
}

void JzRE::JzInputManager::OnMouseButtonPressed(I32 button)
{
    m_mouseButtonStates[static_cast<JzEInputMouseButton>(button)] = JzEInputMouseButtonState::MOUSE_DOWN;
}

void JzRE::JzInputManager::OnMouseButtonReleased(I32 button)
{
    m_mouseButtonStates[static_cast<JzEInputMouseButton>(button)] = JzEInputMouseButtonState::MOUSE_UP;
}
