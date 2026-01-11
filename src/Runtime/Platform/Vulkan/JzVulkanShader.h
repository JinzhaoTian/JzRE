/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzGPUShaderProgramObject.h"

namespace JzRE {
/**
 * @brief Vulkan Shader Implementation (Stub)
 */
class JzVulkanShader : public JzGPUShaderProgramObject {
public:
    JzVulkanShader(const JzShaderProgramDesc &desc);
    ~JzVulkanShader() override;

private:
    // VkShaderModule shaderModule;
    // VkPipelineShaderStageCreateInfo stageInfo;
};
} // namespace JzRE
