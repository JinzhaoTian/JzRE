#pragma once

#include "JzRETypes.h"
#include "JzRHIDescription.h"
#include "JzRHIETypes.h"
#include "JzRHIResource.h"

namespace JzRE {
/**
 * @brief Interface of RHI Buffer
 */
class JzRHIBuffer : public JzRHIResource {
public:
    /**
     * @brief Constructor
     * @param desc The description of the buffer
     */
    JzRHIBuffer(const JzBufferDesc &desc) :
        JzRHIResource(desc.debugName), desc(desc) { }

    /**
     * @brief Destructor
     */
    virtual ~JzRHIBuffer() = default;

    /**
     * @brief Update the data
     * @param data The data to update
     * @param size The size of the data
     * @param offset The offset of the data
     */
    virtual void UpdateData(const void *data, Size size, Size offset = 0) = 0;

    /**
     * @brief Map the buffer
     * @return The mapped data
     */
    virtual void *MapBuffer() = 0;

    /**
     * @brief Unmap the buffer
     */
    virtual void UnmapBuffer() = 0;

    /**
     * @brief Get the type of the buffer
     * @return The type of the buffer
     */
    JzEBufferType GetType() const
    {
        return desc.type;
    }

    /**
     * @brief Get the usage of the buffer
     * @return The usage of the buffer
     */
    JzEBufferUsage GetUsage() const
    {
        return desc.usage;
    }

    /**
     * @brief Get the size of the buffer
     * @return The size of the buffer
     */
    Size GetSize() const
    {
        return desc.size;
    }

protected:
    JzBufferDesc desc;
};
} // namespace JzRE