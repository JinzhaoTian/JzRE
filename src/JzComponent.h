/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzVector.h"

namespace JzRE {

/**
 * @brief Component for position, rotation, and scale.
 */
struct TransformComponent {
    JzVec3 position{0.0f, 0.0f, 0.0f};
    JzVec3 rotation{0.0f, 0.0f, 0.0f};
    JzVec3 scale{1.0f, 1.0f, 1.0f};
};

/**
 * @brief Component for velocity.
 */
struct VelocityComponent {
    JzVec3 velocity{0.0f, 0.0f, 0.0f};
};

} // namespace JzRE
