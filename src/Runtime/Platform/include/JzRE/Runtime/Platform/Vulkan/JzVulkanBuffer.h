/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Platform/RHI/JzGPUBufferObject.h"

namespace JzRE {
/**
 * @brief Vulkan Buffer Implementation (Stub)
 */
class JzVulkanBuffer : public JzGPUBufferObject {
public:
    /**
     * @brief Constructor
     *
     * @param desc
     */
    JzVulkanBuffer(const JzGPUBufferObjectDesc &desc);

    /**
     * @brief Destructor
     */
    ~JzVulkanBuffer() override;

    /**
     * @brief Update data
     *
     * @param data
     * @param size
     * @param offset
     */
    void UpdateData(const void *data, Size size, Size offset = 0) override;

    /**
     * @brief Map Buffer
     *
     * @return void*
     */
    void *MapBuffer() override;

    /**
     * @brief Upmap Buffer
     */
    void UnmapBuffer() override;

private:
    // VkBuffer buffer;
    // VkDeviceMemory memory;
    // VkDevice device;
};
} // namespace JzRE