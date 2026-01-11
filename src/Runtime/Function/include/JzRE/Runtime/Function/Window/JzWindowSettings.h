/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzVector.h"

namespace JzRE {
/**
 * @brief Window Settings
 */
struct JzWindowSettings {
    /**
     * @brief Default value
     */
    static const I32 DefaultValue = -1;

    String  title        = "JzRE";                         // Window's Title
    JzIVec2 size         = {800, 600};                     // Window's Size
    JzIVec2 position     = {0, 0};                         // Window's Position
    JzIVec2 minSize      = {DefaultValue, DefaultValue};   // Window's Minimum Size
    JzIVec2 maxSize      = {DefaultValue, DefaultValue};   // Window's Maximum Size
    Bool    isFullscreen = false;                          // Window's State of Fullscreen
    Bool    isDecorated  = true;                           // Window's State of title bar
    Bool    isResizable  = true;                           // Window's State of Resizable
    Bool    isFocused    = true;                           // Window's State of Focused
    Bool    isMaximized  = false;                          // Window's State of Maximized
    Bool    isVisible    = true;                           // Window's State of Visible
    Bool    isFloating   = false;                          // Window's State of Floating
    Bool    autoIconify  = false;                          // Window's automatically iconify
    U32     refreshRate  = JzWindowSettings::DefaultValue; // Window's Refresh Rate
    U8      samples      = 4;                              // Window's Anti-Aliasing Samples
};
} // namespace JzRE
