#pragma once

#include "JzRHIBuffer.h"
#include "JzRHIVertexArray.h"

namespace JzRE {
/**
 * @brief Vulkan Vertex Array Object Implementation (Stub)
 */
class JzVulkanVertexArray : public JzRHIVertexArray {
public:
    JzVulkanVertexArray(const String &debugName = "");
    ~JzVulkanVertexArray() override;

    void BindVertexBuffer(std::shared_ptr<JzRHIBuffer> buffer, U32 binding = 0) override;
    void BindIndexBuffer(std::shared_ptr<JzRHIBuffer> buffer) override;
    void SetVertexAttribute(U32 index, U32 size, U32 stride, U32 offset) override;

private:
    // std::vector<VkBuffer> vertexBuffers;
    // VkBuffer indexBuffer;
};
} // namespace JzRE