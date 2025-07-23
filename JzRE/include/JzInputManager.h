#pragma once

#include "CommonTypes.h"
#include "JzEvent.h"
#include "JzInputEnums.h"
#include "JzWindow.h"

namespace JzRE {
/**
 * @brief Input manager class
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
     * @return EJzKeyState
     */
    EJzKeyState GetKeyState(EJzKey key) const;

    /**
     * @brief Get the mouse button state
     *
     * @param button
     * @return EJzMouseButtonState
     */
    EJzMouseButtonState GetMouseButtonState(EJzMouseButton button) const;

    /**
     * @brief Check if a key is pressed
     *
     * @param key
     * @return Bool
     */
    Bool IsKeyPressed(EJzKey key) const;

    /**
     * @brief Check if a key is released
     *
     * @param key
     * @return Bool
     */
    Bool IsKeyReleased(EJzKey key) const;

    /**
     * @brief Check if a mouse button is pressed
     *
     * @param button
     * @return Bool
     */
    Bool IsMouseButtonPressed(EJzMouseButton button) const;

    /**
     * @brief Check if a mouse button is released
     *
     * @param button
     * @return Bool
     */
    Bool IsMouseButtonReleased(EJzMouseButton button) const;

    /**
     * @brief Get the mouse position
     *
     * @return EJzVector2
     */
    std::pair<F64, F64> GetMousePosition() const;

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
    JzWindow &m_window;

    ListenerID m_keyPressedListenerID;
    ListenerID m_keyReleasedListenerID;
    ListenerID m_mouseButtonPressedListenerID;
    ListenerID m_mouseButtonReleasedListenerID;

    std::unordered_map<EJzKey, EJzKeyState>                 m_keyStates;
    std::unordered_map<EJzMouseButton, EJzMouseButtonState> m_mouseButtonStates;
};
} // namespace JzRE