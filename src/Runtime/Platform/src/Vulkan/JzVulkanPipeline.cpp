/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#include "JzRE/Runtime/Platform/Vulkan/JzVulkanPipeline.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include <spirv_reflect.h>

#include "JzRE/Runtime/Core/JzLogger.h"
#include "JzRE/Runtime/Platform/Vulkan/JzVulkanBuffer.h"
#include "JzRE/Runtime/Platform/Vulkan/JzVulkanDevice.h"
#include "JzRE/Runtime/Platform/Vulkan/JzVulkanShader.h"
#include "JzRE/Runtime/Platform/Vulkan/JzVulkanTexture.h"

namespace JzRE {

namespace {

struct JzReflectedUniformMember {
    U32 offset = 0;
    U32 size   = 0;
};

struct JzReflectedDescriptorBinding {
    VkDescriptorSetLayoutBinding                     layoutBinding{};
    String                                           name;
    U32                                              blockSize = 0;
    std::unordered_map<String, JzReflectedUniformMember> members;
};

struct JzReflectedVertexInput {
    U32      location = 0;
    VkFormat format   = VK_FORMAT_UNDEFINED;
    U32      size     = 0;
};

VkCullModeFlags ConvertCullMode(JzECullMode mode)
{
    switch (mode) {
        case JzECullMode::None:
            return VK_CULL_MODE_NONE;
        case JzECullMode::Front:
            return VK_CULL_MODE_FRONT_BIT;
        case JzECullMode::Back:
            return VK_CULL_MODE_BACK_BIT;
        case JzECullMode::FrontAndBack:
            return VK_CULL_MODE_FRONT_AND_BACK;
    }

    return VK_CULL_MODE_BACK_BIT;
}

void ConfigureBlend(JzEBlendMode mode, VkPipelineColorBlendAttachmentState &attachment)
{
    attachment.blendEnable         = VK_FALSE;
    attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    attachment.colorBlendOp        = VK_BLEND_OP_ADD;
    attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    attachment.alphaBlendOp        = VK_BLEND_OP_ADD;

    switch (mode) {
        case JzEBlendMode::None:
            break;
        case JzEBlendMode::Alpha:
            attachment.blendEnable         = VK_TRUE;
            attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
            attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            break;
        case JzEBlendMode::Additive:
            attachment.blendEnable         = VK_TRUE;
            attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
            attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
            attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            break;
        case JzEBlendMode::Multiply:
            attachment.blendEnable         = VK_TRUE;
            attachment.srcColorBlendFactor = VK_BLEND_FACTOR_DST_COLOR;
            attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
            attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
            break;
    }
}

VkDescriptorType ConvertDescriptorType(SpvReflectDescriptorType type)
{
    switch (type) {
        case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER:
            return VK_DESCRIPTOR_TYPE_SAMPLER;
        case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
            return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
            return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:
            return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
            return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
            return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
        case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
            return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:
            return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
            return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
            return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
        case SPV_REFLECT_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
            return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
        default:
            break;
    }

    return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
}

U32 ResolveDescriptorCount(const SpvReflectDescriptorBinding *binding)
{
    if (!binding) {
        return 1;
    }
    if (binding->count > 0) {
        return binding->count;
    }

    U32 descriptorCount = 1;
    for (U32 i = 0; i < binding->array.dims_count; ++i) {
        descriptorCount *= std::max<U32>(1, binding->array.dims[i]);
    }
    return std::max<U32>(1, descriptorCount);
}

Bool ConvertReflectedVertexFormat(SpvReflectFormat format, VkFormat &vkFormat, U32 &size)
{
    switch (format) {
        case SPV_REFLECT_FORMAT_R32_SFLOAT:
            vkFormat = VK_FORMAT_R32_SFLOAT;
            size     = 4;
            return true;
        case SPV_REFLECT_FORMAT_R32G32_SFLOAT:
            vkFormat = VK_FORMAT_R32G32_SFLOAT;
            size     = 8;
            return true;
        case SPV_REFLECT_FORMAT_R32G32B32_SFLOAT:
            vkFormat = VK_FORMAT_R32G32B32_SFLOAT;
            size     = 12;
            return true;
        case SPV_REFLECT_FORMAT_R32G32B32A32_SFLOAT:
            vkFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
            size     = 16;
            return true;
        case SPV_REFLECT_FORMAT_R32_SINT:
            vkFormat = VK_FORMAT_R32_SINT;
            size     = 4;
            return true;
        case SPV_REFLECT_FORMAT_R32G32_SINT:
            vkFormat = VK_FORMAT_R32G32_SINT;
            size     = 8;
            return true;
        case SPV_REFLECT_FORMAT_R32G32B32_SINT:
            vkFormat = VK_FORMAT_R32G32B32_SINT;
            size     = 12;
            return true;
        case SPV_REFLECT_FORMAT_R32G32B32A32_SINT:
            vkFormat = VK_FORMAT_R32G32B32A32_SINT;
            size     = 16;
            return true;
        case SPV_REFLECT_FORMAT_R32_UINT:
            vkFormat = VK_FORMAT_R32_UINT;
            size     = 4;
            return true;
        case SPV_REFLECT_FORMAT_R32G32_UINT:
            vkFormat = VK_FORMAT_R32G32_UINT;
            size     = 8;
            return true;
        case SPV_REFLECT_FORMAT_R32G32B32_UINT:
            vkFormat = VK_FORMAT_R32G32B32_UINT;
            size     = 12;
            return true;
        case SPV_REFLECT_FORMAT_R32G32B32A32_UINT:
            vkFormat = VK_FORMAT_R32G32B32A32_UINT;
            size     = 16;
            return true;
        default:
            break;
    }

    return false;
}

Bool ConvertVertexAttributeFormat(JzEVertexAttributeFormat format, VkFormat &vkFormat, U32 &size)
{
    switch (format) {
        case JzEVertexAttributeFormat::Float:
            vkFormat = VK_FORMAT_R32_SFLOAT;
            size     = 4;
            return true;
        case JzEVertexAttributeFormat::Float2:
            vkFormat = VK_FORMAT_R32G32_SFLOAT;
            size     = 8;
            return true;
        case JzEVertexAttributeFormat::Float3:
            vkFormat = VK_FORMAT_R32G32B32_SFLOAT;
            size     = 12;
            return true;
        case JzEVertexAttributeFormat::Float4:
            vkFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
            size     = 16;
            return true;
        case JzEVertexAttributeFormat::Int:
            vkFormat = VK_FORMAT_R32_SINT;
            size     = 4;
            return true;
        case JzEVertexAttributeFormat::Int2:
            vkFormat = VK_FORMAT_R32G32_SINT;
            size     = 8;
            return true;
        case JzEVertexAttributeFormat::Int3:
            vkFormat = VK_FORMAT_R32G32B32_SINT;
            size     = 12;
            return true;
        case JzEVertexAttributeFormat::Int4:
            vkFormat = VK_FORMAT_R32G32B32A32_SINT;
            size     = 16;
            return true;
        case JzEVertexAttributeFormat::UInt:
            vkFormat = VK_FORMAT_R32_UINT;
            size     = 4;
            return true;
        case JzEVertexAttributeFormat::UInt2:
            vkFormat = VK_FORMAT_R32G32_UINT;
            size     = 8;
            return true;
        case JzEVertexAttributeFormat::UInt3:
            vkFormat = VK_FORMAT_R32G32B32_UINT;
            size     = 12;
            return true;
        case JzEVertexAttributeFormat::UInt4:
            vkFormat = VK_FORMAT_R32G32B32A32_UINT;
            size     = 16;
            return true;
    }

    return false;
}

void CollectUniformMembers(const SpvReflectBlockVariable &member,
                           const String                  &prefix,
                           std::unordered_map<String, JzReflectedUniformMember> &outMembers)
{
    const char *memberName = member.name ? member.name : "";

    String currentName;
    if (*memberName != '\0') {
        if (prefix.empty()) {
            currentName = memberName;
        } else {
            currentName = prefix + "." + memberName;
        }
    } else {
        currentName = prefix;
    }

    if (member.member_count > 0 && member.members != nullptr) {
        for (U32 index = 0; index < member.member_count; ++index) {
            CollectUniformMembers(member.members[index], currentName, outMembers);
        }
        return;
    }

    if (currentName.empty()) {
        return;
    }

    JzReflectedUniformMember reflectedMember;
    reflectedMember.offset = member.absolute_offset;
    reflectedMember.size   = member.size > 0 ? member.size : member.padded_size;
    outMembers[currentName] = reflectedMember;
}

} // namespace

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
        return;
    }

    if (!CreateGraphicsPipeline()) {
        m_isValid = false;
    }
}

JzVulkanPipeline::~JzVulkanPipeline()
{
    if (!m_owner || m_owner->GetVkDevice() == VK_NULL_HANDLE) {
        return;
    }

    if (m_pipeline != VK_NULL_HANDLE) {
        vkDestroyPipeline(m_owner->GetVkDevice(), m_pipeline, nullptr);
        m_pipeline = VK_NULL_HANDLE;
    }

    DestroyDescriptorResources();

    if (m_pipelineLayout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(m_owner->GetVkDevice(), m_pipelineLayout, nullptr);
        m_pipelineLayout = VK_NULL_HANDLE;
    }

    DestroyDescriptorSetLayouts();
}

void JzVulkanPipeline::CommitParameters()
{
    // Deferred to BindResources() where descriptor sets and command buffer are available.
}

void JzVulkanPipeline::BindResources(
    VkCommandBuffer commandBuffer,
    const std::unordered_map<U32, std::shared_ptr<JzVulkanTexture>> &boundTextures)
{
    if (commandBuffer == VK_NULL_HANDLE || m_pipelineLayout == VK_NULL_HANDLE) {
        return;
    }

    UploadUniformParameters();
    UpdateSamplerDescriptors(boundTextures);
    BindDescriptorSets(commandBuffer);
}

void JzVulkanPipeline::UploadUniformParameters()
{
    if (m_uniformBindings.empty()) {
        if (HasDirtyParameters()) {
            MarkParametersCommitted();
        }
        return;
    }

    if (!HasDirtyParameters()) {
        return;
    }

    const auto &parameters = GetParameterCache();

    auto writeBytes = [](std::vector<U8> &buffer, U32 offset, const void *src, U32 bytes) {
        if (!src || bytes == 0 || offset >= buffer.size()) {
            return;
        }
        const U32 available = static_cast<U32>(buffer.size() - offset);
        const U32 copySize  = std::min(bytes, available);
        std::memcpy(buffer.data() + offset, src, copySize);
    };

    for (auto &uniformBinding : m_uniformBindings) {
        if (!uniformBinding.buffer) {
            continue;
        }

        if (uniformBinding.cpuData.size() != uniformBinding.size) {
            uniformBinding.cpuData.assign(uniformBinding.size, 0);
        }

        for (const auto &[parameterName, parameterValue] : parameters) {
            const auto memberIter = uniformBinding.members.find(parameterName);
            if (memberIter == uniformBinding.members.end()) {
                continue;
            }

            const auto &member = memberIter->second;

            std::visit(
                [&](const auto &typedValue) {
                    using TValue = std::decay_t<decltype(typedValue)>;

                    if constexpr (std::is_same_v<TValue, I32>) {
                        writeBytes(uniformBinding.cpuData, member.offset, &typedValue, std::min<U32>(member.size, sizeof(I32)));
                    } else if constexpr (std::is_same_v<TValue, F32>) {
                        writeBytes(uniformBinding.cpuData, member.offset, &typedValue, std::min<U32>(member.size, sizeof(F32)));
                    } else if constexpr (std::is_same_v<TValue, JzVec2>) {
                        writeBytes(uniformBinding.cpuData, member.offset, typedValue.Data(), std::min<U32>(member.size, sizeof(F32) * 2));
                    } else if constexpr (std::is_same_v<TValue, JzVec3>) {
                        writeBytes(uniformBinding.cpuData, member.offset, typedValue.Data(), std::min<U32>(member.size, sizeof(F32) * 3));
                    } else if constexpr (std::is_same_v<TValue, JzVec4>) {
                        writeBytes(uniformBinding.cpuData, member.offset, typedValue.Data(), std::min<U32>(member.size, sizeof(F32) * 4));
                    } else if constexpr (std::is_same_v<TValue, JzMat3>) {
                        const JzMat3 transposed = typedValue.Transpose();
                        if (member.size >= 48) {
                            const F32 *data = transposed.Data();
                            for (U32 column = 0; column < 3; ++column) {
                                writeBytes(uniformBinding.cpuData,
                                           member.offset + column * 16,
                                           data + column * 3,
                                           sizeof(F32) * 3);
                            }
                        } else {
                            writeBytes(uniformBinding.cpuData, member.offset, transposed.Data(), std::min<U32>(member.size, sizeof(F32) * 9));
                        }
                    } else if constexpr (std::is_same_v<TValue, JzMat4>) {
                        const JzMat4 transposed = typedValue.Transpose();
                        writeBytes(uniformBinding.cpuData, member.offset, transposed.Data(), std::min<U32>(member.size, sizeof(F32) * 16));
                    }
                },
                parameterValue);
        }

        uniformBinding.buffer->UpdateData(uniformBinding.cpuData.data(), uniformBinding.cpuData.size(), 0);
    }

    MarkParametersCommitted();
}

void JzVulkanPipeline::UpdateSamplerDescriptors(
    const std::unordered_map<U32, std::shared_ptr<JzVulkanTexture>> &boundTextures)
{
    if (!m_owner || m_owner->GetVkDevice() == VK_NULL_HANDLE || m_descriptorSets.empty() || m_samplerBindings.empty()) {
        return;
    }

    const auto &parameters      = GetParameterCache();
    auto        fallbackTexture = m_owner->GetFallbackTexture();

    for (const auto &samplerBinding : m_samplerBindings) {
        if (samplerBinding.set >= m_descriptorSets.size()) {
            continue;
        }

        U32 slot = 0;
        const auto slotIter = parameters.find(samplerBinding.name);
        if (slotIter != parameters.end()) {
            std::visit(
                [&](const auto &typedValue) {
                    using TValue = std::decay_t<decltype(typedValue)>;
                    if constexpr (std::is_same_v<TValue, I32>) {
                        slot = typedValue < 0 ? 0 : static_cast<U32>(typedValue);
                    } else if constexpr (std::is_same_v<TValue, F32>) {
                        slot = typedValue < 0.0f ? 0 : static_cast<U32>(typedValue);
                    }
                },
                slotIter->second);
        }

        std::shared_ptr<JzVulkanTexture> texture;
        const auto boundIter = boundTextures.find(slot);
        if (boundIter != boundTextures.end() && boundIter->second) {
            texture = boundIter->second;
        } else {
            texture = fallbackTexture;
        }

        if (!texture ||
            texture->GetImageView() == VK_NULL_HANDLE ||
            texture->GetSampler() == VK_NULL_HANDLE) {
            continue;
        }

        VkDescriptorImageInfo imageInfo{};
        imageInfo.sampler     = texture->GetSampler();
        imageInfo.imageView   = texture->GetImageView();
        imageInfo.imageLayout = texture->GetLayout() == VK_IMAGE_LAYOUT_UNDEFINED
                                    ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                                    : texture->GetLayout();

        VkWriteDescriptorSet write{};
        write.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet          = m_descriptorSets[samplerBinding.set];
        write.dstBinding      = samplerBinding.binding;
        write.dstArrayElement = 0;
        write.descriptorCount = 1;
        write.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write.pImageInfo      = &imageInfo;

        vkUpdateDescriptorSets(m_owner->GetVkDevice(), 1, &write, 0, nullptr);
    }
}

void JzVulkanPipeline::BindDescriptorSets(VkCommandBuffer commandBuffer)
{
    if (m_descriptorSets.empty()) {
        return;
    }

    vkCmdBindDescriptorSets(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        m_pipelineLayout,
        0,
        static_cast<U32>(m_descriptorSets.size()),
        m_descriptorSets.data(),
        0,
        nullptr);
}

Bool JzVulkanPipeline::CreateGraphicsPipeline()
{
    if (!m_owner || m_owner->GetVkDevice() == VK_NULL_HANDLE || m_owner->GetSwapchainRenderPass() == VK_NULL_HANDLE) {
        return false;
    }

    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
    shaderStages.reserve(m_shaders.size());

    for (const auto &shader : m_shaders) {
        if (!shader || shader->GetModule() == VK_NULL_HANDLE) {
            continue;
        }

        VkPipelineShaderStageCreateInfo stageInfo{};
        stageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stageInfo.stage  = shader->GetStage();
        stageInfo.module = shader->GetModule();
        stageInfo.pName  = shader->GetEntryPoint().empty() ? "main" : shader->GetEntryPoint().c_str();
        shaderStages.push_back(stageInfo);
    }

    if (shaderStages.empty()) {
        return false;
    }

    DestroyDescriptorResources();
    DestroyDescriptorSetLayouts();

    std::unordered_map<U32, std::unordered_map<U32, JzReflectedDescriptorBinding>> reflectedBindings;
    std::vector<JzReflectedVertexInput>                                            reflectedVertexInputs;

    for (const auto &shader : m_shaders) {
        if (!shader) {
            continue;
        }

        const auto &spirv = shader->GetSpirv();
        if (spirv.empty()) {
            continue;
        }

        SpvReflectShaderModule module{};
        const auto reflectResult = spvReflectCreateShaderModule(
            spirv.size() * sizeof(U32),
            spirv.data(),
            &module);
        if (reflectResult != SPV_REFLECT_RESULT_SUCCESS) {
            JzRE_LOG_WARN("JzVulkanPipeline: spirv-reflect failed with {}", static_cast<I32>(reflectResult));
            continue;
        }

        U32 descriptorBindingCount = 0;
        if (spvReflectEnumerateDescriptorBindings(&module, &descriptorBindingCount, nullptr) == SPV_REFLECT_RESULT_SUCCESS && descriptorBindingCount > 0) {
            std::vector<SpvReflectDescriptorBinding *> bindings(descriptorBindingCount, nullptr);
            if (spvReflectEnumerateDescriptorBindings(&module, &descriptorBindingCount, bindings.data()) == SPV_REFLECT_RESULT_SUCCESS) {
                for (const auto *binding : bindings) {
                    if (!binding) {
                        continue;
                    }

                    auto &setMap = reflectedBindings[binding->set];
                    auto [iter, inserted] = setMap.try_emplace(binding->binding);
                    auto &reflectedBinding = iter->second;

                    if (inserted) {
                        reflectedBinding.layoutBinding.binding            = binding->binding;
                        reflectedBinding.layoutBinding.descriptorType     = ConvertDescriptorType(binding->descriptor_type);
                        reflectedBinding.layoutBinding.descriptorCount    = ResolveDescriptorCount(binding);
                        reflectedBinding.layoutBinding.stageFlags         = shader->GetStage();
                        reflectedBinding.layoutBinding.pImmutableSamplers = nullptr;
                        reflectedBinding.name                             = binding->name ? binding->name : "";
                    } else {
                        reflectedBinding.layoutBinding.stageFlags |= shader->GetStage();
                    }

                    if (reflectedBinding.layoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
                        const U32 reflectedSize = binding->block.padded_size > 0
                                                      ? binding->block.padded_size
                                                      : binding->block.size;
                        reflectedBinding.blockSize = std::max(reflectedBinding.blockSize, reflectedSize);

                        for (U32 memberIndex = 0; memberIndex < binding->block.member_count; ++memberIndex) {
                            CollectUniformMembers(binding->block.members[memberIndex], "", reflectedBinding.members);
                        }
                    }
                }
            }
        }

        if (shader->GetStage() == VK_SHADER_STAGE_VERTEX_BIT) {
            U32 inputVariableCount = 0;
            if (spvReflectEnumerateInputVariables(&module, &inputVariableCount, nullptr) == SPV_REFLECT_RESULT_SUCCESS && inputVariableCount > 0) {
                std::vector<SpvReflectInterfaceVariable *> inputVariables(inputVariableCount, nullptr);
                if (spvReflectEnumerateInputVariables(&module, &inputVariableCount, inputVariables.data()) == SPV_REFLECT_RESULT_SUCCESS) {
                    for (const auto *inputVariable : inputVariables) {
                        if (!inputVariable) {
                            continue;
                        }
                        if ((inputVariable->decoration_flags & SPV_REFLECT_DECORATION_BUILT_IN) != 0) {
                            continue;
                        }

                        VkFormat vkFormat = VK_FORMAT_UNDEFINED;
                        U32      size     = 0;
                        if (!ConvertReflectedVertexFormat(inputVariable->format, vkFormat, size)) {
                            continue;
                        }

                        const U32 location = inputVariable->location;
                        const auto duplicate = std::find_if(
                            reflectedVertexInputs.begin(),
                            reflectedVertexInputs.end(),
                            [location](const JzReflectedVertexInput &entry) {
                                return entry.location == location;
                            });
                        if (duplicate != reflectedVertexInputs.end()) {
                            continue;
                        }

                        reflectedVertexInputs.push_back({location, vkFormat, size});
                    }
                }
            }
        }

        spvReflectDestroyShaderModule(&module);
    }

    if (!reflectedBindings.empty()) {
        U32 maxSetIndex = 0;
        for (const auto &[setIndex, _] : reflectedBindings) {
            maxSetIndex = std::max(maxSetIndex, setIndex);
        }

        m_descriptorSetLayouts.resize(static_cast<Size>(maxSetIndex) + 1U, VK_NULL_HANDLE);

        for (U32 setIndex = 0; setIndex <= maxSetIndex; ++setIndex) {
            std::vector<VkDescriptorSetLayoutBinding> bindings;
            const auto reflectedIter = reflectedBindings.find(setIndex);
            if (reflectedIter != reflectedBindings.end()) {
                bindings.reserve(reflectedIter->second.size());
                for (const auto &[bindingIndex, binding] : reflectedIter->second) {
                    (void)bindingIndex;
                    bindings.push_back(binding.layoutBinding);
                }

                std::sort(bindings.begin(), bindings.end(), [](const VkDescriptorSetLayoutBinding &lhs, const VkDescriptorSetLayoutBinding &rhs) {
                    return lhs.binding < rhs.binding;
                });
            }

            VkDescriptorSetLayoutCreateInfo setLayoutInfo{};
            setLayoutInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            setLayoutInfo.bindingCount = static_cast<U32>(bindings.size());
            setLayoutInfo.pBindings    = bindings.empty() ? nullptr : bindings.data();

            if (vkCreateDescriptorSetLayout(m_owner->GetVkDevice(), &setLayoutInfo, nullptr, &m_descriptorSetLayouts[setIndex]) != VK_SUCCESS) {
                JzRE_LOG_ERROR("JzVulkanPipeline: vkCreateDescriptorSetLayout failed for set {}", setIndex);
                DestroyDescriptorSetLayouts();
                return false;
            }
        }
    }

    VkPipelineLayoutCreateInfo layoutInfo{};
    layoutInfo.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layoutInfo.setLayoutCount = static_cast<U32>(m_descriptorSetLayouts.size());
    layoutInfo.pSetLayouts    = m_descriptorSetLayouts.empty() ? nullptr : m_descriptorSetLayouts.data();

    if (vkCreatePipelineLayout(m_owner->GetVkDevice(), &layoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
        JzRE_LOG_ERROR("JzVulkanPipeline: vkCreatePipelineLayout failed");
        DestroyDescriptorSetLayouts();
        return false;
    }

    std::vector<VkVertexInputBindingDescription>   vertexBindings;
    std::vector<VkVertexInputAttributeDescription> vertexAttributes;
    Bool hasExplicitLayout = false;

    if (desc.vertexLayout.IsValid()) {
        vertexBindings.reserve(desc.vertexLayout.bindings.size());
        for (const auto &bindingDesc : desc.vertexLayout.bindings) {
            if (bindingDesc.stride == 0) {
                continue;
            }

            VkVertexInputBindingDescription binding{};
            binding.binding   = bindingDesc.binding;
            binding.stride    = bindingDesc.stride;
            binding.inputRate = bindingDesc.perInstance ? VK_VERTEX_INPUT_RATE_INSTANCE : VK_VERTEX_INPUT_RATE_VERTEX;
            vertexBindings.push_back(binding);
        }

        vertexAttributes.reserve(desc.vertexLayout.attributes.size());
        const auto hasBinding = [&vertexBindings](U32 bindingIndex) {
            return std::any_of(
                vertexBindings.begin(),
                vertexBindings.end(),
                [bindingIndex](const VkVertexInputBindingDescription &binding) {
                    return binding.binding == bindingIndex;
                });
        };
        for (const auto &attributeDesc : desc.vertexLayout.attributes) {
            if (!hasBinding(attributeDesc.binding)) {
                continue;
            }

            VkFormat vkFormat = VK_FORMAT_UNDEFINED;
            U32      size     = 0;
            if (!ConvertVertexAttributeFormat(attributeDesc.format, vkFormat, size)) {
                continue;
            }

            (void)size;
            VkVertexInputAttributeDescription attribute{};
            attribute.location = attributeDesc.location;
            attribute.binding  = attributeDesc.binding;
            attribute.format   = vkFormat;
            attribute.offset   = attributeDesc.offset;
            vertexAttributes.push_back(attribute);
        }

        std::sort(vertexBindings.begin(), vertexBindings.end(), [](const VkVertexInputBindingDescription &lhs, const VkVertexInputBindingDescription &rhs) {
            return lhs.binding < rhs.binding;
        });
        std::sort(vertexAttributes.begin(), vertexAttributes.end(), [](const VkVertexInputAttributeDescription &lhs, const VkVertexInputAttributeDescription &rhs) {
            return lhs.location < rhs.location;
        });

        hasExplicitLayout = !vertexBindings.empty() && !vertexAttributes.empty();
        if (!hasExplicitLayout) {
            vertexBindings.clear();
            vertexAttributes.clear();
            JzRE_LOG_WARN("JzVulkanPipeline: invalid explicit vertex layout for pipeline '{}', fallback to reflection",
                          desc.debugName);
        }
    }

    if (!hasExplicitLayout && !reflectedVertexInputs.empty()) {
        std::sort(reflectedVertexInputs.begin(), reflectedVertexInputs.end(), [](const JzReflectedVertexInput &lhs, const JzReflectedVertexInput &rhs) {
            return lhs.location < rhs.location;
        });

        U32 currentOffset = 0;
        vertexAttributes.reserve(reflectedVertexInputs.size());
        for (const auto &input : reflectedVertexInputs) {
            VkVertexInputAttributeDescription attribute{};
            attribute.location = input.location;
            attribute.binding  = 0;
            attribute.format   = input.format;
            attribute.offset   = currentOffset;
            vertexAttributes.push_back(attribute);
            currentOffset += input.size;
        }

        if (currentOffset > 0) {
            VkVertexInputBindingDescription binding{};
            binding.binding   = 0;
            binding.stride    = currentOffset;
            binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            vertexBindings.push_back(binding);
        }
    }

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount   = static_cast<U32>(vertexBindings.size());
    vertexInputInfo.pVertexBindingDescriptions      = vertexBindings.empty() ? nullptr : vertexBindings.data();
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<U32>(vertexAttributes.size());
    vertexInputInfo.pVertexAttributeDescriptions    = vertexAttributes.empty() ? nullptr : vertexAttributes.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount  = 1;

    const auto &state = GetRenderState();

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable        = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode             = state.wireframe ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth               = 1.0f;
    rasterizer.cullMode                = ConvertCullMode(state.cullMode);
    rasterizer.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable         = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.sampleShadingEnable  = VK_FALSE;

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable       = state.depthTest ? VK_TRUE : VK_FALSE;
    depthStencil.depthWriteEnable      = state.depthWrite ? VK_TRUE : VK_FALSE;
    depthStencil.depthCompareOp        = VK_COMPARE_OP_LESS_OR_EQUAL;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable     = VK_FALSE;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                                          VK_COLOR_COMPONENT_G_BIT |
                                          VK_COLOR_COMPONENT_B_BIT |
                                          VK_COLOR_COMPONENT_A_BIT;
    ConfigureBlend(state.blendMode, colorBlendAttachment);

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable   = VK_FALSE;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments    = &colorBlendAttachment;

    const std::array<VkDynamicState, 2> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<U32>(dynamicStates.size());
    dynamicState.pDynamicStates    = dynamicStates.data();

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount          = static_cast<U32>(shaderStages.size());
    pipelineInfo.pStages             = shaderStages.data();
    pipelineInfo.pVertexInputState   = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState      = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState   = &multisampling;
    pipelineInfo.pDepthStencilState  = &depthStencil;
    pipelineInfo.pColorBlendState    = &colorBlending;
    pipelineInfo.pDynamicState       = &dynamicState;
    pipelineInfo.layout              = m_pipelineLayout;
    pipelineInfo.renderPass          = m_owner->GetSwapchainRenderPass();
    pipelineInfo.subpass             = 0;

    const VkResult pipelineResult = vkCreateGraphicsPipelines(
        m_owner->GetVkDevice(),
        VK_NULL_HANDLE,
        1,
        &pipelineInfo,
        nullptr,
        &m_pipeline);
    if (pipelineResult != VK_SUCCESS) {
        JzRE_LOG_ERROR("JzVulkanPipeline: vkCreateGraphicsPipelines failed with {}", static_cast<I32>(pipelineResult));
        vkDestroyPipelineLayout(m_owner->GetVkDevice(), m_pipelineLayout, nullptr);
        m_pipelineLayout = VK_NULL_HANDLE;
        DestroyDescriptorSetLayouts();
        return false;
    }

    if (!m_descriptorSetLayouts.empty()) {
        std::vector<VkDescriptorPoolSize> poolSizes;
        auto appendPoolSize = [&poolSizes](VkDescriptorType type, U32 count) {
            auto iter = std::find_if(
                poolSizes.begin(),
                poolSizes.end(),
                [type](const VkDescriptorPoolSize &entry) {
                    return entry.type == type;
                });
            if (iter == poolSizes.end()) {
                poolSizes.push_back({type, count});
            } else {
                iter->descriptorCount += count;
            }
        };

        m_uniformBindings.clear();
        m_samplerBindings.clear();

        for (const auto &[setIndex, setMap] : reflectedBindings) {
            for (const auto &[bindingIndex, reflectedBinding] : setMap) {
                (void)bindingIndex;
                appendPoolSize(reflectedBinding.layoutBinding.descriptorType, reflectedBinding.layoutBinding.descriptorCount);

                if (reflectedBinding.layoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
                    JzUniformBindingDesc uniformBinding;
                    uniformBinding.set     = setIndex;
                    uniformBinding.binding = reflectedBinding.layoutBinding.binding;
                    uniformBinding.size    = reflectedBinding.blockSize > 0 ? reflectedBinding.blockSize : 16;
                    uniformBinding.cpuData.assign(uniformBinding.size, 0);
                    for (const auto &[memberName, member] : reflectedBinding.members) {
                        uniformBinding.members[memberName] = {member.offset, member.size};
                    }
                    m_uniformBindings.push_back(std::move(uniformBinding));
                } else if (reflectedBinding.layoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
                    JzSamplerBindingDesc samplerBinding;
                    samplerBinding.set     = setIndex;
                    samplerBinding.binding = reflectedBinding.layoutBinding.binding;
                    samplerBinding.name    = reflectedBinding.name;
                    m_samplerBindings.push_back(std::move(samplerBinding));
                }
            }
        }

        if (!poolSizes.empty()) {
            VkDescriptorPoolCreateInfo poolInfo{};
            poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            poolInfo.maxSets       = static_cast<U32>(m_descriptorSetLayouts.size());
            poolInfo.poolSizeCount = static_cast<U32>(poolSizes.size());
            poolInfo.pPoolSizes    = poolSizes.data();

            if (vkCreateDescriptorPool(m_owner->GetVkDevice(), &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS) {
                JzRE_LOG_ERROR("JzVulkanPipeline: vkCreateDescriptorPool failed");
                vkDestroyPipeline(m_owner->GetVkDevice(), m_pipeline, nullptr);
                m_pipeline = VK_NULL_HANDLE;
                vkDestroyPipelineLayout(m_owner->GetVkDevice(), m_pipelineLayout, nullptr);
                m_pipelineLayout = VK_NULL_HANDLE;
                DestroyDescriptorSetLayouts();
                return false;
            }

            m_descriptorSets.resize(m_descriptorSetLayouts.size(), VK_NULL_HANDLE);

            VkDescriptorSetAllocateInfo allocInfo{};
            allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocInfo.descriptorPool     = m_descriptorPool;
            allocInfo.descriptorSetCount = static_cast<U32>(m_descriptorSetLayouts.size());
            allocInfo.pSetLayouts        = m_descriptorSetLayouts.data();

            if (vkAllocateDescriptorSets(m_owner->GetVkDevice(), &allocInfo, m_descriptorSets.data()) != VK_SUCCESS) {
                JzRE_LOG_ERROR("JzVulkanPipeline: vkAllocateDescriptorSets failed");
                DestroyDescriptorResources();
                vkDestroyPipeline(m_owner->GetVkDevice(), m_pipeline, nullptr);
                m_pipeline = VK_NULL_HANDLE;
                vkDestroyPipelineLayout(m_owner->GetVkDevice(), m_pipelineLayout, nullptr);
                m_pipelineLayout = VK_NULL_HANDLE;
                DestroyDescriptorSetLayouts();
                return false;
            }

            for (auto &uniformBinding : m_uniformBindings) {
                JzGPUBufferObjectDesc bufferDesc{};
                bufferDesc.type      = JzEGPUBufferObjectType::Uniform;
                bufferDesc.usage     = JzEGPUBufferObjectUsage::DynamicDraw;
                bufferDesc.size      = uniformBinding.size;
                bufferDesc.data      = uniformBinding.cpuData.data();
                bufferDesc.debugName = desc.debugName + "_UBO_" + std::to_string(uniformBinding.set) + "_" + std::to_string(uniformBinding.binding);

                uniformBinding.buffer = std::make_shared<JzVulkanBuffer>(*m_owner, bufferDesc);
                if (!uniformBinding.buffer || uniformBinding.buffer->GetBuffer() == VK_NULL_HANDLE) {
                    JzRE_LOG_ERROR("JzVulkanPipeline: failed to create uniform buffer (set={}, binding={})",
                                   uniformBinding.set,
                                   uniformBinding.binding);
                    DestroyDescriptorResources();
                    vkDestroyPipeline(m_owner->GetVkDevice(), m_pipeline, nullptr);
                    m_pipeline = VK_NULL_HANDLE;
                    vkDestroyPipelineLayout(m_owner->GetVkDevice(), m_pipelineLayout, nullptr);
                    m_pipelineLayout = VK_NULL_HANDLE;
                    DestroyDescriptorSetLayouts();
                    return false;
                }

                if (uniformBinding.set >= m_descriptorSets.size()) {
                    continue;
                }

                VkDescriptorBufferInfo bufferInfo{};
                bufferInfo.buffer = uniformBinding.buffer->GetBuffer();
                bufferInfo.offset = 0;
                bufferInfo.range  = uniformBinding.size;

                VkWriteDescriptorSet write{};
                write.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                write.dstSet          = m_descriptorSets[uniformBinding.set];
                write.dstBinding      = uniformBinding.binding;
                write.dstArrayElement = 0;
                write.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                write.descriptorCount = 1;
                write.pBufferInfo     = &bufferInfo;

                vkUpdateDescriptorSets(m_owner->GetVkDevice(), 1, &write, 0, nullptr);
            }
        }
    }

    return true;
}

void JzVulkanPipeline::DestroyDescriptorResources()
{
    m_samplerBindings.clear();
    m_uniformBindings.clear();
    m_descriptorSets.clear();

    if (m_owner && m_owner->GetVkDevice() != VK_NULL_HANDLE && m_descriptorPool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(m_owner->GetVkDevice(), m_descriptorPool, nullptr);
    }
    m_descriptorPool = VK_NULL_HANDLE;
}

void JzVulkanPipeline::DestroyDescriptorSetLayouts()
{
    if (!m_owner || m_owner->GetVkDevice() == VK_NULL_HANDLE) {
        m_descriptorSetLayouts.clear();
        return;
    }

    for (auto &setLayout : m_descriptorSetLayouts) {
        if (setLayout != VK_NULL_HANDLE) {
            vkDestroyDescriptorSetLayout(m_owner->GetVkDevice(), setLayout, nullptr);
            setLayout = VK_NULL_HANDLE;
        }
    }
    m_descriptorSetLayouts.clear();
}

} // namespace JzRE
