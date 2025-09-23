/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzResource.h"
#include "JzRHIBuffer.h"
#include "JzRHIDevice.h"
#include "JzRHIVertexArray.h"
#include <vector>

namespace JzRE {

/**
 * @brief Represents a mesh asset, containing vertex and index data.
 *        It manages both CPU data and its corresponding GPU (RHI) resource.
 */
class JzMesh : public JzResource {
public:
    /**
     * @brief Destructor
     */
    virtual ~JzMesh() = default;

    /**
     * @brief Get the Vertex Buffer RHI Resource.
     *
     * @return std::shared_ptr<JzRHIBuffer>
     */
    std::shared_ptr<JzRHIBuffer> GetVertexBuffer() const { return m_vertexBuffer; }

    /**
     * @brief Get the Index Buffer RHI Resource.
     *
     * @return std::shared_ptr<JzRHIBuffer>
     */
    std::shared_ptr<JzRHIBuffer> GetIndexBuffer() const { return m_indexBuffer; }

    /**
     * @brief Get the Vertex Array RHI Resource.
     *
     * @return std::shared_ptr<JzRHIVertexArray>
     */
    std::shared_ptr<JzRHIVertexArray> GetVertexArray() const { return m_vertexArray; }

    /**
     * @brief Get the number of indices in the mesh.
     *
     * @return uint32_t
     */
    uint32_t GetIndexCount() const { return static_cast<uint32_t>(m_indices.size()); }

protected:
    // CPU-side data
    std::vector<float>    m_vertices;
    std::vector<uint32_t> m_indices;

    // GPU-side RHI resources
    std::shared_ptr<JzRHIBuffer>     m_vertexBuffer;
    std::shared_ptr<JzRHIBuffer>     m_indexBuffer;
    std::shared_ptr<JzRHIVertexArray> m_vertexArray;
};

} // namespace JzRE
