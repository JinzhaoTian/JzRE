/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzVector.h"

namespace JzRE {

// ==================== Bounding Components ====================

/**
 * @brief Component for bounding box
 */
struct JzBoundingBoxComponent {
    JzVec3 min;
    JzVec3 max;
};

/**
 * @brief Component for bounding sphere
 */
struct JzBoundingSphereComponent {
    JzVec3 center;
    F32    radius;
};

/**
 * @brief Component for bounds
 */
struct JzBoundsComponent {
    JzBoundingBoxComponent localBounds;
    JzBoundingBoxComponent worldBounds;
};

// ==================== Spatial Components ====================

/**
 * @brief Component for spatial partition
 */
struct JzSpatialComponent {
    JzVec3                 position;
    JzBoundingBoxComponent bounds;
    I32                    gridCellX, gridCellY, gridCellZ;
};

/**
 * @brief Component for streaming load
 */
struct JzStreamingComponent {
    String sceneSection;
    I32    priority;
    F32    distanceToPlayer;
    Bool   isLoaded;
    Bool   isRequired;
};

} // namespace JzRE
