/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#include "JzRE/Runtime/Platform/Vulkan/JzVulkanShader.h"

#include <shaderc/shaderc.hpp>

#include "JzRE/Runtime/Core/JzLogger.h"
#include "JzRE/Runtime/Platform/Vulkan/JzVulkanDevice.h"

namespace JzRE {

namespace {

shaderc_shader_kind ConvertShaderKind(JzEShaderProgramType type)
{
    switch (type) {
        case JzEShaderProgramType::Vertex:
            return shaderc_glsl_vertex_shader;
        case JzEShaderProgramType::Fragment:
            return shaderc_glsl_fragment_shader;
        case JzEShaderProgramType::Geometry:
            return shaderc_glsl_geometry_shader;
        case JzEShaderProgramType::TessellationControl:
            return shaderc_glsl_tess_control_shader;
        case JzEShaderProgramType::TessellationEvaluation:
            return shaderc_glsl_tess_evaluation_shader;
        case JzEShaderProgramType::Compute:
            return shaderc_glsl_compute_shader;
    }

    return shaderc_glsl_infer_from_source;
}

} // namespace

JzVulkanShader::JzVulkanShader(JzVulkanDevice &device, const JzShaderProgramDesc &desc) :
    JzGPUShaderProgramObject(desc),
    m_owner(&device)
{
    m_stage = ConvertStage(desc.type);
    Compile();
}

JzVulkanShader::~JzVulkanShader()
{
    if (!m_owner || m_shaderModule == VK_NULL_HANDLE) {
        return;
    }

    vkDestroyShaderModule(m_owner->GetVkDevice(), m_shaderModule, nullptr);
    m_shaderModule = VK_NULL_HANDLE;
}

Bool JzVulkanShader::Compile()
{
    m_isCompiled = false;
    m_compileLog.clear();
    m_spirv.clear();

    if (!m_owner || m_owner->GetVkDevice() == VK_NULL_HANDLE) {
        m_compileLog = "invalid Vulkan device";
        return false;
    }

    shaderc::Compiler compiler;
    shaderc::CompileOptions options;

    options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
    options.SetOptimizationLevel(shaderc_optimization_level_performance);
    options.SetGenerateDebugInfo();

    const auto result = compiler.CompileGlslToSpv(
        desc.source,
        ConvertShaderKind(desc.type),
        desc.debugName.empty() ? "JzVulkanShader" : desc.debugName.c_str(),
        desc.entryPoint.empty() ? "main" : desc.entryPoint.c_str(),
        options);

    if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
        m_compileLog = result.GetErrorMessage();
        JzRE_LOG_ERROR("JzVulkanShader: shaderc compilation failed: {}", m_compileLog);
        return false;
    }

    m_spirv.assign(result.cbegin(), result.cend());

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = m_spirv.size() * sizeof(U32);
    createInfo.pCode    = m_spirv.data();

    if (vkCreateShaderModule(m_owner->GetVkDevice(), &createInfo, nullptr, &m_shaderModule) != VK_SUCCESS) {
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
