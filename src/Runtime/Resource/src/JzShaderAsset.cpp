/**
 * @file    JzShaderAsset.cpp
 * @brief   Implementation of JzShaderAsset
 * @author  Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Resource/JzShaderAsset.h"
#include "JzRE/Runtime/Core/JzLogger.h"
#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Runtime/Platform/RHI/JzDevice.h"
#include "JzRE/Runtime/Platform/RHI/JzRHIPipeline.h"
#include "JzShaderVertexLayoutUtils.h"

#include <fstream>
#include <regex>
#include <sstream>

namespace JzRE {

namespace {

String BuildDefinesBlock(const std::unordered_map<String, String> &variantDefines,
                         const std::unordered_map<String, String> &sourceDefines,
                         JzERHIType                                 rhiType)
{
    String definesBlock;
    definesBlock.reserve(1024);

    definesBlock += "#define JZ_BACKEND_OPENGL ";
    definesBlock += (rhiType == JzERHIType::OpenGL) ? "1\n" : "0\n";
    definesBlock += "#define JZ_BACKEND_VULKAN ";
    definesBlock += (rhiType == JzERHIType::Vulkan) ? "1\n" : "0\n";

    for (const auto &[name, value] : variantDefines) {
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

} // namespace

JzShaderAsset::JzShaderAsset(const String &vertexPath, const String &fragmentPath) :
    m_vertexPath(vertexPath),
    m_fragmentPath(fragmentPath)
{
    m_name  = vertexPath + "|" + fragmentPath;
    m_state = JzEResourceState::Unloaded;
}

JzShaderAsset::JzShaderAsset(const String &baseName) :
    m_vertexPath(baseName + ".vert"),
    m_fragmentPath(baseName + ".frag")
{
    m_name  = baseName;
    m_state = JzEResourceState::Unloaded;
}

JzShaderAsset::~JzShaderAsset()
{
    Unload();
}

Bool JzShaderAsset::Load()
{
    if (m_state == JzEResourceState::Loaded) {
        return true;
    }

    m_compileStatus = JzEShaderCompileStatus::Loading;
    m_compileLog.clear();
    m_dependentFiles.clear();
    m_state = JzEResourceState::Loading;

    // Load vertex shader source
    if (!LoadSourceFromFile(m_vertexPath, m_sourceData.vertexSource)) {
        m_compileStatus = JzEShaderCompileStatus::Failed;
        m_compileLog    = "Failed to load vertex shader: " + m_vertexPath;
        m_state         = JzEResourceState::Error;
        JzRE_LOG_ERROR("JzShaderAsset: {}", m_compileLog);
        return false;
    }

    // Load fragment shader source
    if (!LoadSourceFromFile(m_fragmentPath, m_sourceData.fragmentSource)) {
        m_compileStatus = JzEShaderCompileStatus::Failed;
        m_compileLog    = "Failed to load fragment shader: " + m_fragmentPath;
        m_state         = JzEResourceState::Error;
        JzRE_LOG_ERROR("JzShaderAsset: {}", m_compileLog);
        return false;
    }

    // Load geometry shader if path is specified
    if (!m_geometryPath.empty()) {
        if (!LoadSourceFromFile(m_geometryPath, m_sourceData.geometrySource)) {
            JzRE_LOG_WARN("JzShaderAsset: Failed to load geometry shader: {}", m_geometryPath);
            // Geometry shader is optional, so we continue
        }
    }

    // Record file modification times for hot reload
    try {
        if (std::filesystem::exists(m_vertexPath)) {
            m_vertexLastModified = std::filesystem::last_write_time(m_vertexPath);
        }
        if (std::filesystem::exists(m_fragmentPath)) {
            m_fragmentLastModified = std::filesystem::last_write_time(m_fragmentPath);
        }
        if (!m_geometryPath.empty() && std::filesystem::exists(m_geometryPath)) {
            m_geometryLastModified = std::filesystem::last_write_time(m_geometryPath);
        }
    } catch (const std::filesystem::filesystem_error &e) {
        JzRE_LOG_WARN("JzShaderAsset: Could not get file modification time: {}", e.what());
    }

    // Compile the shader program
    if (!CompileProgram()) {
        m_state = JzEResourceState::Error;
        return false;
    }

    m_state = JzEResourceState::Loaded;
    JzRE_LOG_INFO("JzShaderAsset: Loaded shader '{}' successfully", GetName());
    return true;
}

void JzShaderAsset::Unload()
{
    m_program.reset();
    m_mainVariant.reset();
    m_variants.clear();

    m_sourceData.vertexSource.clear();
    m_sourceData.fragmentSource.clear();
    m_sourceData.geometrySource.clear();
    m_sourceData.defines.clear();

    m_compileStatus = JzEShaderCompileStatus::NotLoaded;
    m_compileLog.clear();
    m_dependentFiles.clear();

    m_state = JzEResourceState::Unloaded;
}

std::shared_ptr<JzShaderVariant> JzShaderAsset::GetVariant(const std::unordered_map<String, String> &defines)
{
    // If no defines, return main variant
    if (defines.empty()) {
        return m_mainVariant;
    }

    // Check if variant already exists
    for (const auto &variant : m_variants) {
        if (variant.defines == defines) {
            return variant.variant;
        }
    }

    // Compile new variant
    if (!m_program) {
        JzRE_LOG_ERROR("JzShaderAsset: Cannot compile variant - program not loaded");
        return nullptr;
    }

    // Generate variant key from defines
    JzShaderVariantKey variantKey;
    for (const auto &[name, value] : defines) {
        I32 index = m_program->GetKeywordIndex(name);
        if (index >= 0) {
            variantKey.keywordMask |= (1ULL << static_cast<U32>(index));
        }
    }

    // Create preprocessor defines string
    auto &device     = JzServiceContainer::Get<JzDevice>();
    auto  definesStr = BuildDefinesBlock(defines, m_sourceData.defines, device.GetRHIType());

    // Inject defines after #version to keep GLSL valid for Vulkan.
    String vertexWithDefines   = InjectDefinesAfterVersion(m_sourceData.vertexSource, definesStr);
    String fragmentWithDefines = InjectDefinesAfterVersion(m_sourceData.fragmentSource, definesStr);

    // Create shader descriptors
    JzShaderProgramDesc vsDesc{};
    vsDesc.type       = JzEShaderProgramType::Vertex;
    vsDesc.source     = vertexWithDefines;
    vsDesc.entryPoint = "main";
    vsDesc.debugName  = GetName() + "_Variant_VS";

    JzShaderProgramDesc fsDesc{};
    fsDesc.type       = JzEShaderProgramType::Fragment;
    fsDesc.source     = fragmentWithDefines;
    fsDesc.entryPoint = "main";
    fsDesc.debugName  = GetName() + "_Variant_FS";

    // Build pipeline descriptor
    JzPipelineDesc pipeDesc{};
    pipeDesc.shaders = {vsDesc, fsDesc};
    if (const auto vertexLayout = JzShaderVertexLayoutUtils::BuildVertexLayoutFromVertexSource(vertexWithDefines); vertexLayout.has_value()) {
        pipeDesc.vertexLayout = *vertexLayout;
    }
    if (m_program) {
        pipeDesc.renderState = m_program->GetRenderState();
    }
    pipeDesc.debugName = GetName() + "_Variant_Pipeline";

    auto pipeline = device.CreatePipeline(pipeDesc);
    if (!pipeline) {
        JzRE_LOG_ERROR("JzShaderAsset: Failed to compile shader variant");
        return nullptr;
    }

    // Cache the variant
    auto newVariant = std::make_shared<JzShaderVariant>(variantKey, pipeline);

    JzShaderAssetVariant assetVariant;
    assetVariant.name    = GenerateVariantName(defines);
    assetVariant.defines = defines;
    assetVariant.variant = newVariant;
    m_variants.push_back(std::move(assetVariant));

    JzRE_LOG_INFO("JzShaderAsset: Compiled variant '{}' for shader '{}'",
                  m_variants.back().name, GetName());
    return newVariant;
}

std::shared_ptr<JzShaderVariant> JzShaderAsset::GetVariant(const JzShaderVariantKey &key)
{
    // Convert key to defines map
    std::unordered_map<String, String> defines;

    if (m_program) {
        const auto &keywords = m_program->GetKeywords();
        for (const auto &keyword : keywords) {
            Bool isEnabled = (key.keywordMask & (1ULL << keyword.index)) != 0;
            if (isEnabled) {
                defines[keyword.name] = "1";
            }
        }
    }

    return GetVariant(defines);
}

Bool JzShaderAsset::NeedsReload() const
{
    try {
        if (std::filesystem::exists(m_vertexPath)) {
            auto currentVertexTime = std::filesystem::last_write_time(m_vertexPath);
            if (currentVertexTime > m_vertexLastModified) {
                return true;
            }
        }

        if (std::filesystem::exists(m_fragmentPath)) {
            auto currentFragmentTime = std::filesystem::last_write_time(m_fragmentPath);
            if (currentFragmentTime > m_fragmentLastModified) {
                return true;
            }
        }

        if (!m_geometryPath.empty() && std::filesystem::exists(m_geometryPath)) {
            auto currentGeometryTime = std::filesystem::last_write_time(m_geometryPath);
            if (currentGeometryTime > m_geometryLastModified) {
                return true;
            }
        }

        // TODO: Check dependent files (includes) - would need to track their modification times
    } catch (const std::filesystem::filesystem_error &) {
        // File might have been deleted
        return false;
    }

    return false;
}

Bool JzShaderAsset::Reload()
{
    JzRE_LOG_INFO("JzShaderAsset: Reloading shader '{}'", GetName());

    // Save current variants for recompilation
    auto oldVariants = m_variants;

    // Unload current shader
    Unload();

    // Reload
    if (!Load()) {
        return false;
    }

    // Recompile variants
    for (const auto &oldVariant : oldVariants) {
        GetVariant(oldVariant.defines);
    }

    return true;
}

Bool JzShaderAsset::LoadSourceFromFile(const String &path, String &outSource)
{
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    String rawSource = buffer.str();

    // Process includes
    outSource = ProcessIncludes(rawSource, path, m_dependentFiles);
    return true;
}

String JzShaderAsset::ProcessIncludes(const String        &source,
                                      const String        &basePath,
                                      std::vector<String> &outIncludes)
{
    // Simple #include processor
    std::regex  includeRegex(R"(#include\s*[<"]([^>"]+)[>"])");
    std::string result = source;

    std::smatch match;
    std::string workingSource = source;
    size_t      offset        = 0;

    while (std::regex_search(workingSource, match, includeRegex)) {
        String includePath = match[1].str();

        // Resolve include path relative to current file
        std::filesystem::path currentDir      = std::filesystem::path(basePath).parent_path();
        std::filesystem::path fullIncludePath = currentDir / includePath;

        // Add to dependent files
        outIncludes.push_back(fullIncludePath.string());

        // Load include file
        std::ifstream includeFile(fullIncludePath);
        String        includeContent;
        if (includeFile.is_open()) {
            std::stringstream buffer;
            buffer << includeFile.rdbuf();
            includeContent = buffer.str();
        } else {
            JzRE_LOG_WARN("JzShaderAsset: Could not find include file: {}", fullIncludePath.string());
            includeContent = "// Failed to include: " + includePath + "\n";
        }

        // Replace include directive with content
        size_t matchPos    = offset + static_cast<size_t>(match.position());
        size_t matchLength = static_cast<size_t>(match.length());
        result.replace(matchPos, matchLength, includeContent);
        offset += includeContent.length() - matchLength;

        workingSource = match.suffix().str();
    }

    return result;
}

Bool JzShaderAsset::CompileProgram()
{
    m_compileStatus = JzEShaderCompileStatus::Compiling;

    // Create shader program
    m_program = std::make_shared<JzShaderProgram>(GetName());
    m_program->SetVertexSource(m_sourceData.vertexSource);
    m_program->SetFragmentSource(m_sourceData.fragmentSource);

    if (!m_sourceData.geometrySource.empty()) {
        m_program->SetGeometrySource(m_sourceData.geometrySource);
    }

    // Apply any default defines
    auto &device     = JzServiceContainer::Get<JzDevice>();
    auto  definesStr = BuildDefinesBlock({}, m_sourceData.defines, device.GetRHIType());

    String vertexWithDefines   = InjectDefinesAfterVersion(m_sourceData.vertexSource, definesStr);
    String fragmentWithDefines = InjectDefinesAfterVersion(m_sourceData.fragmentSource, definesStr);

    // Create shader descriptors
    JzShaderProgramDesc vsDesc{};
    vsDesc.type       = JzEShaderProgramType::Vertex;
    vsDesc.source     = vertexWithDefines;
    vsDesc.entryPoint = "main";
    vsDesc.debugName  = GetName() + "_VS";

    JzShaderProgramDesc fsDesc{};
    fsDesc.type       = JzEShaderProgramType::Fragment;
    fsDesc.source     = fragmentWithDefines;
    fsDesc.entryPoint = "main";
    fsDesc.debugName  = GetName() + "_FS";

    // Build pipeline descriptor
    JzPipelineDesc pipeDesc{};
    pipeDesc.shaders = {vsDesc, fsDesc};
    if (const auto vertexLayout = JzShaderVertexLayoutUtils::BuildVertexLayoutFromVertexSource(vertexWithDefines); vertexLayout.has_value()) {
        pipeDesc.vertexLayout = *vertexLayout;
    }

    // Set default render state
    JzRenderState renderState;
    renderState.depthTest = true;
    renderState.cullMode  = JzECullMode::Front;
    pipeDesc.renderState  = renderState;
    m_program->SetRenderState(renderState);

    pipeDesc.debugName = GetName() + "_Pipeline";

    // Add geometry shader if present
    if (!m_sourceData.geometrySource.empty()) {
        String              geometryWithDefines = InjectDefinesAfterVersion(m_sourceData.geometrySource, definesStr);
        JzShaderProgramDesc gsDesc{};
        gsDesc.type       = JzEShaderProgramType::Geometry;
        gsDesc.source     = geometryWithDefines;
        gsDesc.entryPoint = "main";
        gsDesc.debugName  = GetName() + "_GS";
        pipeDesc.shaders.push_back(gsDesc);
    }

    auto pipeline = device.CreatePipeline(pipeDesc);
    if (!pipeline) {
        m_compileStatus = JzEShaderCompileStatus::Failed;
        m_compileLog    = "Failed to create pipeline";
        JzRE_LOG_ERROR("JzShaderAsset: {}", m_compileLog);
        return false;
    }

    // Create main variant
    m_mainVariant   = std::make_shared<JzShaderVariant>(JzShaderVariantKey(), pipeline);
    m_compileStatus = JzEShaderCompileStatus::Compiled;

    return true;
}

String JzShaderAsset::GenerateVariantName(const std::unordered_map<String, String> &defines) const
{
    if (defines.empty()) {
        return "default";
    }

    String name;
    for (const auto &[key, value] : defines) {
        if (!name.empty()) {
            name += "_";
        }
        name += key;
        if (value != "1" && !value.empty()) {
            name += "=" + value;
        }
    }
    return name;
}

} // namespace JzRE
