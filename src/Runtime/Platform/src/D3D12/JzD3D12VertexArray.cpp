/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#include "JzRE/Runtime/Platform/D3D12/JzD3D12VertexArray.h"

#if defined(_WIN32)

#include <algorithm>

namespace JzRE {

JzD3D12VertexArray::JzD3D12VertexArray(const String &debugName) :
    JzGPUVertexArrayObject(debugName)
{ }

void JzD3D12VertexArray::BindVertexBuffer(std::shared_ptr<JzGPUBufferObject> buffer, U32 binding)
{
    if (!buffer) {
        return;
    }

    m_vertexBuffers[binding] = std::move(buffer);
}

void JzD3D12VertexArray::BindIndexBuffer(std::shared_ptr<JzGPUBufferObject> buffer)
{
    m_indexBuffer = std::move(buffer);
}

void JzD3D12VertexArray::SetVertexAttribute(U32 index, U32 size, U32 stride, U32 offset)
{
    auto iter = std::find_if(
        m_attributes.begin(),
        m_attributes.end(),
        [index](const JzD3D12VertexAttribute &attr) {
            return attr.index == index;
        });

    if (iter == m_attributes.end()) {
        JzD3D12VertexAttribute attr;
        attr.index  = index;
        attr.size   = size;
        attr.stride = stride;
        attr.offset = offset;
        m_attributes.push_back(attr);
        return;
    }

    iter->size   = size;
    iter->stride = stride;
    iter->offset = offset;
}

const std::unordered_map<U32, std::shared_ptr<JzGPUBufferObject>> &JzD3D12VertexArray::GetVertexBuffers() const
{
    return m_vertexBuffers;
}

const std::shared_ptr<JzGPUBufferObject> &JzD3D12VertexArray::GetIndexBuffer() const
{
    return m_indexBuffer;
}

const std::vector<JzD3D12VertexAttribute> &JzD3D12VertexArray::GetAttributes() const
{
    return m_attributes;
}

} // namespace JzRE

#endif // _WIN32
