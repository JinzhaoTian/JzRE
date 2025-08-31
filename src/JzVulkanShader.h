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
