/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzGPUTextureObject.h"

namespace JzRE {
/**
 * @brief Vulkan Texture Implementation (Stub)
 */
class JzVulkanTexture : public JzGPUTextureObject {
public:
    JzVulkanTexture(const JzGPUTextureObjectDesc &desc);
    ~JzVulkanTexture() override;

    void  UpdateData(const void *data, U32 mipLevel = 0, U32 arrayIndex = 0) override;
    void  GenerateMipmaps() override;
    void *GetTextureID() const override
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