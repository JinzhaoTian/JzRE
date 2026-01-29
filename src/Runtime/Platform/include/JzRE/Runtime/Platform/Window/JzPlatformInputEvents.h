/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <vector>

#include "JzRE/Runtime/Core/JzPlatformEvent.h"
#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzVector.h"

namespace JzRE {

// ==================== Keyboard Events ====================

/**
 * @brief Platform keyboard key event
 */
struct JzPlatformKeyEvent : public JzPlatformEvent {
    I32 key{0};      ///< Key code (GLFW key codes)
    I32 scancode{0}; ///< Platform-specific scancode
    I32 action{0};   ///< 0=release, 1=press, 2=repeat
    I32 mods{0};     ///< Modifier keys bitmask
};

// ==================== Mouse Events ====================

/**
 * @brief Platform mouse button event
 */
struct JzPlatformMouseButtonEvent : public JzPlatformEvent {
    I32    button{0}; ///< Mouse button (0=left, 1=right, 2=middle, etc.)
    I32    action{0}; ///< 0=release, 1=press
    I32    mods{0};   ///< Modifier keys bitmask
    JzVec2 position;  ///< Mouse position at time of event
};

/**
 * @brief Platform mouse move event
 */
struct JzPlatformMouseMoveEvent : public JzPlatformEvent {
    JzVec2 position; ///< Current cursor position
};

/**
 * @brief Platform mouse scroll event
 */
struct JzPlatformMouseScrollEvent : public JzPlatformEvent {
    JzVec2 offset; ///< Scroll delta (x, y)
};

/**
 * @brief Platform mouse enter/leave window event
 */
struct JzPlatformMouseEnterEvent : public JzPlatformEvent {
    Bool entered{false}; ///< true = entered, false = left
};

// ==================== Window Events ====================

/**
 * @brief Platform window resize event
 */
struct JzPlatformWindowResizeEvent : public JzPlatformEvent {
    JzIVec2 size{0}; ///< New window size
};

/**
 * @brief Platform framebuffer resize event (for HiDPI displays)
 */
struct JzPlatformFramebufferResizeEvent : public JzPlatformEvent {
    JzIVec2 size{0}; ///< New framebuffer size
};

/**
 * @brief Platform window move event
 */
struct JzPlatformWindowMoveEvent : public JzPlatformEvent {
    JzIVec2 position{0}; ///< New window position
};

/**
 * @brief Platform window focus event
 */
struct JzPlatformWindowFocusEvent : public JzPlatformEvent {
    Bool focused{false}; ///< true = gained focus, false = lost focus
};

/**
 * @brief Platform window iconify (minimize) event
 */
struct JzPlatformWindowIconifyEvent : public JzPlatformEvent {
    Bool iconified{false}; ///< true = minimized, false = restored
};

/**
 * @brief Platform window maximize event
 */
struct JzPlatformWindowMaximizeEvent : public JzPlatformEvent {
    Bool maximized{false}; ///< true = maximized, false = restored
};

/**
 * @brief Platform window close event
 */
struct JzPlatformWindowCloseEvent : public JzPlatformEvent {
    // No additional data needed
};

/**
 * @brief Platform file drop event
 */
struct JzPlatformFileDropEvent : public JzPlatformEvent {
    std::vector<String> paths;    ///< Dropped file paths
    JzVec2              position; ///< Drop position
};

/**
 * @brief Platform content scale change event (DPI change)
 */
struct JzPlatformContentScaleEvent : public JzPlatformEvent {
    JzVec2 scale{1.0f, 1.0f}; ///< Content scale (x, y)
};

/**
 * @brief Platform character input event (for text input)
 */
struct JzPlatformCharEvent : public JzPlatformEvent {
    U32 codepoint{0}; ///< Unicode codepoint
};

} // namespace JzRE
