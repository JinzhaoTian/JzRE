#pragma once

#include "CommonTypes.h"
#include "JzRHIBuffer.h"
#include "JzRHIPipeline.h"
#include "JzRHITexture.h"
#include "JzRHIVertexArray.h"
#include "JzVertex.h"

namespace JzRE {
/**
 * @brief Mesh class - Platform-independent mesh using RHI
 */
class JzMesh {
public:
    /**
     * @brief Constructor
     */
    JzMesh(std::vector<JzVertex> vertices, std::vector<U32> indices, std::vector<std::shared_ptr<JzRHITexture>> textures);

    /**
     * @brief Destructor
     */
    ~JzMesh();

    /**
     * @brief Draw the mesh using RHI
     * 
     * @param pipeline The pipeline to use for rendering
     */
    void Draw(std::shared_ptr<JzRHIPipeline> pipeline);

    /**
     * @brief Setup mesh for rendering (create RHI resources)
     */
    void SetupMesh();

    /**
     * @brief Get the vertex array object
     * 
     * @return The vertex array
     */
    std::shared_ptr<JzRHIVertexArray> GetVertexArray() const
    {
        return m_vertexArray;
    }

    /**
     * @brief Get the number of indices
     * 
     * @return The index count
     */
    U32 GetIndexCount() const
    {
        return static_cast<U32>(indices.size());
    }

public:
    std::vector<JzVertex>                      vertices;
    std::vector<U32>                           indices;
    std::vector<std::shared_ptr<JzRHITexture>> textures;

private:
    std::shared_ptr<JzRHIVertexArray> m_vertexArray;
    std::shared_ptr<JzRHIBuffer>      m_vertexBuffer;
    std::shared_ptr<JzRHIBuffer>      m_indexBuffer;
    Bool                              m_isSetup = false;
};

} // namespace JzRE