/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>
#include <vector>
#include "JzRE/Runtime/Core/JzVector.h"
#include "JzRE/Runtime/Core/JzMatrix.h"
#include "JzRE/Runtime/Function/ECS/JzEntity.h"
#include "JzRE/Runtime/Resource/JzResource.h"

namespace JzRE {

/**
 * @brief Component for position, rotation, and scale.
 */
struct JzTransformComponent {
    JzVec3 position{0.0f, 0.0f, 0.0f};
    JzVec3 rotation{0.0f, 0.0f, 0.0f};
    JzVec3 scale{1.0f, 1.0f, 1.0f};
};

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
 * @brief Component for hierarchy
 */
struct JzHierarchyComponent {
    JzEntity              parent;
    std::vector<JzEntity> children;
    Bool                  transformDirty;
};

/**
 * @brief Component for bounds
 */
struct JzBoundsComponent {
    JzBoundingBoxComponent localBounds;
    JzBoundingBoxComponent worldBounds;
};

/**
 * @brief Component for scene node
 */
struct JzSceneNodeComponent {
    JzEntity              parent;
    std::vector<JzEntity> children;
    JzMat4                localTransform;
    JzMat4                worldTransform;
    Bool                  isStatic;
};

/**
 * @brief Component for scene
 */
struct JzSceneComponent {
    JzEntity               rootNode;
    String                 sceneName;
    JzBoundingBoxComponent worldBounds;
};

/**
 * @brief Component for velocity.
 */
struct JzVelocityComponent {
    JzVec3 velocity{0.0f, 0.0f, 0.0f};
};

/**
 * @brief Component that holds a reference to a mesh resource.
 */
struct JzMeshComponent {
    std::shared_ptr<JzResource> mesh;
};

/**
 * @brief Component that holds a reference to a material resource.
 */
struct JzMaterialComponent {
    std::shared_ptr<JzResource> material;
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

/**
 * @brief Component for spatial partition
 */
struct JzSpatialComponent {
    JzVec3                 position;
    JzBoundingBoxComponent bounds;
    I32                    gridCellX, gridCellY, gridCellZ;
};

/**
 * @brief Component for camera properties.
 */
struct JzCameraComponent {
    JzVec3 target{0.0f, 0.0f, 0.0f};
    F32    fov{45.0f};
};

/**
 * @brief Component for skybox
 */
struct JzSkyboxComponent { };

/**
 * @brief Component for grid
 */
struct JzGridComponent {
    F32    size             = 100.0f;
    F32    majorGridSpacing = 10.0f;
    F32    minorGridSpacing = 1.0f;
    JzVec3 center           = {0.0f, 0.0f, 0.0f};
    JzVec4 majorColor       = {0.5f, 0.5f, 0.5f, 1.0f};
    JzVec4 minorColor       = {0.3f, 0.3f, 0.3f, 1.0f};
    // ViewType  visibleIn        = ViewType::Scene;
};

/**
 * @brief Component for gizmo component
 */
struct JzGizmoComponent {
    enum class Type { Transform,
                      Light,
                      Camera,
                      Collider };
    Type type;
    Bool isSelected = false;
    // ViewType visibleIn  = ViewType::Scene;
};

} // namespace JzRE
