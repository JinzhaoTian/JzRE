#include "JzInputManager.h"

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

JzRE::EJzKeyState JzRE::JzInputManager::GetKeyState(EJzKey key) const
{
    switch (glfwGetKey(m_window.GetGLFWWindow(), static_cast<I32>(key))) {
        case GLFW_PRESS:
            return EJzKeyState::KEY_DOWN;
        case GLFW_RELEASE:
            return EJzKeyState::KEY_UP;
        default:
            return EJzKeyState::KEY_UP;
    }
}

JzRE::EJzMouseButtonState JzRE::JzInputManager::GetMouseButtonState(EJzMouseButton button) const
{
    switch (glfwGetMouseButton(m_window.GetGLFWWindow(), static_cast<I32>(button))) {
        case GLFW_PRESS:
            return EJzMouseButtonState::MOUSE_DOWN;
        case GLFW_RELEASE:
            return EJzMouseButtonState::MOUSE_UP;
        default:
            return EJzMouseButtonState::MOUSE_UP;
    }
}

JzRE::Bool JzRE::JzInputManager::IsKeyPressed(EJzKey key) const
{
    return m_keyStates.find(key) != m_keyStates.end() && m_keyStates.at(key) == EJzKeyState::KEY_DOWN;
}

JzRE::Bool JzRE::JzInputManager::IsKeyReleased(EJzKey key) const
{
    return m_keyStates.find(key) != m_keyStates.end() && m_keyStates.at(key) == EJzKeyState::KEY_UP;
}

JzRE::Bool JzRE::JzInputManager::IsMouseButtonPressed(EJzMouseButton button) const
{
    return m_mouseButtonStates.find(button) != m_mouseButtonStates.end() && m_mouseButtonStates.at(button) == EJzMouseButtonState::MOUSE_DOWN;
}

JzRE::Bool JzRE::JzInputManager::IsMouseButtonReleased(EJzMouseButton button) const
{
    return m_mouseButtonStates.find(button) != m_mouseButtonStates.end() && m_mouseButtonStates.at(button) == EJzMouseButtonState::MOUSE_UP;
}

std::pair<JzRE::F64, JzRE::F64> JzRE::JzInputManager::GetMousePosition() const
{
    std::pair<JzRE::F64, JzRE::F64> mousePosition;
    glfwGetCursorPos(m_window.GetGLFWWindow(), &mousePosition.first, &mousePosition.second);
    return mousePosition;
}

void JzRE::JzInputManager::ClearEvents()
{
    m_keyStates.clear();
    m_mouseButtonStates.clear();
}

void JzRE::JzInputManager::OnKeyPressed(I32 key)
{
    m_keyStates[static_cast<EJzKey>(key)] = EJzKeyState::KEY_DOWN;
}

void JzRE::JzInputManager::OnKeyReleased(I32 key)
{
    m_keyStates[static_cast<EJzKey>(key)] = EJzKeyState::KEY_UP;
}

void JzRE::JzInputManager::OnMouseButtonPressed(I32 button)
{
    m_mouseButtonStates[static_cast<EJzMouseButton>(button)] = EJzMouseButtonState::MOUSE_DOWN;
}

void JzRE::JzInputManager::OnMouseButtonReleased(I32 button)
{
    m_mouseButtonStates[static_cast<EJzMouseButton>(button)] = EJzMouseButtonState::MOUSE_UP;
}
