/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Resource/JzShaderManager.h"
#include "JzRE/Runtime/Core/JzLogger.h"
#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Runtime/Platform/JzDevice.h"
#include <fstream>
#include <sstream>

namespace JzRE {

void JzShaderManager::Initialize()
{
    RegisterBuiltInShaders();
}

void JzShaderManager::Shutdown()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_variantCache.clear();
    m_programs.clear();
}

void JzShaderManager::RegisterShaderProgram(const String &name, std::shared_ptr<JzShaderProgram> program)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_programs[name] = std::move(program);
}

std::shared_ptr<JzShaderProgram> JzShaderManager::GetShaderProgram(const String &name)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto                        it = m_programs.find(name);
    if (it != m_programs.end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<JzShaderVariant> JzShaderManager::GetVariant(const String             &programName,
                                                             const JzShaderVariantKey &variantKey)
{
    String cacheKey = MakeVariantCacheKey(programName, variantKey);

    {
        std::lock_guard<std::mutex> lock(m_mutex);

        // Check cache first
        auto it = m_variantCache.find(cacheKey);
        if (it != m_variantCache.end()) {
            return it->second;
        }
    }

    // Get the program
    auto program = GetShaderProgram(programName);
    if (!program) {
        return nullptr;
    }

    // Compile new variant
    auto variant = CompileVariant(program, variantKey);
    if (!variant) {
        return nullptr;
    }

    // Cache and return
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_variantCache[cacheKey] = variant;
    }

    return variant;
}

std::shared_ptr<JzShaderVariant> JzShaderManager::GetStandardShader(const JzShaderVariantKey &variantKey)
{
    return GetVariant("standard", variantKey);
}

std::shared_ptr<JzShaderVariant> JzShaderManager::GetUnlitShader()
{
    return GetVariant("unlit", {});
}

void JzShaderManager::InvalidateAllVariants()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_variantCache.clear();
}

JzShaderManager::Stats JzShaderManager::GetStats() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return Stats{m_programs.size(), m_variantCache.size()};
}

std::shared_ptr<JzShaderVariant> JzShaderManager::CompileVariant(std::shared_ptr<JzShaderProgram> program,
                                                                 const JzShaderVariantKey        &variantKey)
{
    auto &device = JzServiceContainer::Get<JzDevice>();

    // Generate preprocessor defines
    String defines = GenerateDefines(program, variantKey);

    // Prepend defines to shader sources
    String vertexSource   = defines + program->GetVertexSource();
    String fragmentSource = defines + program->GetFragmentSource();

    // Create shader descriptors
    JzShaderProgramDesc vsDesc{};
    vsDesc.type       = JzEShaderProgramType::Vertex;
    vsDesc.source     = vertexSource;
    vsDesc.entryPoint = "main";
    vsDesc.debugName  = program->GetName() + "_VS";

    JzShaderProgramDesc fsDesc{};
    fsDesc.type       = JzEShaderProgramType::Fragment;
    fsDesc.source     = fragmentSource;
    fsDesc.entryPoint = "main";
    fsDesc.debugName  = program->GetName() + "_FS";

    // Build pipeline descriptor
    JzPipelineDesc pipeDesc{};
    pipeDesc.shaders     = {vsDesc, fsDesc};
    pipeDesc.renderState = program->GetRenderState();
    pipeDesc.debugName   = program->GetName() + "_Pipeline";

    // Add geometry shader if present
    if (program->HasGeometryShader()) {
        String              geometrySource = defines + program->GetGeometrySource();
        JzShaderProgramDesc gsDesc{};
        gsDesc.type       = JzEShaderProgramType::Geometry;
        gsDesc.source     = geometrySource;
        gsDesc.entryPoint = "main";
        gsDesc.debugName  = program->GetName() + "_GS";
        pipeDesc.shaders.push_back(gsDesc);
    }

    // Create pipeline
    auto pipeline = device.CreatePipeline(pipeDesc);
    if (!pipeline) {
        return nullptr;
    }

    return std::make_shared<JzShaderVariant>(variantKey, pipeline);
}

String JzShaderManager::GenerateDefines(std::shared_ptr<JzShaderProgram> program, const JzShaderVariantKey &variantKey)
{
    String defines;

    for (const auto &keyword : program->GetKeywords()) {
        Bool isEnabled = (variantKey.keywordMask & (1ULL << keyword.index)) != 0;
        if (isEnabled) {
            defines += "#define " + keyword.name + " 1\n";
        }
    }

    return defines;
}

String JzShaderManager::MakeVariantCacheKey(const String &programName, const JzShaderVariantKey &variantKey)
{
    return programName + ":" + std::to_string(variantKey.keywordMask);
}

void JzShaderManager::RegisterBuiltInShaders()
{
    auto shaderDir = GetShaderDirectory();

    // ==================== Standard Shader ====================
    if (LoadShaderProgram("standard", shaderDir / "standard.vert", shaderDir / "standard.frag")) {
        auto program = GetShaderProgram("standard");
        if (program) {
            program->AddKeyword({"SKINNED", 0, false});
            program->AddKeyword({"SHADOWS", 1, false});
            program->AddKeyword({"NORMAL_MAPPING", 2, false});
        }
    } else {
        JzRE_LOG_ERROR("Failed to load built-in standard shader");
    }
}

Bool JzShaderManager::LoadShaderProgram(const String                &name,
                                        const std::filesystem::path &vertexPath,
                                        const std::filesystem::path &fragmentPath,
                                        const std::filesystem::path &geometryPath)
{
    String vertexSource = ReadShaderFile(vertexPath);
    if (vertexSource.empty()) {
        JzRE_LOG_ERROR("Failed to load vertex shader: {}", vertexPath.string());
        return false;
    }

    String fragmentSource = ReadShaderFile(fragmentPath);
    if (fragmentSource.empty()) {
        JzRE_LOG_ERROR("Failed to load fragment shader: {}", fragmentPath.string());
        return false;
    }

    auto program = std::make_shared<JzShaderProgram>(name);
    program->SetVertexSource(vertexSource);
    program->SetFragmentSource(fragmentSource);

    if (!geometryPath.empty()) {
        String geometrySource = ReadShaderFile(geometryPath);
        if (geometrySource.empty()) {
            JzRE_LOG_ERROR("Failed to load geometry shader: {}", geometryPath.string());
            return false;
        }
        program->SetGeometrySource(geometrySource);
    }

    // Set default render state
    JzRenderState renderState;
    renderState.depthTest = true;
    renderState.cullMode  = JzECullMode::None; // Disabled for debugging Cornell Box
    program->SetRenderState(renderState);

    RegisterShaderProgram(name, program);
    JzRE_LOG_INFO("Loaded shader program: {}", name);
    return true;
}

String JzShaderManager::ReadShaderFile(const std::filesystem::path &path)
{
    if (!std::filesystem::exists(path)) {
        JzRE_LOG_ERROR("Shader file not found: {}", path.string());
        return {};
    }

    std::ifstream file(path);
    if (!file.is_open()) {
        JzRE_LOG_ERROR("Failed to open shader file: {}", path.string());
        return {};
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::filesystem::path JzShaderManager::GetShaderDirectory() const
{
    return std::filesystem::current_path() / "shaders";
}

} // namespace JzRE
