/**
 * @file    JzShaderRegistry.cpp
 * @brief   Implementation of JzShaderRegistry
 * @author  Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Resource/JzShaderRegistry.h"

#include <sstream>

#include <shaderc/shaderc.hpp>
#include <spirv_reflect.h>

#include "JzRE/Runtime/Core/JzLogger.h"
#include "JzRE/Runtime/Platform/RHI/JzDevice.h"
#include "JzRE/Runtime/Platform/RHI/JzRHIPipeline.h"
#include "JzRE/Runtime/Resource/JzShaderVariant.h"

namespace JzRE {

namespace {

String BuildDefinesBlock(const std::unordered_map<String, String> &defines,
                         const std::unordered_map<String, String> &sourceDefines,
                         const String                             &backendMacro,
                         const String                             &backendValue)
{
    String definesBlock;
    definesBlock.reserve(1024);

    definesBlock += "#define ";
    definesBlock += backendMacro;
    definesBlock += " ";
    definesBlock += backendValue;
    definesBlock += "\n";

    for (const auto &[name, value] : defines) {
        definesBlock += "#define " + name + " " + value + "\n";
    }
    for (const auto &[name, value] : sourceDefines) {
        definesBlock += "#define " + name + " " + value + "\n";
    }

    return definesBlock;
}

String InjectDefinesAfterVersion(const String &source, const String &definesBlock)
{
    if (definesBlock.empty()) {
        return source;
    }

    if (source.rfind("#version", 0) == 0) {
        const auto lineEnd = source.find('\n');
        if (lineEnd != String::npos) {
            String merged;
            merged.reserve(source.size() + definesBlock.size() + 1);
            merged.append(source, 0, lineEnd + 1);
            merged += definesBlock;
            merged.append(source, lineEnd + 1, String::npos);
            return merged;
        }
    }

    return definesBlock + source;
}

Bool ValidateSpirvWithReflection(const String &source, shaderc_shader_kind kind, const String &stageName, String &log)
{
    shaderc::Compiler       compiler;
    shaderc::CompileOptions options;

    options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
    options.SetOptimizationLevel(shaderc_optimization_level_performance);

    auto result = compiler.CompileGlslToSpv(source, kind, stageName.c_str(), "main", options);
    if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
        log = "shaderc compile failed(" + stageName + "): " + result.GetErrorMessage();
        return false;
    }

    std::vector<U32> spirv(result.cbegin(), result.cend());

    SpvReflectShaderModule module{};
    const auto             reflectResult = spvReflectCreateShaderModule(
        spirv.size() * sizeof(U32),
        spirv.data(),
        &module);
    if (reflectResult != SPV_REFLECT_RESULT_SUCCESS) {
        log = "spirv-reflect failed(" + stageName + ")";
        return false;
    }

    spvReflectDestroyShaderModule(&module);
    return true;
}

Bool BuildPipelineFromSources(JzDevice                              &device,
                              const JzShaderSourceData              &source,
                              const JzRenderState                   &renderState,
                              const String                          &definesBlock,
                              std::shared_ptr<JzRHIPipeline>        &pipeline,
                              const String                          &pipelineDebugName,
                              String                                &log)
{
    JzShaderProgramDesc vsDesc{};
    vsDesc.type       = JzEShaderProgramType::Vertex;
    vsDesc.source     = InjectDefinesAfterVersion(source.vertexSource, definesBlock);
    vsDesc.entryPoint = "main";
    vsDesc.debugName  = pipelineDebugName + "_VS";

    JzShaderProgramDesc fsDesc{};
    fsDesc.type       = JzEShaderProgramType::Fragment;
    fsDesc.source     = InjectDefinesAfterVersion(source.fragmentSource, definesBlock);
    fsDesc.entryPoint = "main";
    fsDesc.debugName  = pipelineDebugName + "_FS";

    JzPipelineDesc pipelineDesc{};
    pipelineDesc.shaders     = {vsDesc, fsDesc};
    pipelineDesc.renderState = renderState;
    pipelineDesc.debugName   = pipelineDebugName;

    if (!source.geometrySource.empty()) {
        JzShaderProgramDesc gsDesc{};
        gsDesc.type       = JzEShaderProgramType::Geometry;
        gsDesc.source     = InjectDefinesAfterVersion(source.geometrySource, definesBlock);
        gsDesc.entryPoint = "main";
        gsDesc.debugName  = pipelineDebugName + "_GS";
        pipelineDesc.shaders.push_back(gsDesc);
    }

    pipeline = device.CreatePipeline(pipelineDesc);
    if (!pipeline) {
        log = "failed to create pipeline object";
        return false;
    }

    return true;
}

} // namespace

// ==================== JzOpenGLShaderCompiler ====================

JzOpenGLShaderCompiler::JzOpenGLShaderCompiler(JzDevice &device) :
    m_device(device)
{ }

Bool JzOpenGLShaderCompiler::Compile(const JzShaderSourceData                 &source,
                                     const JzShaderCompileConfig              &config,
                                     const std::unordered_map<String, String> &defines,
                                     std::shared_ptr<JzRHIPipeline>           &pipeline,
                                     String                                   &log)
{
    (void)config;

    JzRenderState renderState;
    renderState.depthTest = true;
    renderState.cullMode  = JzECullMode::Front;

    const String definesBlock = BuildDefinesBlock(defines, source.defines, "JZ_BACKEND_OPENGL", "1");
    return BuildPipelineFromSources(
        m_device,
        source,
        renderState,
        definesBlock,
        pipeline,
        "CompiledVariant_OpenGL",
        log);
}

// ==================== JzShaderRegistry ====================

// ==================== JzVulkanShaderCompiler ====================

JzVulkanShaderCompiler::JzVulkanShaderCompiler(JzDevice &device) :
    m_device(device)
{ }

Bool JzVulkanShaderCompiler::Compile(const JzShaderSourceData                 &source,
                                     const JzShaderCompileConfig              &config,
                                     const std::unordered_map<String, String> &defines,
                                     std::shared_ptr<JzRHIPipeline>           &pipeline,
                                     String                                   &log)
{
    (void)config;

    JzRenderState renderState;
    renderState.depthTest = true;
    renderState.cullMode  = JzECullMode::Front;

    const String definesBlock = BuildDefinesBlock(defines, source.defines, "JZ_BACKEND_VULKAN", "1");

    const String vertexSource   = InjectDefinesAfterVersion(source.vertexSource, definesBlock);
    const String fragmentSource = InjectDefinesAfterVersion(source.fragmentSource, definesBlock);
    if (!ValidateSpirvWithReflection(vertexSource, shaderc_glsl_vertex_shader, "VulkanVertex", log)) {
        JzRE_LOG_WARN("JzVulkanShaderCompiler: vertex pre-validation skipped: {}", log);
        log.clear();
    }
    if (!ValidateSpirvWithReflection(fragmentSource, shaderc_glsl_fragment_shader, "VulkanFragment", log)) {
        JzRE_LOG_WARN("JzVulkanShaderCompiler: fragment pre-validation skipped: {}", log);
        log.clear();
    }
    if (!source.geometrySource.empty()) {
        const String geometrySource = InjectDefinesAfterVersion(source.geometrySource, definesBlock);
        if (!ValidateSpirvWithReflection(geometrySource, shaderc_glsl_geometry_shader, "VulkanGeometry", log)) {
            JzRE_LOG_WARN("JzVulkanShaderCompiler: geometry pre-validation skipped: {}", log);
            log.clear();
        }
    }

    return BuildPipelineFromSources(
        m_device,
        source,
        renderState,
        definesBlock,
        pipeline,
        "CompiledVariant_Vulkan",
        log);
}

JzShaderRegistry::JzShaderRegistry(JzDevice &device) :
    m_device(device)
{
    if (m_device.GetRHIType() == JzERHIType::Vulkan) {
        m_compiler = std::make_unique<JzVulkanShaderCompiler>(device);
    } else {
        m_compiler = std::make_unique<JzOpenGLShaderCompiler>(device);
    }

    JzRE_LOG_INFO("JzShaderRegistry: Initialized with {} compiler", m_compiler->GetName());
}

JzShaderRegistry::~JzShaderRegistry()
{
    JzRE_LOG_INFO("JzShaderRegistry: Shutdown, compiled {} variants total", m_compiledVariantCount);
}

void JzShaderRegistry::SetCompiler(std::unique_ptr<IShaderCompiler> compiler)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_compiler = std::move(compiler);
    JzRE_LOG_INFO("JzShaderRegistry: Compiler changed to {}", m_compiler->GetName());
}

std::shared_ptr<JzShaderVariant> JzShaderRegistry::CompileVariant(
    JzShaderAsset                            *shaderAsset,
    const std::unordered_map<String, String> &defines)
{
    if (!shaderAsset || !m_compiler) {
        return nullptr;
    }

    std::lock_guard<std::mutex> lock(m_mutex);

    std::shared_ptr<JzRHIPipeline> pipeline;
    String                         compileLog;

    Bool success = m_compiler->Compile(shaderAsset->GetSourceData(),
                                       shaderAsset->GetCompileConfig(),
                                       defines,
                                       pipeline,
                                       compileLog);

    if (!success) {
        JzRE_LOG_ERROR("JzShaderRegistry: Failed to compile variant for '{}': {}",
                       shaderAsset->GetName(), compileLog);
        return nullptr;
    }

    // Create variant key from defines
    JzShaderVariantKey variantKey;
    auto              *program = shaderAsset->GetProgram().get();
    if (program) {
        for (const auto &[name, value] : defines) {
            I32 index = program->GetKeywordIndex(name);
            if (index >= 0) {
                variantKey.keywordMask |= (1ULL << static_cast<U32>(index));
            }
        }
    }

    ++m_compiledVariantCount;

    JzRE_LOG_DEBUG("JzShaderRegistry: Compiled variant for '{}' with {} defines",
                   shaderAsset->GetName(), defines.size());

    return std::make_shared<JzShaderVariant>(variantKey, pipeline);
}

void JzShaderRegistry::RegisterIncludeDependency(const String &shaderPath, const String &includePath)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_includeDependencies[includePath].insert(shaderPath);
}

std::unordered_set<String> JzShaderRegistry::GetDependentShaders(const String &includePath) const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_includeDependencies.find(includePath);
    if (it != m_includeDependencies.end()) {
        return it->second;
    }
    return {};
}

void JzShaderRegistry::ClearDependencies()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_includeDependencies.clear();
}

std::vector<String> JzShaderRegistry::CheckForUpdates() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<String>         updatedShaders;

    for (const auto &[path, lastModTime] : m_shaderModTimes) {
        try {
            if (std::filesystem::exists(path)) {
                auto currentModTime = std::filesystem::last_write_time(path);
                if (currentModTime > lastModTime) {
                    updatedShaders.push_back(path);
                }
            }
        } catch (const std::filesystem::filesystem_error &) {
            // File might have been deleted or is inaccessible
        }
    }

    return updatedShaders;
}

void JzShaderRegistry::TrackShaderForReload(const String &shaderPath)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    try {
        if (std::filesystem::exists(shaderPath)) {
            m_shaderModTimes[shaderPath] = std::filesystem::last_write_time(shaderPath);
        }
    } catch (const std::filesystem::filesystem_error &e) {
        JzRE_LOG_WARN("JzShaderRegistry: Could not track shader '{}': {}", shaderPath, e.what());
    }
}

void JzShaderRegistry::UntrackShaderForReload(const String &shaderPath)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_shaderModTimes.erase(shaderPath);
}

Size JzShaderRegistry::GetTrackedShaderCount() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_shaderModTimes.size();
}

void JzShaderRegistry::PrecompileCommonVariants(JzShaderAsset             *shaderAsset,
                                                const std::vector<String> &keywords)
{
    if (!shaderAsset || keywords.empty()) {
        return;
    }

    auto combinations = GenerateKeywordCombinations(keywords);

    JzRE_LOG_INFO("JzShaderRegistry: Precompiling {} variants for '{}'",
                  combinations.size(), shaderAsset->GetName());

    for (const auto &defines : combinations) {
        auto variant = CompileVariant(shaderAsset, defines);
        if (variant) {
            // Store in shader asset's variant cache
            shaderAsset->GetVariant(defines);
        }
    }
}

String JzShaderRegistry::GenerateDefinesString(const std::unordered_map<String, String> &defines)
{
    std::ostringstream oss;
    for (const auto &[name, value] : defines) {
        oss << "#define " << name << " " << value << "\n";
    }
    return oss.str();
}

std::vector<std::unordered_map<String, String>>
JzShaderRegistry::GenerateKeywordCombinations(const std::vector<String> &keywords)
{
    std::vector<std::unordered_map<String, String>> combinations;

    // Generate all 2^n combinations
    Size totalCombinations = 1ULL << keywords.size();

    for (Size i = 0; i < totalCombinations; ++i) {
        std::unordered_map<String, String> defines;

        for (Size j = 0; j < keywords.size(); ++j) {
            Bool enabled         = (i & (1ULL << j)) != 0;
            defines[keywords[j]] = enabled ? "1" : "0";
        }

        combinations.push_back(std::move(defines));
    }

    return combinations;
}

} // namespace JzRE
