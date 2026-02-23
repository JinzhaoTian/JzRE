/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#pragma once

#include <vulkan/vulkan.h>

#include "JzRE/Runtime/Platform/RHI/JzGPUTextureObject.h"

namespace JzRE {

class JzVulkanDevice;

/**
 * @brief Vulkan implementation of GPU texture object.
 */
class JzVulkanTexture : public JzGPUTextureObject {
public:
    /**
     * @brief Constructor.
     *
     * @param device Vulkan device owner.
     * @param desc Texture description.
     */
    JzVulkanTexture(JzVulkanDevice &device, const JzGPUTextureObjectDesc &desc);

    /**
     * @brief Destructor.
     */
    ~JzVulkanTexture() override;

    void UpdateData(const void *data, U32 mipLevel = 0, U32 arrayIndex = 0) override;
    void GenerateMipmaps() override;
    void *GetTextureID() const override;

    /**
     * @brief Native Vulkan image.
     */
    VkImage GetImage() const
    {
        return m_image;
    }

    /**
     * @brief Native Vulkan image view.
     */
    VkImageView GetImageView() const
    {
        return m_imageView;
    }

    /**
     * @brief Native Vulkan sampler.
     */
    VkSampler GetSampler() const
    {
        return m_sampler;
    }

    /**
     * @brief Native Vulkan image format.
     */
    VkFormat GetVkFormat() const
    {
        return m_format;
    }

    /**
     * @brief Current image layout tracked by this texture object.
     */
    VkImageLayout GetLayout() const
    {
        return m_layout;
    }

    /**
     * @brief Set tracked image layout.
     */
    void SetLayout(VkImageLayout layout)
    {
        m_layout = layout;
    }

private:
    static VkFormat ConvertTextureFormat(JzETextureResourceFormat format);
    static VkImageAspectFlags GetImageAspectMask(JzETextureResourceFormat format);

private:
    JzVulkanDevice *m_owner     = nullptr;
    VkImage         m_image     = VK_NULL_HANDLE;
    VkDeviceMemory  m_memory    = VK_NULL_HANDLE;
    VkImageView     m_imageView = VK_NULL_HANDLE;
    VkSampler       m_sampler   = VK_NULL_HANDLE;
    VkFormat        m_format    = VK_FORMAT_UNDEFINED;
    VkImageLayout   m_layout    = VK_IMAGE_LAYOUT_UNDEFINED;
};

} // namespace JzRE
