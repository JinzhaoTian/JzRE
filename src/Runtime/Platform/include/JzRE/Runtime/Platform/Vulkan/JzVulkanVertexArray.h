/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "JzRE/Runtime/Platform/RHI/JzGPUVertexArrayObject.h"

namespace JzRE {

/**
 * @brief Vertex attribute metadata used by Vulkan pipeline creation.
 */
struct JzVulkanVertexAttribute {
    U32 index   = 0;
    U32 size    = 0;
    U32 stride  = 0;
    U32 offset  = 0;
    U32 binding = 0;
};

/**
 * @brief Vulkan implementation of vertex array abstraction.
 */
class JzVulkanVertexArray : public JzGPUVertexArrayObject {
public:
    explicit JzVulkanVertexArray(const String &debugName = "");
    ~JzVulkanVertexArray() override;

    void BindVertexBuffer(std::shared_ptr<JzGPUBufferObject> buffer, U32 binding = 0) override;
    void BindIndexBuffer(std::shared_ptr<JzGPUBufferObject> buffer) override;
    void SetVertexAttribute(U32 index, U32 size, U32 stride, U32 offset) override;

    /**
     * @brief Bound vertex buffer map.
     */
    const std::unordered_map<U32, std::shared_ptr<JzGPUBufferObject>> &GetVertexBuffers() const
    {
        return m_vertexBuffers;
    }

    /**
     * @brief Bound index buffer.
     */
    std::shared_ptr<JzGPUBufferObject> GetIndexBuffer() const
    {
        return m_indexBuffer;
    }

    /**
     * @brief Vertex attributes metadata.
     */
    const std::vector<JzVulkanVertexAttribute> &GetVertexAttributes() const
    {
        return m_attributes;
    }

private:
    std::unordered_map<U32, std::shared_ptr<JzGPUBufferObject>> m_vertexBuffers;
    std::shared_ptr<JzGPUBufferObject>                          m_indexBuffer;
    std::vector<JzVulkanVertexAttribute>                        m_attributes;
};

} // namespace JzRE
