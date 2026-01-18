/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>
#include <vector>
#include "JzRE/Runtime/Core/JzVector.h"
#include "JzRE/Runtime/Core/JzMatrix.h"
#include "JzRE/Runtime/Core/JzVertex.h"
#include "JzRE/Runtime/Platform/JzGPUBufferObject.h"
#include "JzRE/Runtime/Platform/JzGPUVertexArrayObject.h"
#include "JzRE/Runtime/Platform/JzGPUTextureObject.h"

namespace JzRE {

// ==================== Mesh Component ====================

/**
 * @brief Component that holds mesh geometry data and GPU resources.
 *
 * This component stores direct references to GPU resources for efficient rendering.
 * It follows EnTT's data-driven design by holding actual render data rather than
 * abstract resource handles.
 */
struct JzMeshComponent {
    // CPU-side geometry data (optional, can be cleared after GPU upload)
    std::vector<JzVertex> vertices;
    std::vector<U32>      indices;

    // GPU resources
    std::shared_ptr<JzGPUBufferObject>      vertexBuffer;
    std::shared_ptr<JzGPUBufferObject>      indexBuffer;
    std::shared_ptr<JzGPUVertexArrayObject> vertexArray;

    // Mesh metadata
    U32  indexCount{0};
    I32  materialIndex{-1};
    Bool isGPUReady{false};

    JzMeshComponent() = default;

    JzMeshComponent(std::vector<JzVertex> verts, std::vector<U32> inds, I32 matIdx = -1) :
        vertices(std::move(verts)),
        indices(std::move(inds)),
        indexCount(static_cast<U32>(indices.size())),
        materialIndex(matIdx) { }

    /**
     * @brief Check if the mesh has valid GPU resources
     */
    Bool HasGPUResources() const
    {
        return vertexArray != nullptr && isGPUReady;
    }

    /**
     * @brief Clear CPU-side data after GPU upload to save memory
     */
    void ClearCPUData()
    {
        vertices.clear();
        vertices.shrink_to_fit();
        indices.clear();
        indices.shrink_to_fit();
    }
};

// ==================== Transform Component ====================

/**
 * @brief Component for position, rotation, and scale with cached world matrix.
 *
 * This component stores transform data directly in a cache-friendly layout.
 * The world matrix is cached and updated lazily when dirty flag is set.
 */
struct JzTransformComponent {
    // Local transform data
    JzVec3 position{0.0f, 0.0f, 0.0f};
    JzVec3 rotation{0.0f, 0.0f, 0.0f}; // Euler angles in radians
    JzVec3 scale{1.0f, 1.0f, 1.0f};

    // Cached matrices
    JzMat4 localMatrix{JzMat4::Identity()};
    JzMat4 worldMatrix{JzMat4::Identity()};

    // Dirty flag for lazy matrix update
    Bool isDirty{true};

    JzTransformComponent() = default;

    JzTransformComponent(const JzVec3 &pos) :
        position(pos), isDirty(true) { }

    JzTransformComponent(const JzVec3 &pos, const JzVec3 &rot, const JzVec3 &scl) :
        position(pos), rotation(rot), scale(scl), isDirty(true) { }

    /**
     * @brief Mark transform as dirty (needs matrix recalculation)
     */
    void SetDirty()
    {
        isDirty = true;
    }

    /**
     * @brief Update local matrix from position, rotation, scale
     */
    void UpdateLocalMatrix()
    {
        if (!isDirty) return;

        // Compute TRS matrix: Translation * RotationZ * RotationY * RotationX * Scale
        JzMat4 T = JzMat4::Translate(position);
        JzMat4 R = JzMat4::RotateZ(rotation.z) * JzMat4::RotateY(rotation.y) * JzMat4::RotateX(rotation.x);
        JzMat4 S = JzMat4::Scale(scale);

        localMatrix = T * R * S;
        worldMatrix = localMatrix; // No parent hierarchy for now
        isDirty     = false;
    }

    /**
     * @brief Get the world matrix, updating if dirty
     */
    const JzMat4 &GetWorldMatrix()
    {
        if (isDirty) {
            UpdateLocalMatrix();
        }
        return worldMatrix;
    }
};

// ==================== Material Component ====================

/**
 * @brief Component that holds material properties and textures for rendering.
 *
 * This component stores PBR material properties directly for cache-friendly access.
 * Textures are stored as GPU texture object references.
 */
struct JzMaterialComponent {
    // PBR base properties
    JzVec4 baseColor{1.0f, 1.0f, 1.0f, 1.0f};
    F32    roughness{0.5f};
    F32    metallic{0.0f};
    F32    ao{1.0f}; // Ambient occlusion

    // Legacy Phong properties (for compatibility)
    JzVec3 ambientColor{0.1f, 0.1f, 0.1f};
    JzVec3 diffuseColor{0.8f, 0.8f, 0.8f};
    JzVec3 specularColor{0.5f, 0.5f, 0.5f};
    F32    shininess{32.0f};
    F32    opacity{1.0f};

    // Texture slots
    std::shared_ptr<JzGPUTextureObject> albedoTexture;
    std::shared_ptr<JzGPUTextureObject> normalTexture;
    std::shared_ptr<JzGPUTextureObject> metallicRoughnessTexture;
    std::shared_ptr<JzGPUTextureObject> aoTexture;
    std::shared_ptr<JzGPUTextureObject> emissiveTexture;

    // Texture tiling and offset
    JzVec2 textureTiling{1.0f, 1.0f};
    JzVec2 textureOffset{0.0f, 0.0f};

    // Render state
    Bool doubleSided{false};
    Bool transparent{false};

    JzMaterialComponent() = default;

    /**
     * @brief Create from legacy Phong properties
     */
    static JzMaterialComponent FromPhong(const JzVec3 &ambient, const JzVec3 &diffuse,
                                         const JzVec3 &specular, F32 shine, F32 alpha = 1.0f)
    {
        JzMaterialComponent mat;
        mat.ambientColor  = ambient;
        mat.diffuseColor  = diffuse;
        mat.specularColor = specular;
        mat.shininess     = shine;
        mat.opacity       = alpha;
        mat.baseColor     = JzVec4(diffuse.x, diffuse.y, diffuse.z, alpha);
        mat.transparent   = alpha < 1.0f;
        return mat;
    }

    /**
     * @brief Check if material has any textures
     */
    Bool HasTextures() const
    {
        return albedoTexture || normalTexture || metallicRoughnessTexture || aoTexture || emissiveTexture;
    }
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
 * @brief Component for bounds
 */
struct JzBoundsComponent {
    JzBoundingBoxComponent localBounds;
    JzBoundingBoxComponent worldBounds;
};

/**
 * @brief Component for velocity.
 */
struct JzVelocityComponent {
    JzVec3 velocity{0.0f, 0.0f, 0.0f};
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

// ==================== EnTT-Specific Components ====================

/**
 * @brief Tag component to mark an entity as active.
 *
 * Tag components are empty structs used to categorize entities without
 * storing any data. They are very efficient in EnTT.
 */
struct JzActiveTag { };

/**
 * @brief Tag component to mark an entity as static (non-moving).
 */
struct JzStaticTag { };

/**
 * @brief Tag component to mark an entity for destruction at the end of the frame.
 */
struct JzPendingDestroyTag { };

/**
 * @brief Component storing a human-readable name for an entity.
 */
struct JzNameComponent {
    String name;

    JzNameComponent() = default;

    explicit JzNameComponent(const String &n) :
        name(n) { }

    explicit JzNameComponent(String &&n) :
        name(std::move(n)) { }
};

/**
 * @brief Component storing a unique identifier (UUID) for serialization.
 */
struct JzUUIDComponent {
    U64 uuid;

    JzUUIDComponent() :
        uuid(0) { }

    explicit JzUUIDComponent(U64 id) :
        uuid(id) { }
};

} // namespace JzRE
