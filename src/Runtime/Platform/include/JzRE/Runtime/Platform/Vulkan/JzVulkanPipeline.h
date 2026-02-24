/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include <vulkan/vulkan.h>

#include "JzRE/Runtime/Platform/RHI/JzRHIPipeline.h"

namespace JzRE {

class JzVulkanDevice;
class JzVulkanBuffer;
class JzVulkanShader;
class JzVulkanTexture;

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

    /**
     * @brief Native Vulkan pipeline handle.
     */
    VkPipeline GetPipeline() const
    {
        return m_pipeline;
    }

    /**
     * @brief Native Vulkan pipeline layout handle.
     */
    VkPipelineLayout GetPipelineLayout() const
    {
        return m_pipelineLayout;
    }

    /**
     * @brief Upload parameters and bind descriptor sets for the current draw.
     *
     * @param commandBuffer Target command buffer.
     * @param boundTextures Bound textures keyed by texture slot.
     */
    void BindResources(VkCommandBuffer                                                  commandBuffer,
                       const std::unordered_map<U32, std::shared_ptr<JzVulkanTexture>> &boundTextures);

private:
    struct JzUniformMemberDesc {
        U32 offset = 0;
        U32 size   = 0;
    };

    struct JzUniformBindingDesc {
        U32                                             set     = 0;
        U32                                             binding = 0;
        U32                                             size    = 0;
        std::unordered_map<String, JzUniformMemberDesc> members;
        std::shared_ptr<JzVulkanBuffer>                 buffer;
        std::vector<U8>                                 cpuData;
    };

    struct JzSamplerBindingDesc {
        U32              set            = 0;
        U32              binding        = 0;
        VkDescriptorType descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        String           name;
    };

    Bool CreateGraphicsPipeline();
    void DestroyDescriptorResources();
    void DestroyDescriptorSetLayouts();
    void UploadUniformParameters();
    void UpdateSamplerDescriptors(const std::unordered_map<U32, std::shared_ptr<JzVulkanTexture>> &boundTextures);
    void BindDescriptorSets(VkCommandBuffer commandBuffer);

private:
    JzVulkanDevice                              *m_owner   = nullptr;
    Bool                                         m_isValid = false;
    std::vector<std::shared_ptr<JzVulkanShader>> m_shaders;
    VkPipelineLayout                             m_pipelineLayout = VK_NULL_HANDLE;
    VkPipeline                                   m_pipeline       = VK_NULL_HANDLE;
    std::vector<VkDescriptorSetLayout>           m_descriptorSetLayouts;
    VkDescriptorPool                             m_descriptorPool = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet>                 m_descriptorSets;
    std::vector<JzUniformBindingDesc>            m_uniformBindings;
    std::vector<JzSamplerBindingDesc>            m_samplerBindings;
};

} // namespace JzRE
