#pragma once

#include "JzRHITexture.h"

namespace JzRE {
/**
 * @brief Vulkan Texture Implementation (Stub)
 */
class JzVulkanTexture : public JzRHITexture {
public:
    JzVulkanTexture(const JzTextureDesc &desc);
    ~JzVulkanTexture() override;

    void UpdateData(const void *data, U32 mipLevel = 0, U32 arrayIndex = 0) override;
    void GenerateMipmaps() override;
    U32  GetHandle() const override
    {
        return 0;
    } // Stub implementation

private:
    // VkImage image;
    // VkImageView imageView;
    // VkDeviceMemory memory;
    // VkSampler sampler;
};
} // namespace JzRE