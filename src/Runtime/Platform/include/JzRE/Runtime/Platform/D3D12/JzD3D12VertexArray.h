/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#pragma once

#include <unordered_map>
#include <vector>

#include "JzRE/Runtime/Platform/RHI/JzGPUVertexArrayObject.h"

namespace JzRE {

#if defined(_WIN32)

/**
 * @brief D3D12 vertex attribute snapshot.
 */
struct JzD3D12VertexAttribute {
    U32 index  = 0;
    U32 size   = 0;
    U32 stride = 0;
    U32 offset = 0;
};

/**
 * @brief Direct3D 12 implementation of vertex array object.
 */
class JzD3D12VertexArray final : public JzGPUVertexArrayObject {
public:
    explicit JzD3D12VertexArray(const String &debugName = "");
    ~JzD3D12VertexArray() override = default;

    void BindVertexBuffer(std::shared_ptr<JzGPUBufferObject> buffer, U32 binding = 0) override;
    void BindIndexBuffer(std::shared_ptr<JzGPUBufferObject> buffer) override;
    void SetVertexAttribute(U32 index, U32 size, U32 stride, U32 offset) override;

    const std::unordered_map<U32, std::shared_ptr<JzGPUBufferObject>> &GetVertexBuffers() const;
    const std::shared_ptr<JzGPUBufferObject>                          &GetIndexBuffer() const;
    const std::vector<JzD3D12VertexAttribute>                         &GetAttributes() const;

private:
    std::unordered_map<U32, std::shared_ptr<JzGPUBufferObject>> m_vertexBuffers;
    std::shared_ptr<JzGPUBufferObject>                          m_indexBuffer;
    std::vector<JzD3D12VertexAttribute>                         m_attributes;
};

#endif // _WIN32

} // namespace JzRE
