/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#pragma once

#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

#include "JzRE/Runtime/Platform/RHI/JzRHIPipeline.h"

namespace JzRE {

class JzVulkanDevice;
class JzVulkanShader;

/**
 * @brief Vulkan pipeline abstraction.
 *
 * This class keeps shader modules and parameter cache for Vulkan backend.
 * Full graphics pipeline state is progressively built in the device path.
 */
class JzVulkanPipeline : public JzRHIPipeline {
public:
    /**
     * @brief Constructor.
     *
     * @param device Vulkan device owner.
     * @param desc Pipeline description.
     */
    JzVulkanPipeline(JzVulkanDevice &device, const JzPipelineDesc &desc);

    /**
     * @brief Destructor.
     */
    ~JzVulkanPipeline() override;

    /**
     * @brief Commit cached parameters for this pipeline.
     */
    void CommitParameters() override;

    /**
     * @brief Check if all shader stages are compiled.
     */
    Bool IsValid() const
    {
        return m_isValid;
    }

    /**
     * @brief Shader list used by this pipeline.
     */
    const std::vector<std::shared_ptr<JzVulkanShader>> &GetShaders() const
    {
        return m_shaders;
    }

private:
    JzVulkanDevice                                  *m_owner  = nullptr;
    Bool                                             m_isValid = false;
    std::vector<std::shared_ptr<JzVulkanShader>>     m_shaders;
};

} // namespace JzRE
