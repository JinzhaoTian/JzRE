/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUResource.h"

namespace JzRE {

/**
 * @brief Enums of GPU buffer object types
 */
enum class JzEGPUBufferObjectType : U8 {
    Vertex,
    Index,
    Uniform,
    Storage
};

/**
 * @brief Enums of GPU buffer usages
 */
enum class JzEGPUBufferObjectUsage : U8 {
    StaticDraw,
    DynamicDraw,
    StreamDraw
};

/**
 * @brief GPU buffer object description
 */
struct JzGPUBufferObjectDesc {
    JzEGPUBufferObjectType  type;
    JzEGPUBufferObjectUsage usage;
    Size                    size;
    const void             *data = nullptr;
    String                  debugName;
};

/**
 * @brief Interface of GPU buffer object, represents a block of binary data of any type stored on the GPU.
 */
class JzGPUBufferObject : public JzGPUResource {
public:
    /**
     * @brief Constructor
     *
     * @param desc The description of the GPU buffer object
     */
    JzGPUBufferObject(const JzGPUBufferObjectDesc &desc) :
        JzGPUResource(desc.debugName), desc(desc) { }

    /**
     * @brief Destructor
     */
    virtual ~JzGPUBufferObject() = default;

    /**
     * @brief Update the data
     *
     * @param data The data to update
     * @param size The size of the data
     * @param offset The offset of the data
     */
    virtual void UpdateData(const void *data, Size size, Size offset = 0) = 0;

    /**
     * @brief Map the buffer
     *
     * @return The mapped data
     */
    virtual void *MapBuffer() = 0;

    /**
     * @brief Unmap the buffer
     */
    virtual void UnmapBuffer() = 0;

    /**
     * @brief Get the type of the buffer
     *
     * @return The type of the buffer
     */
    JzEGPUBufferObjectType GetType() const
    {
        return desc.type;
    }

    /**
     * @brief Get the usage of the buffer
     *
     * @return The usage of the buffer
     */
    JzEGPUBufferObjectUsage GetUsage() const
    {
        return desc.usage;
    }

    /**
     * @brief Get the size of the buffer
     *
     * @return The size of the buffer
     */
    Size GetSize() const
    {
        return desc.size;
    }

protected:
    JzGPUBufferObjectDesc desc;
};
} // namespace JzRE