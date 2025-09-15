/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRETypes.h"

namespace JzRE {
/**
 * @brief Window Settings
 */
struct JzWindowSettings {
    /**
     * @brief Default value
     */
    static const I32 DefaultValue = -1;

    String title        = "JzRE";                         // Window's Title
    U16    width        = 800;                            // Window's Width
    U16    height       = 600;                            // Window's Height
    I16    x            = 0;                              // Window's X Position
    I16    y            = 0;                              // Window's Y Position
    I16    minWidth     = DefaultValue;                   // Window's Minimum Width
    I16    minHeight    = DefaultValue;                   // Window's Minimum Height
    I16    maxWidth     = DefaultValue;                   // Window's Maximum Width
    I16    maxHeight    = DefaultValue;                   // Window's Maximum Height
    Bool   isFullscreen = false;                          // Window's State of Fullscreen
    Bool   isDecorated  = true;                           // Window's State of title bar
    Bool   isResizable  = true;                           // Window's State of Resizable
    Bool   isFocused    = true;                           // Window's State of Focused
    Bool   isMaximized  = false;                          // Window's State of Maximized
    Bool   isVisible    = true;                           // Window's State of Visible
    Bool   isFloating   = false;                          // Window's State of Floating
    Bool   autoIconify  = false;                          // Window's automatically iconify
    U32    refreshRate  = JzWindowSettings::DefaultValue; // Window's Refresh Rate
    U8     samples      = 4;                              // Window's Anti-Aliasing Samples
};
} // namespace JzRE
