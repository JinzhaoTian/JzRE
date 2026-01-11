/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>
#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Platform/JzGPUBufferObject.h"

namespace JzRE {

/**
 * @brief Interface of GPU vertex array object, represents a recorder or container for the state of a buffer object.
 */
class JzGPUVertexArrayObject : public JzGPUResource {
public:
    /**
     * @brief Constructor
     *
     * @param debugName The debug name of the GPU vertex array object
     */
    JzGPUVertexArrayObject(const String &debugName = "") :
        JzGPUResource(debugName) { }

    /**
     * @brief Destructor
     */
    virtual ~JzGPUVertexArrayObject() = default;

    /**
     * @brief Bind a vertex buffer object
     *
     * @param buffer The vertex buffer to bind
     * @param binding The binding point
     */
    virtual void BindVertexBuffer(std::shared_ptr<JzGPUBufferObject> buffer, U32 binding = 0) = 0;

    /**
     * @brief Bind an index buffer object
     *
     * @param buffer The index buffer to bind
     */
    virtual void BindIndexBuffer(std::shared_ptr<JzGPUBufferObject> buffer) = 0;

    /**
     * @brief Set a vertex attribute
     *
     * @param index The index of the attribute
     * @param size The size of the attribute
     * @param stride The stride of the attribute
     * @param offset The offset of the attribute
     */
    virtual void SetVertexAttribute(U32 index, U32 size, U32 stride, U32 offset) = 0;
};

} // namespace JzRE