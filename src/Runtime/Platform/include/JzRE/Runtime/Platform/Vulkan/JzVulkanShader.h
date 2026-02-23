/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#pragma once

#include <atomic>
#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

#include "JzRE/Runtime/Platform/RHI/JzGPUShaderProgramObject.h"

namespace JzRE {

class JzVulkanDevice;

/**
 * @brief Vulkan shader object compiled from GLSL source.
 */
class JzVulkanShader : public JzGPUShaderProgramObject {
public:
    /**
     * @brief Constructor.
     *
     * @param device Vulkan device owner.
     * @param desc Shader descriptor.
     */
    JzVulkanShader(JzVulkanDevice &device, const JzShaderProgramDesc &desc);

    /**
     * @brief Destructor.
     */
    ~JzVulkanShader() override;

    /**
     * @brief Shader module handle.
     */
    VkShaderModule GetModule() const
    {
        return m_shaderModule;
    }

    /**
     * @brief Shader stage flag.
     */
    VkShaderStageFlagBits GetStage() const
    {
        return m_stage;
    }

    /**
     * @brief Check compile status.
     */
    Bool IsCompiled() const
    {
        return m_isCompiled;
    }

    /**
     * @brief Compile log.
     */
    const String &GetCompileLog() const
    {
        return m_compileLog;
    }

    /**
     * @brief SPIR-V bytecode generated from source.
     */
    const std::vector<U32> &GetSpirv() const
    {
        return m_spirv;
    }

private:
    Bool Compile();

    static VkShaderStageFlagBits ConvertStage(JzEShaderProgramType type);

private:
    VkDevice                           m_device       = VK_NULL_HANDLE;
    std::shared_ptr<std::atomic_bool>  m_deviceAlive;
    VkShaderModule                     m_shaderModule = VK_NULL_HANDLE;
    VkShaderStageFlagBits              m_stage        = VK_SHADER_STAGE_VERTEX_BIT;
    Bool                               m_isCompiled   = false;
    String                             m_compileLog;
    std::vector<U32>                   m_spirv;
};

} // namespace JzRE
