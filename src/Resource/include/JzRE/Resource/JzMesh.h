/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <vector>
#include "JzRE/Resource/JzResource.h"
#include "JzRE/Core/JzVertex.h"
#include "JzRE/RHI/JzRHIBuffer.h"
#include "JzRE/RHI/JzRHIVertexArray.h"

namespace JzRE {

class JzDevice; // Forward declaration

/**
 * @brief Represents a mesh asset, containing vertex and index data.
 *        It manages both CPU data and its corresponding GPU (RHI) resource.
 */
class JzMesh : public JzResource {
public:
    /**
     * @brief Constructor for file-based meshes.
     *
     * @param path File path to the mesh.
     */
    JzMesh(const String &path);

    /**
     * @brief Constructor for procedural meshes.
     *
     * @param vertices Vector of vertices.
     * @param indices Vector of indices.
     */
    JzMesh(std::vector<JzVertex> vertices, std::vector<U32> indices);

    /**
     * @brief Destructor
     */
    virtual ~JzMesh();

    /**
     * @brief Load a resource. For meshes, this is used to create GPU resources.
     *
     * @return Bool True if successful.
     */
    virtual Bool Load() override;

    /**
     * @brief Unload a resource. Releases GPU resources and clears CPU data.
     */
    virtual void Unload() override;

    /**
     * @brief Get the Vertex Array RHI Resource.
     *
     * @return std::shared_ptr<JzRHIVertexArray>
     */
    std::shared_ptr<JzRHIVertexArray> GetVertexArray() const
    {
        return m_vertexArray;
    }

    /**
     * @brief Get the number of indices in the mesh.
     *
     * @return U32
     */
    U32 GetIndexCount() const
    {
        return static_cast<U32>(m_indices.size());
    }

private:
    /**
     * @brief Creates RHI resources (buffers and vertex array) for the mesh.
     */
    void SetupMesh();

private:
    // CPU-side data
    std::vector<JzVertex> m_vertices;
    std::vector<U32>      m_indices;

    // GPU-side RHI resources
    std::shared_ptr<JzRHIBuffer>      m_vertexBuffer;
    std::shared_ptr<JzRHIBuffer>      m_indexBuffer;
    std::shared_ptr<JzRHIVertexArray> m_vertexArray;
};

} // namespace JzRE
