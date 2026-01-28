/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Core/JzRETypes.h"

namespace JzRE {

/**
 * @brief Window creation configuration.
 *
 * Used when creating a new window via the window backend.
 */
struct JzWindowConfig {
    String title{"JzRE"};
    I32    width{1280};
    I32    height{720};
    Bool   fullscreen{false};
    Bool   vsync{true};
    Bool   resizable{true};
    Bool   decorated{true};
    Bool   floating{false};
    Bool   visible{true};
    I32    samples{4}; ///< MSAA samples
    Bool   srgb{true};
};

} // namespace JzRE
