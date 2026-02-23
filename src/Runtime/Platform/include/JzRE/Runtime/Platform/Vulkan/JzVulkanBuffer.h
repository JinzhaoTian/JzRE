/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#pragma once

#include <vulkan/vulkan.h>

#include "JzRE/Runtime/Platform/RHI/JzGPUBufferObject.h"

namespace JzRE {

class JzVulkanDevice;

/**
 * @brief Vulkan implementation of GPU buffer object.
 */
class JzVulkanBuffer : public JzGPUBufferObject {
public:
    /**
     * @brief Constructor.
     *
     * @param device Vulkan device owner.
     * @param desc Buffer description.
     */
    JzVulkanBuffer(JzVulkanDevice &device, const JzGPUBufferObjectDesc &desc);

    /**
     * @brief Destructor.
     */
    ~JzVulkanBuffer() override;

    void UpdateData(const void *data, Size size, Size offset = 0) override;
    void *MapBuffer() override;
    void  UnmapBuffer() override;

    /**
     * @brief Native Vulkan buffer handle.
     */
    VkBuffer GetBuffer() const
    {
        return m_buffer;
    }

private:
    static VkBufferUsageFlags ConvertBufferUsage(JzEGPUBufferObjectType type);

private:
    JzVulkanDevice *m_owner  = nullptr;
    VkBuffer        m_buffer = VK_NULL_HANDLE;
    VkDeviceMemory  m_memory = VK_NULL_HANDLE;
    void           *m_mapped = nullptr;
};

} // namespace JzRE
