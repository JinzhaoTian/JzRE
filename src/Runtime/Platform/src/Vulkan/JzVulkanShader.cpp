/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#include "JzRE/Runtime/Platform/Vulkan/JzVulkanShader.h"

#include <cstring>

#include "JzRE/Runtime/Core/JzLogger.h"
#include "JzRE/Runtime/Platform/Vulkan/JzVulkanDevice.h"

namespace JzRE {

JzVulkanShader::JzVulkanShader(JzVulkanDevice &device, const JzShaderProgramDesc &desc) :
    JzGPUShaderProgramObject(desc),
    m_device(device.GetVkDevice()),
    m_deviceAlive(device.GetLifetimeFlag())
{
    m_stage = ConvertStage(desc.stage);
    Compile();
}

JzVulkanShader::~JzVulkanShader()
{
    if (m_shaderModule == VK_NULL_HANDLE) {
        return;
    }

    if (m_deviceAlive && m_deviceAlive->load(std::memory_order_acquire) && m_device != VK_NULL_HANDLE) {
        vkDestroyShaderModule(m_device, m_shaderModule, nullptr);
    }

    m_shaderModule = VK_NULL_HANDLE;
}

Bool JzVulkanShader::Compile()
{
    m_isCompiled = false;
    m_compileLog.clear();
    m_spirv.clear();

    if (m_device == VK_NULL_HANDLE) {
        m_compileLog = "invalid Vulkan device";
        return false;
    }

    if (desc.format != JzEShaderBinaryFormat::SPIRV) {
        m_compileLog = "Vulkan backend requires SPIR-V payload";
        JzRE_LOG_ERROR("JzVulkanShader: {}", m_compileLog);
        return false;
    }

    if (desc.bytecodeOrText.empty()) {
        m_compileLog = "SPIR-V payload is empty";
        JzRE_LOG_ERROR("JzVulkanShader: {}", m_compileLog);
        return false;
    }

    if (desc.bytecodeOrText.size() % sizeof(U32) != 0) {
        m_compileLog = "SPIR-V payload size is not 4-byte aligned";
        JzRE_LOG_ERROR("JzVulkanShader: {}", m_compileLog);
        return false;
    }

    m_spirv.resize(desc.bytecodeOrText.size() / sizeof(U32));
    std::memcpy(m_spirv.data(), desc.bytecodeOrText.data(), desc.bytecodeOrText.size());

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = m_spirv.size() * sizeof(U32);
    createInfo.pCode    = m_spirv.data();

    if (vkCreateShaderModule(m_device, &createInfo, nullptr, &m_shaderModule) != VK_SUCCESS) {
        m_compileLog = "vkCreateShaderModule failed";
        JzRE_LOG_ERROR("JzVulkanShader: {}", m_compileLog);
        return false;
    }

    m_isCompiled = true;
    return true;
}

VkShaderStageFlagBits JzVulkanShader::ConvertStage(JzEShaderProgramType type)
{
    switch (type) {
        case JzEShaderProgramType::Vertex:
            return VK_SHADER_STAGE_VERTEX_BIT;
        case JzEShaderProgramType::Fragment:
            return VK_SHADER_STAGE_FRAGMENT_BIT;
        case JzEShaderProgramType::Geometry:
            return VK_SHADER_STAGE_GEOMETRY_BIT;
        case JzEShaderProgramType::TessellationControl:
            return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        case JzEShaderProgramType::TessellationEvaluation:
            return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        case JzEShaderProgramType::Compute:
            return VK_SHADER_STAGE_COMPUTE_BIT;
    }

    return VK_SHADER_STAGE_VERTEX_BIT;
}

} // namespace JzRE
