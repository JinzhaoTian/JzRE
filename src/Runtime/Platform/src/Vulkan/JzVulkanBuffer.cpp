/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#include "JzRE/Runtime/Platform/Vulkan/JzVulkanBuffer.h"

#include <algorithm>
#include <cstring>

#include "JzRE/Runtime/Core/JzLogger.h"
#include "JzRE/Runtime/Platform/Vulkan/JzVulkanDevice.h"

namespace JzRE {

JzVulkanBuffer::JzVulkanBuffer(JzVulkanDevice &device, const JzGPUBufferObjectDesc &desc) :
    JzGPUBufferObject(desc),
    m_owner(&device)
{
    if (!m_owner || m_owner->GetVkDevice() == VK_NULL_HANDLE) {
        return;
    }

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size  = static_cast<VkDeviceSize>(std::max<Size>(desc.size, 1));
    bufferInfo.usage = ConvertBufferUsage(desc.type) | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    const VkResult createResult = vkCreateBuffer(m_owner->GetVkDevice(), &bufferInfo, nullptr, &m_buffer);
    if (createResult != VK_SUCCESS) {
        JzRE_LOG_ERROR("JzVulkanBuffer: vkCreateBuffer failed ({})", static_cast<I32>(createResult));
        return;
    }

    VkMemoryRequirements memoryRequirements{};
    vkGetBufferMemoryRequirements(m_owner->GetVkDevice(), m_buffer, &memoryRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize  = memoryRequirements.size;
    allocInfo.memoryTypeIndex = m_owner->FindMemoryType(
        memoryRequirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    const VkResult allocResult = vkAllocateMemory(m_owner->GetVkDevice(), &allocInfo, nullptr, &m_memory);
    if (allocResult != VK_SUCCESS) {
        JzRE_LOG_ERROR("JzVulkanBuffer: vkAllocateMemory failed ({})", static_cast<I32>(allocResult));
        vkDestroyBuffer(m_owner->GetVkDevice(), m_buffer, nullptr);
        m_buffer = VK_NULL_HANDLE;
        return;
    }

    const VkResult bindResult = vkBindBufferMemory(m_owner->GetVkDevice(), m_buffer, m_memory, 0);
    if (bindResult != VK_SUCCESS) {
        JzRE_LOG_ERROR("JzVulkanBuffer: vkBindBufferMemory failed ({})", static_cast<I32>(bindResult));
        vkFreeMemory(m_owner->GetVkDevice(), m_memory, nullptr);
        vkDestroyBuffer(m_owner->GetVkDevice(), m_buffer, nullptr);
        m_memory = VK_NULL_HANDLE;
        m_buffer = VK_NULL_HANDLE;
        return;
    }

    if (desc.data != nullptr && desc.size > 0) {
        UpdateData(desc.data, desc.size, 0);
    }
}

JzVulkanBuffer::~JzVulkanBuffer()
{
    if (!m_owner || m_owner->GetVkDevice() == VK_NULL_HANDLE) {
        return;
    }

    if (m_mapped) {
        vkUnmapMemory(m_owner->GetVkDevice(), m_memory);
        m_mapped = nullptr;
    }

    if (m_memory != VK_NULL_HANDLE) {
        vkFreeMemory(m_owner->GetVkDevice(), m_memory, nullptr);
        m_memory = VK_NULL_HANDLE;
    }

    if (m_buffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(m_owner->GetVkDevice(), m_buffer, nullptr);
        m_buffer = VK_NULL_HANDLE;
    }
}

void JzVulkanBuffer::UpdateData(const void *data, Size size, Size offset)
{
    if (!data || size == 0 || !m_owner || m_memory == VK_NULL_HANDLE) {
        return;
    }

    if (offset + size > desc.size) {
        JzRE_LOG_WARN("JzVulkanBuffer: update range exceeds buffer size (offset={}, size={}, capacity={})",
                      offset,
                      size,
                      desc.size);
        return;
    }

    void *mapped = MapBuffer();
    if (!mapped) {
        return;
    }

    std::memcpy(static_cast<U8 *>(mapped) + offset, data, size);
    UnmapBuffer();
}

void *JzVulkanBuffer::MapBuffer()
{
    if (!m_owner || m_memory == VK_NULL_HANDLE) {
        return nullptr;
    }

    if (m_mapped != nullptr) {
        return m_mapped;
    }

    const VkResult result = vkMapMemory(
        m_owner->GetVkDevice(),
        m_memory,
        0,
        VK_WHOLE_SIZE,
        0,
        &m_mapped);
    if (result != VK_SUCCESS) {
        JzRE_LOG_ERROR("JzVulkanBuffer: vkMapMemory failed ({})", static_cast<I32>(result));
        m_mapped = nullptr;
    }

    return m_mapped;
}

void JzVulkanBuffer::UnmapBuffer()
{
    if (!m_owner || !m_mapped || m_memory == VK_NULL_HANDLE) {
        return;
    }

    vkUnmapMemory(m_owner->GetVkDevice(), m_memory);
    m_mapped = nullptr;
}

VkBufferUsageFlags JzVulkanBuffer::ConvertBufferUsage(JzEGPUBufferObjectType type)
{
    switch (type) {
        case JzEGPUBufferObjectType::Vertex:
            return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        case JzEGPUBufferObjectType::Index:
            return VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        case JzEGPUBufferObjectType::Uniform:
            return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        case JzEGPUBufferObjectType::Storage:
            return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    }

    return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
}

} // namespace JzRE
