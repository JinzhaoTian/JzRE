/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <vector>
#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzVector.h"
#include "JzRE/Runtime/Function/Event/JzECSEvent.h"

namespace JzRE {

/**
 * @brief Window resized event
 */
struct JzWindowResizedEvent : public JzECSEvent {
    JzIVec2 size;    ///< New window size
    JzIVec2 oldSize; ///< Previous window size
};

/**
 * @brief Window framebuffer resized event (for HiDPI)
 */
struct JzWindowFramebufferResizedEvent : public JzECSEvent {
    JzIVec2 size; ///< New framebuffer size
};

/**
 * @brief Window moved event
 */
struct JzWindowMovedEvent : public JzECSEvent {
    JzIVec2 position; ///< New window position
};

/**
 * @brief Window focus changed event
 */
struct JzWindowFocusEvent : public JzECSEvent {
    Bool focused; ///< true = gained focus, false = lost focus
};

/**
 * @brief Window iconified (minimized/restored) event
 */
struct JzWindowIconifiedEvent : public JzECSEvent {
    Bool iconified; ///< true = minimized, false = restored
};

/**
 * @brief Window maximized event
 */
struct JzWindowMaximizedEvent : public JzECSEvent {
    Bool maximized; ///< true = maximized, false = restored
};

/**
 * @brief Window closed event
 */
struct JzWindowClosedEvent : public JzECSEvent {
    Bool forced{false}; ///< Whether it was a forced close
};

/**
 * @brief File drop event
 */
struct JzFileDroppedEvent : public JzECSEvent {
    std::vector<String> filePaths;
    JzVec2              dropPosition;
};

/**
 * @brief Content scale changed event (DPI change)
 */
struct JzWindowContentScaleChangedEvent : public JzECSEvent {
    JzVec2 scale; ///< New content scale
};

} // namespace JzRE
