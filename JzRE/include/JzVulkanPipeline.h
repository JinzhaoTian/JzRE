#pragma once

#include "JzRHIPipeline.h"

namespace JzRE {
/**
 * Vulkan渲染管线实现（存根）
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
