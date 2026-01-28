/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzVector.h"

namespace JzRE {

// ==================== Light Components ====================

/**
 * @brief Directional light component.
 *
 * Represents a light source with parallel rays (like the sun).
 */
struct JzDirectionalLightComponent {
    JzVec3 direction{0.3f, -1.0f, -0.5f};
    JzVec3 color{1.0f, 1.0f, 1.0f};
    F32    intensity  = 1.0f;
    Bool   castShadow = false;
};

/**
 * @brief Point light component.
 *
 * Represents a light source that emits light in all directions from a point.
 */
struct JzPointLightComponent {
    JzVec3 color{1.0f, 1.0f, 1.0f};
    F32    intensity = 1.0f;
    F32    range     = 10.0f;
    F32    constant  = 1.0f;
    F32    linear    = 0.09f;
    F32    quadratic = 0.032f;
};

/**
 * @brief Spot light component.
 *
 * Represents a light source that emits light in a cone shape.
 */
struct JzSpotLightComponent {
    JzVec3 direction{0.0f, -1.0f, 0.0f};
    JzVec3 color{1.0f, 1.0f, 1.0f};
    F32    intensity   = 1.0f;
    F32    range       = 10.0f;
    F32    innerCutoff = 12.5f; ///< Inner cone angle in degrees
    F32    outerCutoff = 17.5f; ///< Outer cone angle in degrees
};

} // namespace JzRE
