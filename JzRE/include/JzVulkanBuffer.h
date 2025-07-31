#pragma once

#include "JzRHIBuffer.h"

namespace JzRE {

/**
 * Vulkan缓冲区实现（存根）
 * 未来可扩展为完整的Vulkan实现
 */
class JzVulkanBuffer : public JzRHIBuffer {
public:
    JzVulkanBuffer(const JzBufferDesc &desc);
    ~JzVulkanBuffer() override;

    void  UpdateData(const void *data, Size size, Size offset = 0) override;
    void *MapBuffer() override;
    void  UnmapBuffer() override;

private:
    // VkBuffer buffer;
    // VkDeviceMemory memory;
    // VkDevice device;
};
} // namespace JzRE