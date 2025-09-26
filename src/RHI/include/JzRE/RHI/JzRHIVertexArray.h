/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>
#include "JzRE/Core/JzRETypes.h"
#include "JzRE/RHI/JzRHIBuffer.h"
#include "JzRE/RHI/JzRHIResource.h"

namespace JzRE {
/**
 * @brief Interface of RHI Vertex Array Object
 */
class JzRHIVertexArray : public JzRHIResource {
public:
    /**
     * @brief Constructor
     * @param debugName The debug name of the vertex array
     */
    JzRHIVertexArray(const String &debugName = "") :
        JzRHIResource(debugName) { }

    /**
     * @brief Destructor
     */
    virtual ~JzRHIVertexArray() = default;

    /**
     * @brief Bind a vertex buffer
     * @param buffer The vertex buffer to bind
     * @param binding The binding point
     */
    virtual void BindVertexBuffer(std::shared_ptr<JzRHIBuffer> buffer, U32 binding = 0) = 0;

    /**
     * @brief Bind an index buffer
     * @param buffer The index buffer to bind
     */
    virtual void BindIndexBuffer(std::shared_ptr<JzRHIBuffer> buffer) = 0;

    /**
     * @brief Set a vertex attribute
     * @param index The index of the attribute
     * @param size The size of the attribute
     * @param stride The stride of the attribute
     * @param offset The offset of the attribute
     */
    virtual void SetVertexAttribute(U32 index, U32 size, U32 stride, U32 offset) = 0;
};

} // namespace JzRE