/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRHIPipeline.h"

namespace JzRE {
/**
 * @brief Vulkan Pipeline Implementation (Stub)
 */
class JzVulkanPipeline : public JzRHIPipeline {
public:
    JzVulkanPipeline(const JzPipelineDesc &desc);
    ~JzVulkanPipeline() override;

private:
    // VkPipeline pipeline;
    // VkPipelineLayout pipelineLayout;
    // VkRenderPass renderPass;
};
} // namespace JzRE
