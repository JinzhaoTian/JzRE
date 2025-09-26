/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRETypes.h"
#include "JzEInput.h"
#include "JzEvent.h"
#include "JzWindow.h"

namespace JzRE {
/**
 * @brief Input Manager Class of JzRE
 */
class JzInputManager {
public:
    /**
     * @brief Constructor
     *
     * @param window
     */
    JzInputManager(JzWindow &window);

    /**
     * @brief Destructor
     */
    ~JzInputManager();

    /**
     * @brief Get the key state
     *
     * @param key
     * @return JzEInputKeyState
     */
    JzEInputKeyState GetKeyState(JzEInputKey key) const;

    /**
     * @brief Get the mouse button state
     *
     * @param button
     * @return JzEInputMouseButtonState
     */
    JzEInputMouseButtonState GetMouseButtonState(JzEInputMouseButton button) const;

    /**
     * @brief Check if a key is pressed
     *
     * @param key
     * @return Bool
     */
    Bool IsKeyPressed(JzEInputKey key) const;

    /**
     * @brief Check if a key is released
     *
     * @param key
     * @return Bool
     */
    Bool IsKeyReleased(JzEInputKey key) const;

    /**
     * @brief Check if a mouse button is pressed
     *
     * @param button
     * @return Bool
     */
    Bool IsMouseButtonPressed(JzEInputMouseButton button) const;

    /**
     * @brief Check if a mouse button is released
     *
     * @param button
     * @return Bool
     */
    Bool IsMouseButtonReleased(JzEInputMouseButton button) const;

    /**
     * @brief Get the mouse position
     *
     * @return std::pair<F64, F64>
     */
    std::pair<F64, F64> GetMousePosition() const;

    /**
     * @brief Get the mouse scroll
     *
     * @return std::pair<F64, F64>
     */
    std::pair<F64, F64> GetMouseScroll() const;

    /**
     * @brief Clear the events
     */
    void ClearEvents();

private:
    void OnKeyPressed(I32 key);
    void OnKeyReleased(I32 key);
    void OnMouseButtonPressed(I32 button);
    void OnMouseButtonReleased(I32 button);

private:
    JzWindow                                                         &m_window;
    ListenerID                                                        m_keyPressedListenerID;
    ListenerID                                                        m_keyReleasedListenerID;
    ListenerID                                                        m_mouseButtonPressedListenerID;
    ListenerID                                                        m_mouseButtonReleasedListenerID;
    std::unordered_map<JzEInputKey, JzEInputKeyState>                 m_keyStates;
    std::unordered_map<JzEInputMouseButton, JzEInputMouseButtonState> m_mouseButtonStates;
    std::pair<F64, F64>                                               m_mouseScroll;
};
} // namespace JzRE