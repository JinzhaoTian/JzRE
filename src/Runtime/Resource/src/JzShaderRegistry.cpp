/**
 * @file    JzShaderRegistry.cpp
 * @brief   Implementation of JzShaderRegistry
 * @author  Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Resource/JzShaderRegistry.h"

#include <sstream>

#include "JzRE/Runtime/Core/JzLogger.h"
#include "JzRE/Runtime/Platform/RHI/JzDevice.h"
#include "JzRE/Runtime/Platform/RHI/JzRHIPipeline.h"
#include "JzRE/Runtime/Resource/JzShaderVariant.h"

namespace JzRE {

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
    // Generate defines string
    String definesStr;
    for (const auto &[name, value] : defines) {
        definesStr += "#define " + name + " " + value + "\n";
    }

    // Also add source-level defines
    for (const auto &[name, value] : source.defines) {
        definesStr += "#define " + name + " " + value + "\n";
    }

    // Prepend defines to shader sources
    String vertexWithDefines   = definesStr + source.vertexSource;
    String fragmentWithDefines = definesStr + source.fragmentSource;

    // Create shader descriptors
    JzShaderProgramDesc vsDesc{};
    vsDesc.type       = JzEShaderProgramType::Vertex;
    vsDesc.source     = vertexWithDefines;
    vsDesc.entryPoint = "main";
    vsDesc.debugName  = "CompiledVariant_VS";

    JzShaderProgramDesc fsDesc{};
    fsDesc.type       = JzEShaderProgramType::Fragment;
    fsDesc.source     = fragmentWithDefines;
    fsDesc.entryPoint = "main";
    fsDesc.debugName  = "CompiledVariant_FS";

    // Build pipeline descriptor
    JzPipelineDesc pipeDesc{};
    pipeDesc.shaders = {vsDesc, fsDesc};

    // Set default render state
    JzRenderState renderState;
    renderState.depthTest = true;
    renderState.cullMode  = JzECullMode::Front;
    pipeDesc.renderState  = renderState;

    pipeDesc.debugName = "CompiledVariant_Pipeline";

    // Add geometry shader if present
    if (!source.geometrySource.empty()) {
        String              geometryWithDefines = definesStr + source.geometrySource;
        JzShaderProgramDesc gsDesc{};
        gsDesc.type       = JzEShaderProgramType::Geometry;
        gsDesc.source     = geometryWithDefines;
        gsDesc.entryPoint = "main";
        gsDesc.debugName  = "CompiledVariant_GS";
        pipeDesc.shaders.push_back(gsDesc);
    }

    pipeline = m_device.CreatePipeline(pipeDesc);
    if (!pipeline) {
        log = "Failed to create pipeline";
        return false;
    }

    return true;
}

// ==================== JzShaderRegistry ====================

JzShaderRegistry::JzShaderRegistry(JzDevice &device) :
    m_device(device)
{
    // Create default OpenGL compiler
    m_compiler = std::make_unique<JzOpenGLShaderCompiler>(device);

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
