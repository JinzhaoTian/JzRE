#pragma once

#include "CommonTypes.h"
#include "JzRHIBuffer.h"
#include "JzRHIVertexArray.h"

namespace JzRE {
/**
 * @brief OpenGL Implementation of RHI Vertex Array
 */
class JzOpenGLVertexArray : public JzRHIVertexArray {
public:
    /**
     * @brief Constructor
     * @param debugName The debug name of the vertex array
     */
    JzOpenGLVertexArray(const String &debugName = "");

    /**
     * @brief Destructor
     */
    ~JzOpenGLVertexArray() override;

    /**
     * @brief Bind a vertex buffer
     * @param buffer The vertex buffer to bind
     * @param binding The binding point
     */
    void BindVertexBuffer(std::shared_ptr<JzRHIBuffer> buffer, U32 binding = 0) override;

    /**
     * @brief Bind an index buffer
     * @param buffer The index buffer to bind
     */
    void BindIndexBuffer(std::shared_ptr<JzRHIBuffer> buffer) override;

    /**
     * @brief Set a vertex attribute
     * @param index The index of the vertex attribute
     * @param size The size of the vertex attribute
     * @param stride The stride of the vertex attribute
     * @param offset The offset of the vertex attribute
     */
    void SetVertexAttribute(U32 index, U32 size, U32 stride, U32 offset) override;

    /**
     * @brief Get the handle of the vertex array
     * @return The handle of the vertex array
     */
    GLuint GetHandle() const;

private:
    GLuint                                    m_handle = 0;
    std::vector<std::shared_ptr<JzRHIBuffer>> m_vertexBuffers;
    std::shared_ptr<JzRHIBuffer>              m_indexBuffer;
};
} // namespace JzRE