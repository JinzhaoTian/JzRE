/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#include "JzRE/Runtime/Platform/Vulkan/JzVulkanVertexArray.h"

#include <algorithm>

namespace JzRE {

JzVulkanVertexArray::JzVulkanVertexArray(const String &debugName) :
    JzGPUVertexArrayObject(debugName)
{ }

JzVulkanVertexArray::~JzVulkanVertexArray() = default;

void JzVulkanVertexArray::BindVertexBuffer(std::shared_ptr<JzGPUBufferObject> buffer, U32 binding)
{
    if (!buffer) {
        return;
    }

    m_vertexBuffers[binding] = std::move(buffer);
}

void JzVulkanVertexArray::BindIndexBuffer(std::shared_ptr<JzGPUBufferObject> buffer)
{
    m_indexBuffer = std::move(buffer);
}

void JzVulkanVertexArray::SetVertexAttribute(U32 index, U32 size, U32 stride, U32 offset)
{
    auto iter = std::find_if(
        m_attributes.begin(),
        m_attributes.end(),
        [index](const JzVulkanVertexAttribute &attribute) {
            return attribute.index == index;
        });

    if (iter == m_attributes.end()) {
        JzVulkanVertexAttribute attribute;
        attribute.index  = index;
        attribute.size   = size;
        attribute.stride = stride;
        attribute.offset = offset;
        m_attributes.push_back(attribute);
        return;
    }

    iter->size   = size;
    iter->stride = stride;
    iter->offset = offset;
}

} // namespace JzRE
