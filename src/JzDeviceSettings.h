#pragma once

#include "JzRETypes.h"

namespace JzRE {
/**
 * @brief Device Settings
 */
struct JzDeviceSettings {
    Bool debug               = false; // Debug context
    U8   contextVersionMajor = 3;     // Context version major
    U8   contextVersionMinor = 3;     // Context version minor
    Bool forwardCompatible   = true;  // Forward compatible
    U8   samples             = 4;     // Number of samples
};
} // namespace JzRE