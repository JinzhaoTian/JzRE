#pragma once

#include "JzRHIBuffer.h"

namespace JzRE {
/**
 * @brief Vulkan Buffer Implementation (Stub)
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