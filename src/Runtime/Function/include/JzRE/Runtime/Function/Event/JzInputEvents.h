/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzVector.h"
#include "JzRE/Runtime/Function/Event/JzEventBase.h"
#include "JzRE/Runtime/Function/ECS/JzInputComponents.h"

namespace JzRE {

/**
 * @brief Key action enumeration (GLFW compatible)
 */
enum class JzEKeyAction : I32 {
    Released = 0, // GLFW_RELEASE
    Pressed  = 1, // GLFW_PRESS
    Repeat   = 2  // GLFW_REPEAT
};

/**
 * @brief Keyboard key event
 */
struct JzKeyEvent : public JzREEvent {
    JzEKeyCode   key;
    I32          scancode{0};
    JzEKeyAction action;
    I32          mods{0}; ///< Modifier keys bitmask
};

/**
 * @brief Mouse button event
 */
struct JzMouseButtonEvent : public JzREEvent {
    JzEMouseButton button;
    JzEKeyAction   action;
    I32            mods{0};
    JzVec2         position; ///< Mouse position at time of event
};

/**
 * @brief Mouse move event
 */
struct JzMouseMoveEvent : public JzREEvent {
    JzVec2 position; ///< Current mouse position
    JzVec2 delta;    ///< Movement since last frame
};

/**
 * @brief Mouse scroll event
 */
struct JzMouseScrollEvent : public JzREEvent {
    JzVec2 offset;   ///< Scroll delta (x, y)
    JzVec2 position; ///< Mouse position at time of scroll
};

/**
 * @brief Mouse enter/leave window event
 */
struct JzMouseEnterEvent : public JzREEvent {
    Bool entered; ///< true = entered, false = left
};

/**
 * @brief Input action triggered event (high-level)
 */
struct JzInputActionTriggeredEvent : public JzREEvent {
    String actionName;
    F32    value{0.0f};
};

/**
 * @brief Input action released event (high-level)
 */
struct JzInputActionReleasedEvent : public JzREEvent {
    String actionName;
    F32    duration{0.0f}; ///< How long the action was held
};

} // namespace JzRE
