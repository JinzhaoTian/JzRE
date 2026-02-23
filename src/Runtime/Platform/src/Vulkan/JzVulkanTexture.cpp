/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#include "JzRE/Runtime/Platform/Vulkan/JzVulkanTexture.h"

#include <algorithm>
#include <cstring>

#include "JzRE/Runtime/Core/JzLogger.h"
#include "JzRE/Runtime/Platform/Vulkan/JzVulkanDevice.h"

namespace JzRE {

namespace {

Bool IsDepthFormat(JzETextureResourceFormat format)
{
    return format == JzETextureResourceFormat::Depth16 ||
           format == JzETextureResourceFormat::Depth24 ||
           format == JzETextureResourceFormat::Depth32F ||
           format == JzETextureResourceFormat::Depth24Stencil8;
}

U32 GetPixelSize(JzETextureResourceFormat format)
{
    switch (format) {
        case JzETextureResourceFormat::R8:
            return 1;
        case JzETextureResourceFormat::RG8:
            return 2;
        case JzETextureResourceFormat::RGB8:
            return 3;
        case JzETextureResourceFormat::RGBA8:
            return 4;
        default:
            return 0;
    }
}

} // namespace

JzVulkanTexture::JzVulkanTexture(JzVulkanDevice &device, const JzGPUTextureObjectDesc &desc) :
    JzGPUTextureObject(desc),
    m_owner(&device)
{
    if (!m_owner || m_owner->GetVkDevice() == VK_NULL_HANDLE) {
        return;
    }

    m_format = ConvertTextureFormat(desc.format);

    VkImageCreateInfo imageInfo{};
    imageInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType     = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width  = std::max<U32>(1, desc.width);
    imageInfo.extent.height = std::max<U32>(1, desc.height);
    imageInfo.extent.depth  = std::max<U32>(1, desc.depth);
    imageInfo.mipLevels     = std::max<U32>(1, desc.mipLevels);
    imageInfo.arrayLayers   = std::max<U32>(1, desc.arraySize);
    imageInfo.format        = m_format;
    imageInfo.tiling        = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples       = VK_SAMPLE_COUNT_1_BIT;

    const Bool depthFormat = IsDepthFormat(desc.format);
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                      VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                      VK_IMAGE_USAGE_SAMPLED_BIT;
    if (depthFormat) {
        imageInfo.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    } else {
        imageInfo.usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    }

    if (desc.type == JzETextureResourceType::TextureCube) {
        imageInfo.flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
        imageInfo.arrayLayers = std::max<U32>(6, imageInfo.arrayLayers);
    }

    const VkResult imageResult = vkCreateImage(m_owner->GetVkDevice(), &imageInfo, nullptr, &m_image);
    if (imageResult != VK_SUCCESS) {
        JzRE_LOG_ERROR("JzVulkanTexture: vkCreateImage failed ({})", static_cast<I32>(imageResult));
        return;
    }

    VkMemoryRequirements memoryRequirements{};
    vkGetImageMemoryRequirements(m_owner->GetVkDevice(), m_image, &memoryRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize  = memoryRequirements.size;
    allocInfo.memoryTypeIndex = m_owner->FindMemoryType(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    const VkResult allocResult = vkAllocateMemory(m_owner->GetVkDevice(), &allocInfo, nullptr, &m_memory);
    if (allocResult != VK_SUCCESS) {
        JzRE_LOG_ERROR("JzVulkanTexture: vkAllocateMemory failed ({})", static_cast<I32>(allocResult));
        vkDestroyImage(m_owner->GetVkDevice(), m_image, nullptr);
        m_image = VK_NULL_HANDLE;
        return;
    }

    const VkResult bindResult = vkBindImageMemory(m_owner->GetVkDevice(), m_image, m_memory, 0);
    if (bindResult != VK_SUCCESS) {
        JzRE_LOG_ERROR("JzVulkanTexture: vkBindImageMemory failed ({})", static_cast<I32>(bindResult));
        vkFreeMemory(m_owner->GetVkDevice(), m_memory, nullptr);
        vkDestroyImage(m_owner->GetVkDevice(), m_image, nullptr);
        m_memory = VK_NULL_HANDLE;
        m_image  = VK_NULL_HANDLE;
        return;
    }

    VkImageViewCreateInfo imageViewInfo{};
    imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewInfo.image = m_image;
    imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewInfo.format   = m_format;
    imageViewInfo.subresourceRange.baseMipLevel   = 0;
    imageViewInfo.subresourceRange.levelCount     = imageInfo.mipLevels;
    imageViewInfo.subresourceRange.baseArrayLayer = 0;
    imageViewInfo.subresourceRange.layerCount     = imageInfo.arrayLayers;
    imageViewInfo.subresourceRange.aspectMask     = GetImageAspectMask(desc.format);

    if (desc.type == JzETextureResourceType::TextureCube) {
        imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
    }

    const VkResult imageViewResult = vkCreateImageView(m_owner->GetVkDevice(), &imageViewInfo, nullptr, &m_imageView);
    if (imageViewResult != VK_SUCCESS) {
        JzRE_LOG_ERROR("JzVulkanTexture: vkCreateImageView failed ({})", static_cast<I32>(imageViewResult));
        return;
    }

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType        = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter    = VK_FILTER_LINEAR;
    samplerInfo.minFilter    = VK_FILTER_LINEAR;
    samplerInfo.mipmapMode   = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.minLod       = 0.0f;
    samplerInfo.maxLod       = static_cast<F32>(imageInfo.mipLevels);
    samplerInfo.maxAnisotropy = 1.0f;

    const VkResult samplerResult = vkCreateSampler(m_owner->GetVkDevice(), &samplerInfo, nullptr, &m_sampler);
    if (samplerResult != VK_SUCCESS) {
        JzRE_LOG_ERROR("JzVulkanTexture: vkCreateSampler failed ({})", static_cast<I32>(samplerResult));
        return;
    }

    m_layout = VK_IMAGE_LAYOUT_UNDEFINED;

    const VkImageLayout     initialLayout = depthFormat
                                                ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
                                                : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    const VkImageAspectFlags aspectMask   = GetImageAspectMask(desc.format);
    m_owner->ExecuteImmediate([this, initialLayout, aspectMask](VkCommandBuffer commandBuffer) {
        VkImageMemoryBarrier barrier{};
        barrier.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED;
        barrier.newLayout           = initialLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image               = m_image;
        barrier.subresourceRange.aspectMask     = aspectMask;
        barrier.subresourceRange.baseMipLevel   = 0;
        barrier.subresourceRange.levelCount     = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount     = 1;
        barrier.srcAccessMask                   = 0;
        barrier.dstAccessMask                   = (initialLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
                                                      ? VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
                                                      : VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(
            commandBuffer,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
            0,
            0,
            nullptr,
            0,
            nullptr,
            1,
            &barrier);
    });
    m_layout = initialLayout;

    if (desc.data != nullptr) {
        UpdateData(desc.data, 0, 0);
    }
}

JzVulkanTexture::~JzVulkanTexture()
{
    if (!m_owner || m_owner->GetVkDevice() == VK_NULL_HANDLE) {
        return;
    }

    if (m_sampler != VK_NULL_HANDLE) {
        vkDestroySampler(m_owner->GetVkDevice(), m_sampler, nullptr);
        m_sampler = VK_NULL_HANDLE;
    }

    if (m_imageView != VK_NULL_HANDLE) {
        vkDestroyImageView(m_owner->GetVkDevice(), m_imageView, nullptr);
        m_imageView = VK_NULL_HANDLE;
    }

    if (m_memory != VK_NULL_HANDLE) {
        vkFreeMemory(m_owner->GetVkDevice(), m_memory, nullptr);
        m_memory = VK_NULL_HANDLE;
    }

    if (m_image != VK_NULL_HANDLE) {
        vkDestroyImage(m_owner->GetVkDevice(), m_image, nullptr);
        m_image = VK_NULL_HANDLE;
    }
}

void JzVulkanTexture::UpdateData(const void *data, U32 mipLevel, U32 arrayIndex)
{
    (void)mipLevel;
    (void)arrayIndex;

    if (!data || !m_owner || m_owner->GetVkDevice() == VK_NULL_HANDLE || m_image == VK_NULL_HANDLE) {
        return;
    }

    const U32 pixelSize = GetPixelSize(desc.format);
    if (pixelSize == 0 || IsDepthFormat(desc.format)) {
        return;
    }

    const VkDeviceSize uploadSize = static_cast<VkDeviceSize>(desc.width) *
                                    static_cast<VkDeviceSize>(desc.height) *
                                    static_cast<VkDeviceSize>(pixelSize);
    if (uploadSize == 0) {
        return;
    }

    VkBuffer stagingBuffer = VK_NULL_HANDLE;
    VkDeviceMemory stagingMemory = VK_NULL_HANDLE;

    VkBufferCreateInfo stagingBufferInfo{};
    stagingBufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    stagingBufferInfo.size        = uploadSize;
    stagingBufferInfo.usage       = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    stagingBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(m_owner->GetVkDevice(), &stagingBufferInfo, nullptr, &stagingBuffer) != VK_SUCCESS) {
        return;
    }

    VkMemoryRequirements stagingRequirements{};
    vkGetBufferMemoryRequirements(m_owner->GetVkDevice(), stagingBuffer, &stagingRequirements);

    VkMemoryAllocateInfo stagingAlloc{};
    stagingAlloc.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    stagingAlloc.allocationSize  = stagingRequirements.size;
    stagingAlloc.memoryTypeIndex = m_owner->FindMemoryType(
        stagingRequirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    if (vkAllocateMemory(m_owner->GetVkDevice(), &stagingAlloc, nullptr, &stagingMemory) != VK_SUCCESS) {
        vkDestroyBuffer(m_owner->GetVkDevice(), stagingBuffer, nullptr);
        return;
    }

    if (vkBindBufferMemory(m_owner->GetVkDevice(), stagingBuffer, stagingMemory, 0) != VK_SUCCESS) {
        vkFreeMemory(m_owner->GetVkDevice(), stagingMemory, nullptr);
        vkDestroyBuffer(m_owner->GetVkDevice(), stagingBuffer, nullptr);
        return;
    }

    void *mapped = nullptr;
    if (vkMapMemory(m_owner->GetVkDevice(), stagingMemory, 0, uploadSize, 0, &mapped) == VK_SUCCESS) {
        std::memcpy(mapped, data, static_cast<Size>(uploadSize));
        vkUnmapMemory(m_owner->GetVkDevice(), stagingMemory);
    }

    m_owner->ExecuteImmediate([this, stagingBuffer](VkCommandBuffer commandBuffer) {
        VkImageMemoryBarrier toTransfer{};
        toTransfer.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        toTransfer.oldLayout           = m_layout;
        toTransfer.newLayout           = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        toTransfer.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        toTransfer.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        toTransfer.image               = m_image;
        toTransfer.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        toTransfer.subresourceRange.baseMipLevel   = 0;
        toTransfer.subresourceRange.levelCount     = 1;
        toTransfer.subresourceRange.baseArrayLayer = 0;
        toTransfer.subresourceRange.layerCount     = 1;
        toTransfer.srcAccessMask                   = 0;
        toTransfer.dstAccessMask                   = VK_ACCESS_TRANSFER_WRITE_BIT;

        vkCmdPipelineBarrier(
            commandBuffer,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            0,
            0,
            nullptr,
            0,
            nullptr,
            1,
            &toTransfer);

        VkBufferImageCopy region{};
        region.bufferOffset                    = 0;
        region.bufferRowLength                 = 0;
        region.bufferImageHeight               = 0;
        region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel       = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount     = 1;
        region.imageOffset                     = {0, 0, 0};
        region.imageExtent                     = {std::max<U32>(1, desc.width), std::max<U32>(1, desc.height), 1};

        vkCmdCopyBufferToImage(
            commandBuffer,
            stagingBuffer,
            m_image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region);

        VkImageMemoryBarrier toShaderRead = toTransfer;
        toShaderRead.oldLayout            = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        toShaderRead.newLayout            = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        toShaderRead.srcAccessMask        = VK_ACCESS_TRANSFER_WRITE_BIT;
        toShaderRead.dstAccessMask        = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(
            commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            0,
            0,
            nullptr,
            0,
            nullptr,
            1,
            &toShaderRead);
    });

    m_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    vkFreeMemory(m_owner->GetVkDevice(), stagingMemory, nullptr);
    vkDestroyBuffer(m_owner->GetVkDevice(), stagingBuffer, nullptr);
}

void JzVulkanTexture::GenerateMipmaps()
{
    // TODO: Full mipmap generation path is not required for the initial Vulkan integration.
}

void *JzVulkanTexture::GetTextureID() const
{
    // Keep a stable ID for UI bridge lookup.
    return const_cast<JzVulkanTexture *>(this);
}

VkFormat JzVulkanTexture::ConvertTextureFormat(JzETextureResourceFormat format)
{
    switch (format) {
        case JzETextureResourceFormat::R8:
            return VK_FORMAT_R8_UNORM;
        case JzETextureResourceFormat::RG8:
            return VK_FORMAT_R8G8_UNORM;
        case JzETextureResourceFormat::RGB8:
            return VK_FORMAT_R8G8B8_UNORM;
        case JzETextureResourceFormat::RGBA8:
            return VK_FORMAT_R8G8B8A8_UNORM;
        case JzETextureResourceFormat::R16F:
            return VK_FORMAT_R16_SFLOAT;
        case JzETextureResourceFormat::RG16F:
            return VK_FORMAT_R16G16_SFLOAT;
        case JzETextureResourceFormat::RGB16F:
            return VK_FORMAT_R16G16B16_SFLOAT;
        case JzETextureResourceFormat::RGBA16F:
            return VK_FORMAT_R16G16B16A16_SFLOAT;
        case JzETextureResourceFormat::R32F:
            return VK_FORMAT_R32_SFLOAT;
        case JzETextureResourceFormat::RG32F:
            return VK_FORMAT_R32G32_SFLOAT;
        case JzETextureResourceFormat::RGB32F:
            return VK_FORMAT_R32G32B32_SFLOAT;
        case JzETextureResourceFormat::RGBA32F:
            return VK_FORMAT_R32G32B32A32_SFLOAT;
        case JzETextureResourceFormat::Depth16:
            return VK_FORMAT_D16_UNORM;
        case JzETextureResourceFormat::Depth24:
            return VK_FORMAT_D32_SFLOAT;
        case JzETextureResourceFormat::Depth32F:
            return VK_FORMAT_D32_SFLOAT;
        case JzETextureResourceFormat::Depth24Stencil8:
            return VK_FORMAT_D24_UNORM_S8_UINT;
        case JzETextureResourceFormat::Unknown:
            break;
    }

    return VK_FORMAT_R8G8B8A8_UNORM;
}

VkImageAspectFlags JzVulkanTexture::GetImageAspectMask(JzETextureResourceFormat format)
{
    switch (format) {
        case JzETextureResourceFormat::Depth16:
        case JzETextureResourceFormat::Depth24:
        case JzETextureResourceFormat::Depth32F:
            return VK_IMAGE_ASPECT_DEPTH_BIT;
        case JzETextureResourceFormat::Depth24Stencil8:
            return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        default:
            return VK_IMAGE_ASPECT_COLOR_BIT;
    }
}

} // namespace JzRE
