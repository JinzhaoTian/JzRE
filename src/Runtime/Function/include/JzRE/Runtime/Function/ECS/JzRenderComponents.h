/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>
#include <vector>
#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzVector.h"
#include "JzRE/Runtime/Core/JzVertex.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUBufferObject.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUVertexArrayObject.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUTextureObject.h"

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

// ==================== Rendering Tags ====================

/**
 * @brief Tag to mark entities as renderable.
 */
struct JzRenderableTag { };

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
};

} // namespace JzRE
