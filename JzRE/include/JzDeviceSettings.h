#pragma once

#include "CommonTypes.h"

namespace JzRE {
/**
 * @brief Device settings
 */
struct JzDeviceSettings {
    /**
     * @brief Context version major
     */
    U8 contextVersionMajor = 4;

    /**
     * @brief Context version minor
     */
    U8 contextVersionMinor = 6;

    /**
     * @brief Forward compatible
     */
    Bool forwardCompatible = true;

    /**
     * @brief Number of samples
     */
    U8 samples = 4;
};
} // namespace JzRE