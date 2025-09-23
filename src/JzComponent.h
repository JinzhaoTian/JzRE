/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzVector.h"
#include "JzResource.h"
#include <memory>

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

/**
 * @brief Component that holds a reference to a mesh resource.
 */
struct MeshComponent {
    std::shared_ptr<JzResource> mesh;
};

/**
 * @brief Component that holds a reference to a material resource.
 */
struct MaterialComponent {
    std::shared_ptr<JzResource> material;
};

/**
 * @brief Component for camera properties.
 */
struct CameraComponent {
    JzVec3 target{0.0f, 0.0f, 0.0f};
    float  fov{45.0f};
};

} // namespace JzRE
