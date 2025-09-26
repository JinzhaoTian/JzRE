/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRHIShader.h"

namespace JzRE {
/**
 * @brief Vulkan Shader Implementation (Stub)
 */
class JzVulkanShader : public JzRHIShader {
public:
    JzVulkanShader(const JzShaderDesc &desc);
    ~JzVulkanShader() override;

private:
    // VkShaderModule shaderModule;
    // VkPipelineShaderStageCreateInfo stageInfo;
};
} // namespace JzRE
