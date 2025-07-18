#pragma once

#include "CommonTypes.h"

namespace JzRE {
/**
 * @brief Window settings
 */
struct JzWindowSettings {
    /**
     * @brief Default value
     */
    static const I32 DefaultValue = -1;
    /**
     * @brief Title of the window
     */
    String title = "JzRE";

    /**
     * @brief Width of the window
     */
    U16 width = 800;

    /**
     * @brief Height of the window
     */
    U16 height = 600;

    /**
     * @brief X position of the window
     */
    I16 x = 0;

    /**
     * @brief Y position of the window
     */
    I16 y = 0;

    /**
     * @brief Minimum width of the window
     */
    I16 minWidth = DefaultValue;

    /**
     * @brief Minimum height of the window
     */
    I16 minHeight = DefaultValue;

    /**
     * @brief Maximum width of the window
     */
    I16 maxWidth = DefaultValue;

    /**
     * @brief Maximum height of the window
     */
    I16 maxHeight = DefaultValue;

    /**
     * @brief Is the window fullscreen
     */
    Bool isFullscreen = false;

    /**
     * @brief Is the window resizable
     */
    Bool isResizable = true;

    /**
     * @brief Is the window focused
     */
    Bool isFocused = true;

    /**
     * @brief Is the window maximized
     */
    Bool isMaximized = false;

    /**
     * @brief Is the window visible
     */
    Bool isVisible = true;

    /**
     * @brief Is the window floating
     */
    Bool isFloating = true;

    /**
     * @brief Number of samples for anti-aliasing
     */
    U8 samples = 4;
};
} // namespace JzRE
