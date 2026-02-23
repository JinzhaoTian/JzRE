/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#include "JzRE/Runtime/Platform/Vulkan/JzVulkanPipeline.h"

#include "JzRE/Runtime/Core/JzLogger.h"
#include "JzRE/Runtime/Platform/Vulkan/JzVulkanDevice.h"
#include "JzRE/Runtime/Platform/Vulkan/JzVulkanShader.h"

namespace JzRE {

JzVulkanPipeline::JzVulkanPipeline(JzVulkanDevice &device, const JzPipelineDesc &desc) :
    JzRHIPipeline(desc),
    m_owner(&device)
{
    m_isValid = true;

    for (const auto &shaderDesc : desc.shaders) {
        auto shader = std::make_shared<JzVulkanShader>(device, shaderDesc);
        if (!shader->IsCompiled()) {
            JzRE_LOG_ERROR("JzVulkanPipeline: failed to compile shader stage '{}' for pipeline '{}': {}",
                           static_cast<I32>(shaderDesc.type),
                           desc.debugName,
                           shader->GetCompileLog());
            m_isValid = false;
            continue;
        }

        m_shaders.push_back(std::move(shader));
    }

    if (m_shaders.empty()) {
        m_isValid = false;
    }
}

JzVulkanPipeline::~JzVulkanPipeline() = default;

void JzVulkanPipeline::CommitParameters()
{
    if (!HasDirtyParameters()) {
        return;
    }

    // Vulkan backend uses descriptor/uniform updates at draw submission points.
    // Parameter cache and dirty state are preserved here for API compatibility.
    MarkParametersCommitted();
}

} // namespace JzRE
