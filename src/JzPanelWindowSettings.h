/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRETypes.h"

namespace JzRE {
/**
 * @brief Settings of Panel Window
 */
struct JzPanelWindowSettings {
    Bool closable                 = false;
    Bool resizable                = true;
    Bool movable                  = true;
    Bool dockable                 = false;
    Bool scrollable               = true;
    Bool hideBackground           = false;
    Bool forceHorizontalScrollbar = false;
    Bool forceVerticalScrollbar   = false;
    Bool allowHorizontalScrollbar = false;
    Bool bringToFrontOnFocus      = true;
    Bool collapsable              = false;
    Bool allowInputs              = true;
    Bool titleBar                 = true;
    Bool autoSize                 = false;
};
} // namespace JzRE