#pragma once

#include "JzRHIShader.h"

namespace JzRE {
/**
 * Vulkan着色器实现（存根）
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
