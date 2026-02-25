/**
 * @author  Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#include "JzRE/Runtime/Resource/JzShader.h"

#include <algorithm>
#include <cstring>
#include <fstream>
#include <unordered_set>

#include <nlohmann/json.hpp>

#include "JzRE/Runtime/Core/JzLogger.h"
#include "JzRE/Runtime/Core/JzServiceContainer.h"

namespace JzRE {

namespace {

using Json = nlohmann::json;

struct JzBlobHeader {
    char magic[4];
    U32  version;
    U32  chunkCount;
    U32  reserved;
};

struct JzBlobChunkHeader {
    U32 id;
    U32 offset;
    U32 size;
    U32 flags;
};

JzEVertexAttributeFormat ParseVertexFormat(const String &value)
{
    if (value == "Float") return JzEVertexAttributeFormat::Float;
    if (value == "Float2") return JzEVertexAttributeFormat::Float2;
    if (value == "Float3") return JzEVertexAttributeFormat::Float3;
    if (value == "Float4") return JzEVertexAttributeFormat::Float4;
    if (value == "Int") return JzEVertexAttributeFormat::Int;
    if (value == "Int2") return JzEVertexAttributeFormat::Int2;
    if (value == "Int3") return JzEVertexAttributeFormat::Int3;
    if (value == "Int4") return JzEVertexAttributeFormat::Int4;
    if (value == "UInt") return JzEVertexAttributeFormat::UInt;
    if (value == "UInt2") return JzEVertexAttributeFormat::UInt2;
    if (value == "UInt3") return JzEVertexAttributeFormat::UInt3;
    if (value == "UInt4") return JzEVertexAttributeFormat::UInt4;
    return JzEVertexAttributeFormat::Float3;
}

Bool ReadBinaryFile(const String &path, std::vector<U8> &outBytes)
{
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    file.seekg(0, std::ios::end);
    const auto size = file.tellg();
    file.seekg(0, std::ios::beg);

    if (size <= 0) {
        outBytes.clear();
        return true;
    }

    outBytes.resize(static_cast<Size>(size));
    file.read(reinterpret_cast<char *>(outBytes.data()), size);
    return file.good() || file.eof();
}

Bool ParseRenderState(const Json &node, JzRenderState &state)
{
    if (!node.is_object()) {
        return false;
    }

    if (node.contains("blendMode") && node["blendMode"].is_string()) {
        const auto blend = node["blendMode"].get<String>();
        if (blend == "Alpha")
            state.blendMode = JzEBlendMode::Alpha;
        else if (blend == "Additive")
            state.blendMode = JzEBlendMode::Additive;
        else if (blend == "Multiply")
            state.blendMode = JzEBlendMode::Multiply;
        else
            state.blendMode = JzEBlendMode::None;
    }

    if (node.contains("depthFunc") && node["depthFunc"].is_string()) {
        const auto depth = node["depthFunc"].get<String>();
        if (depth == "Never")
            state.depthFunc = JzEDepthFunc::Never;
        else if (depth == "Less")
            state.depthFunc = JzEDepthFunc::Less;
        else if (depth == "Equal")
            state.depthFunc = JzEDepthFunc::Equal;
        else if (depth == "LessEqual")
            state.depthFunc = JzEDepthFunc::LessEqual;
        else if (depth == "Greater")
            state.depthFunc = JzEDepthFunc::Greater;
        else if (depth == "NotEqual")
            state.depthFunc = JzEDepthFunc::NotEqual;
        else if (depth == "GreaterEqual")
            state.depthFunc = JzEDepthFunc::GreaterEqual;
        else if (depth == "Always")
            state.depthFunc = JzEDepthFunc::Always;
    }

    if (node.contains("cullMode") && node["cullMode"].is_string()) {
        const auto cull = node["cullMode"].get<String>();
        if (cull == "None")
            state.cullMode = JzECullMode::None;
        else if (cull == "Front")
            state.cullMode = JzECullMode::Front;
        else if (cull == "Back")
            state.cullMode = JzECullMode::Back;
        else if (cull == "FrontAndBack")
            state.cullMode = JzECullMode::FrontAndBack;
    }

    if (node.contains("depthTest")) state.depthTest = node["depthTest"].get<Bool>();
    if (node.contains("depthWrite")) state.depthWrite = node["depthWrite"].get<Bool>();
    if (node.contains("wireframe")) state.wireframe = node["wireframe"].get<Bool>();

    if (node.contains("msaa") && node["msaa"].is_number_unsigned()) {
        const auto msaa = node["msaa"].get<U32>();
        switch (msaa) {
            case 2: state.msaaLevel = JzEMSAALevel::X2; break;
            case 4: state.msaaLevel = JzEMSAALevel::X4; break;
            case 8: state.msaaLevel = JzEMSAALevel::X8; break;
            case 16: state.msaaLevel = JzEMSAALevel::X16; break;
            default: state.msaaLevel = JzEMSAALevel::None; break;
        }
    }

    return true;
}

} // namespace

JzShader::JzShader(const String &shaderPath)
{
    namespace fs = std::filesystem;

    fs::path path(shaderPath);
    if (path.extension().empty()) {
        path += ".jzshader";
    }

    m_manifestPath = path.string();
    m_name         = path.stem().string();
    m_state        = JzEResourceState::Unloaded;
}

JzShader::~JzShader()
{
    Unload();
}

Bool JzShader::Load()
{
    if (m_state == JzEResourceState::Loaded) {
        return true;
    }

    m_compileStatus = JzEShaderCompileStatus::Loading;
    m_compileLog.clear();
    m_state = JzEResourceState::Loading;

    m_dependentFiles.clear();
    m_keywords.clear();
    m_keywordBits.clear();
    m_reflectionLayouts.clear();
    m_vertexLayouts.clear();
    m_variants.clear();
    m_blobChunkTable.clear();
    m_blobData.clear();
    m_compiledVariants.clear();
    m_mainVariant.reset();

    if (!LoadManifest()) {
        m_compileStatus = JzEShaderCompileStatus::Failed;
        m_state         = JzEResourceState::Error;
        return false;
    }

    if (!LoadBlob()) {
        m_compileStatus = JzEShaderCompileStatus::Failed;
        m_state         = JzEResourceState::Error;
        return false;
    }

    m_compileStatus = JzEShaderCompileStatus::Compiling;

    if (!m_variants.empty()) {
        std::shared_ptr<JzRHIPipeline> defaultVariant;
        U64                            defaultMask = m_variants.front().keywordMask;
        for (const auto &variant : m_variants) {
            if (variant.keywordMask == 0) {
                defaultMask = 0;
                break;
            }
        }

        if (!BuildVariantPipeline(defaultMask, defaultVariant) || !defaultVariant) {
            m_compileStatus = JzEShaderCompileStatus::Failed;
            m_compileLog    = "Failed to build default shader variant pipeline";
            m_state         = JzEResourceState::Error;
            JzRE_LOG_ERROR("JzShader: {} ({})", m_compileLog, m_manifestPath);
            return false;
        }

        m_mainVariant = std::move(defaultVariant);
    }

    m_compileStatus = JzEShaderCompileStatus::Compiled;
    m_state         = JzEResourceState::Loaded;
    return true;
}

void JzShader::Unload()
{
    m_mainVariant.reset();
    m_compiledVariants.clear();

    m_keywords.clear();
    m_keywordBits.clear();
    m_reflectionLayouts.clear();
    m_vertexLayouts.clear();
    m_variants.clear();

    m_blobData.clear();
    m_blobChunkTable.clear();

    m_dependentFiles.clear();

    m_compileStatus = JzEShaderCompileStatus::NotLoaded;
    m_compileLog.clear();

    m_state = JzEResourceState::Unloaded;
}

std::shared_ptr<JzRHIPipeline> JzShader::GetVariant(U64 keywordMask)
{
    auto cached = m_compiledVariants.find(keywordMask);
    if (cached != m_compiledVariants.end()) {
        return cached->second;
    }

    std::shared_ptr<JzRHIPipeline> pipeline;
    if (!BuildVariantPipeline(keywordMask, pipeline)) {
        return nullptr;
    }

    if (pipeline) {
        m_compiledVariants[keywordMask] = pipeline;
        if (keywordMask == 0 && !m_mainVariant) {
            m_mainVariant = pipeline;
        }
    }

    return pipeline;
}

std::vector<JzShaderProgramDesc> JzShader::GetBackendProgramDesc(JzERHIType rhiType, U64 keywordMask) const
{
    std::vector<JzShaderProgramDesc> result;

    const auto *variant = FindVariant(keywordMask);
    if (!variant) {
        return result;
    }

    const auto *target = FindTarget(*variant, rhiType);
    if (!target) {
        return result;
    }

    result.reserve(target->stages.size());
    for (const auto &stageData : target->stages) {
        JzShaderProgramDesc desc;
        if (BuildProgramDesc(stageData, desc)) {
            result.push_back(std::move(desc));
        }
    }

    return result;
}

U64 JzShader::BuildKeywordMask(const std::unordered_map<String, String> &defines) const
{
    U64 mask = 0;

    for (const auto &[name, value] : defines) {
        auto bitIter = m_keywordBits.find(name);
        if (bitIter == m_keywordBits.end()) {
            continue;
        }

        const Bool enabled = !value.empty() && value != "0" && value != "false" && value != "False";
        if (!enabled) {
            continue;
        }

        if (bitIter->second < 64) {
            mask |= (1ULL << bitIter->second);
        }
    }

    return mask;
}

Bool JzShader::NeedsReload() const
{
    namespace fs = std::filesystem;

    try {
        if (!m_manifestPath.empty() && fs::exists(m_manifestPath)) {
            if (fs::last_write_time(m_manifestPath) > m_manifestLastModified) {
                return true;
            }
        }

        if (!m_blobPath.empty() && fs::exists(m_blobPath)) {
            if (fs::last_write_time(m_blobPath) > m_blobLastModified) {
                return true;
            }
        }
    } catch (const fs::filesystem_error &) {
        return false;
    }

    return false;
}

Bool JzShader::Reload()
{
    Unload();
    return Load();
}

Bool JzShader::LoadManifest()
{
    namespace fs = std::filesystem;

    std::ifstream file(m_manifestPath);
    if (!file.is_open()) {
        m_compileLog = "Failed to open shader manifest: " + m_manifestPath;
        JzRE_LOG_ERROR("JzShader: {}", m_compileLog);
        return false;
    }

    Json manifest;
    try {
        file >> manifest;
    } catch (const std::exception &e) {
        m_compileLog  = "Failed to parse shader manifest: ";
        m_compileLog += e.what();
        JzRE_LOG_ERROR("JzShader: {} ({})", m_compileLog, m_manifestPath);
        return false;
    }

    if (!manifest.contains("version") || !manifest.contains("shaderName") || !manifest.contains("sourceHash") || !manifest.contains("keywords") || !manifest.contains("variants") || !manifest.contains("targets") || !manifest.contains("reflectionLayouts") || !manifest.contains("vertexLayouts")) {
        m_compileLog = "Shader manifest missing required top-level fields";
        JzRE_LOG_ERROR("JzShader: {} ({})", m_compileLog, m_manifestPath);
        return false;
    }

    fs::path manifestDir = fs::path(m_manifestPath).parent_path();

    if (manifest.contains("blob") && manifest["blob"].is_string()) {
        fs::path blobPath = manifestDir / manifest["blob"].get<String>();
        m_blobPath        = blobPath.lexically_normal().string();
    } else {
        fs::path blobPath = fs::path(m_manifestPath);
        blobPath.replace_extension(".jzsblob");
        m_blobPath = blobPath.lexically_normal().string();
    }

    if (manifest["shaderName"].is_string()) {
        m_name = manifest["shaderName"].get<String>();
    }

    if (!manifest["keywords"].is_array()) {
        m_compileLog = "Shader manifest 'keywords' must be an array";
        JzRE_LOG_ERROR("JzShader: {} ({})", m_compileLog, m_manifestPath);
        return false;
    }

    {
        U32                     implicitBit = 0;
        std::unordered_set<U32> usedBits;
        for (const auto &entry : manifest["keywords"]) {
            JzShaderKeywordDefinition keyword;
            if (entry.is_string()) {
                keyword.name = entry.get<String>();
                keyword.bit  = implicitBit;
            } else if (entry.is_object() && entry.contains("name") && entry["name"].is_string()) {
                keyword.name = entry["name"].get<String>();
                if (entry.contains("bit") && entry["bit"].is_number_unsigned()) {
                    keyword.bit = entry["bit"].get<U32>();
                } else {
                    keyword.bit = implicitBit;
                }
            } else {
                m_compileLog = "Shader keyword entry must be string or object{name, bit}";
                JzRE_LOG_ERROR("JzShader: {} ({})", m_compileLog, m_manifestPath);
                return false;
            }

            if (keyword.name.empty()) {
                m_compileLog = "Shader keyword name cannot be empty";
                JzRE_LOG_ERROR("JzShader: {} ({})", m_compileLog, m_manifestPath);
                return false;
            }

            if (keyword.bit >= 64) {
                m_compileLog = "Shader keyword bit index exceeds 63: " + std::to_string(keyword.bit);
                JzRE_LOG_ERROR("JzShader: {} ({})", m_compileLog, m_manifestPath);
                return false;
            }

            if (m_keywordBits.find(keyword.name) != m_keywordBits.end()) {
                m_compileLog = "Duplicate shader keyword name: " + keyword.name;
                JzRE_LOG_ERROR("JzShader: {} ({})", m_compileLog, m_manifestPath);
                return false;
            }

            if (!usedBits.insert(keyword.bit).second) {
                m_compileLog = "Duplicate shader keyword bit index: " + std::to_string(keyword.bit);
                JzRE_LOG_ERROR("JzShader: {} ({})", m_compileLog, m_manifestPath);
                return false;
            }

            m_keywordBits[keyword.name] = keyword.bit;
            m_keywords.push_back(keyword);
            implicitBit = std::max<U32>(implicitBit + 1, keyword.bit + 1);
        }
    }

    if (!manifest["reflectionLayouts"].is_object()) {
        m_compileLog = "Shader manifest 'reflectionLayouts' must be an object";
        JzRE_LOG_ERROR("JzShader: {} ({})", m_compileLog, m_manifestPath);
        return false;
    }

    for (auto it = manifest["reflectionLayouts"].begin(); it != manifest["reflectionLayouts"].end(); ++it) {
        if (!it.value().is_object()) {
            m_compileLog = "Reflection layout entry must be an object: " + it.key();
            JzRE_LOG_ERROR("JzShader: {} ({})", m_compileLog, m_manifestPath);
            return false;
        }

        JzShaderLayoutDesc layout;
        const auto        &layoutNode = it.value();
        if (layoutNode.contains("resources")) {
            if (!layoutNode["resources"].is_array()) {
                m_compileLog = "Reflection layout resources must be an array: " + it.key();
                JzRE_LOG_ERROR("JzShader: {} ({})", m_compileLog, m_manifestPath);
                return false;
            }

            for (const auto &resourceNode : layoutNode["resources"]) {
                if (!resourceNode.is_object()) {
                    m_compileLog = "Reflection layout resource entry must be an object: " + it.key();
                    JzRE_LOG_ERROR("JzShader: {} ({})", m_compileLog, m_manifestPath);
                    return false;
                }

                const String typeName = resourceNode.value("type", String());
                if (typeName != "UniformBuffer" && typeName != "StorageBuffer" && typeName != "SampledTexture" && typeName != "Sampler" && typeName != "StorageTexture" && typeName != "PushConstants") {
                    m_compileLog = "Unknown reflection resource type: " + typeName;
                    JzRE_LOG_ERROR("JzShader: {} ({})", m_compileLog, m_manifestPath);
                    return false;
                }

                JzShaderResourceBindingDesc resource;
                resource.name      = resourceNode.value("name", String());
                resource.type      = ParseShaderResourceType(typeName);
                resource.set       = resourceNode.value("set", 0U);
                resource.binding   = resourceNode.value("binding", 0U);
                resource.arraySize = std::max<U32>(1U, resourceNode.value("arraySize", 1U));

                if (!MergeLayoutResource(layout, resource, it.key())) {
                    m_compileLog = "Reflection layout resource conflict in key: " + it.key();
                    JzRE_LOG_ERROR("JzShader: {} ({})", m_compileLog, m_manifestPath);
                    return false;
                }
            }
        }

        m_reflectionLayouts[it.key()] = std::move(layout);
    }

    if (!manifest["vertexLayouts"].is_object()) {
        m_compileLog = "Shader manifest 'vertexLayouts' must be an object";
        JzRE_LOG_ERROR("JzShader: {} ({})", m_compileLog, m_manifestPath);
        return false;
    }

    if (manifest.contains("vertexLayouts") && manifest["vertexLayouts"].is_object()) {
        for (auto it = manifest["vertexLayouts"].begin(); it != manifest["vertexLayouts"].end(); ++it) {
            if (!it.value().is_object()) {
                continue;
            }

            JzVertexLayoutDesc layout;

            if (it.value().contains("bindings") && it.value()["bindings"].is_array()) {
                for (const auto &bindingNode : it.value()["bindings"]) {
                    if (!bindingNode.is_object()) {
                        continue;
                    }

                    JzVertexBindingDesc binding;
                    binding.binding     = bindingNode.value("binding", 0U);
                    binding.stride      = bindingNode.value("stride", 0U);
                    binding.perInstance = bindingNode.value("perInstance", false);
                    layout.bindings.push_back(binding);
                }
            }

            if (it.value().contains("attributes") && it.value()["attributes"].is_array()) {
                for (const auto &attributeNode : it.value()["attributes"]) {
                    if (!attributeNode.is_object()) {
                        continue;
                    }

                    JzVertexAttributeDesc attribute;
                    attribute.location = attributeNode.value("location", 0U);
                    attribute.binding  = attributeNode.value("binding", 0U);
                    attribute.offset   = attributeNode.value("offset", 0U);
                    attribute.format   = ParseVertexFormat(attributeNode.value("format", String("Float3")));
                    layout.attributes.push_back(attribute);
                }
            }

            m_vertexLayouts[it.key()] = std::move(layout);
        }
    }

    if (manifest.contains("variants") && manifest["variants"].is_array()) {
        for (const auto &variantNode : manifest["variants"]) {
            if (!variantNode.is_object()) {
                continue;
            }

            JzShaderVariantData variant;
            variant.keywordMask      = variantNode.value("keywordMask", 0ULL);
            variant.vertexLayoutName = variantNode.value("vertexLayout", String("default"));

            if (variantNode.contains("renderState")) {
                ParseRenderState(variantNode["renderState"], variant.renderState);
            }

            if (variantNode.contains("targets") && variantNode["targets"].is_array()) {
                for (const auto &targetNode : variantNode["targets"]) {
                    if (!targetNode.is_object()) {
                        continue;
                    }

                    JzShaderTargetData target;
                    target.rhiType = ParseRhiType(targetNode.value("rhi", String("Unknown")));
                    if (target.rhiType == JzERHIType::Unknown) {
                        m_compileLog = "Shader target has unknown RHI type";
                        JzRE_LOG_ERROR("JzShader: {} ({})", m_compileLog, m_manifestPath);
                        return false;
                    }

                    if (targetNode.contains("stages") && targetNode["stages"].is_array()) {
                        for (const auto &stageNode : targetNode["stages"]) {
                            if (!stageNode.is_object()) {
                                continue;
                            }

                            JzShaderStageData stageData;
                            stageData.stage         = ParseShaderStage(stageNode.value("stage", String("Vertex")));
                            stageData.format        = ParseShaderFormat(stageNode.value("format", String("GLSL")));
                            stageData.chunkId       = stageNode.value("chunk", 0U);
                            stageData.entryPoint    = stageNode.value("entryPoint", String("main"));
                            stageData.reflectionKey = stageNode.value("reflectionKey", String());

                            if (stageData.entryPoint.empty()) {
                                m_compileLog = "Shader stage entryPoint is required";
                                JzRE_LOG_ERROR("JzShader: {} ({})", m_compileLog, m_manifestPath);
                                return false;
                            }

                            if (stageData.reflectionKey.empty()) {
                                m_compileLog = "Shader stage reflectionKey is required";
                                JzRE_LOG_ERROR("JzShader: {} ({})", m_compileLog, m_manifestPath);
                                return false;
                            }

                            target.stages.push_back(stageData);
                        }
                    }

                    if (!target.stages.empty()) {
                        variant.targets.push_back(std::move(target));
                    }
                }
            }

            if (!variant.targets.empty()) {
                m_variants.push_back(std::move(variant));
            }
        }
    }

    if (m_variants.empty()) {
        m_compileLog = "Shader manifest has no valid variants";
        JzRE_LOG_ERROR("JzShader: {} ({})", m_compileLog, m_manifestPath);
        return false;
    }

    try {
        if (fs::exists(m_manifestPath)) {
            m_manifestLastModified = fs::last_write_time(m_manifestPath);
        }
    } catch (const fs::filesystem_error &e) {
        JzRE_LOG_WARN("JzShader: Could not get manifest modification time: {}", e.what());
    }

    m_dependentFiles.push_back(m_manifestPath);
    m_dependentFiles.push_back(m_blobPath);

    return true;
}

Bool JzShader::LoadBlob()
{
    if (!ReadBinaryFile(m_blobPath, m_blobData)) {
        m_compileLog = "Failed to read shader blob: " + m_blobPath;
        JzRE_LOG_ERROR("JzShader: {}", m_compileLog);
        return false;
    }

    if (m_blobData.size() < sizeof(JzBlobHeader)) {
        m_compileLog = "Shader blob is too small";
        JzRE_LOG_ERROR("JzShader: {} ({})", m_compileLog, m_blobPath);
        return false;
    }

    JzBlobHeader header{};
    std::memcpy(&header, m_blobData.data(), sizeof(JzBlobHeader));

    if (std::memcmp(header.magic, "JZSB", 4) != 0) {
        m_compileLog = "Shader blob magic mismatch";
        JzRE_LOG_ERROR("JzShader: {} ({})", m_compileLog, m_blobPath);
        return false;
    }

    if (header.version != 1) {
        m_compileLog = "Unsupported shader blob version: " + std::to_string(header.version);
        JzRE_LOG_ERROR("JzShader: {} ({})", m_compileLog, m_blobPath);
        return false;
    }

    const Size expectedTableBytes = sizeof(JzBlobHeader) + static_cast<Size>(header.chunkCount) * sizeof(JzBlobChunkHeader);
    if (m_blobData.size() < expectedTableBytes) {
        m_compileLog = "Shader blob chunk table truncated";
        JzRE_LOG_ERROR("JzShader: {} ({})", m_compileLog, m_blobPath);
        return false;
    }

    const auto *chunkTable = reinterpret_cast<const JzBlobChunkHeader *>(m_blobData.data() + sizeof(JzBlobHeader));
    for (U32 i = 0; i < header.chunkCount; ++i) {
        const auto &chunk    = chunkTable[i];
        const Size  chunkEnd = static_cast<Size>(chunk.offset) + static_cast<Size>(chunk.size);
        if (chunkEnd > m_blobData.size()) {
            m_compileLog = "Shader blob contains out-of-range chunk";
            JzRE_LOG_ERROR("JzShader: {} ({})", m_compileLog, m_blobPath);
            return false;
        }

        m_blobChunkTable[chunk.id] = {chunk.offset, chunk.size};
    }

    try {
        if (std::filesystem::exists(m_blobPath)) {
            m_blobLastModified = std::filesystem::last_write_time(m_blobPath);
        }
    } catch (const std::filesystem::filesystem_error &e) {
        JzRE_LOG_WARN("JzShader: Could not get blob modification time: {}", e.what());
    }

    return true;
}

const JzShaderVariantData *JzShader::FindVariant(U64 keywordMask) const
{
    auto iter = std::find_if(m_variants.begin(), m_variants.end(), [keywordMask](const JzShaderVariantData &variant) {
        return variant.keywordMask == keywordMask;
    });

    if (iter != m_variants.end()) {
        return &(*iter);
    }

    // Fallback to default variant when exact mask is not cooked.
    iter = std::find_if(m_variants.begin(), m_variants.end(), [](const JzShaderVariantData &variant) {
        return variant.keywordMask == 0;
    });

    if (iter != m_variants.end()) {
        return &(*iter);
    }

    return m_variants.empty() ? nullptr : &m_variants.front();
}

const JzShaderTargetData *JzShader::FindTarget(const JzShaderVariantData &variant, JzERHIType rhiType) const
{
    auto exactIter = std::find_if(variant.targets.begin(), variant.targets.end(), [rhiType](const JzShaderTargetData &target) {
        return target.rhiType == rhiType;
    });
    if (exactIter != variant.targets.end()) {
        return &(*exactIter);
    }

    if (rhiType == JzERHIType::D3D11) {
        auto d3d12Iter = std::find_if(variant.targets.begin(), variant.targets.end(), [](const JzShaderTargetData &target) {
            return target.rhiType == JzERHIType::D3D12;
        });
        if (d3d12Iter != variant.targets.end()) {
            return &(*d3d12Iter);
        }
    }

    auto openglIter = std::find_if(variant.targets.begin(), variant.targets.end(), [](const JzShaderTargetData &target) {
        return target.rhiType == JzERHIType::OpenGL;
    });
    if (openglIter != variant.targets.end()) {
        return &(*openglIter);
    }

    auto vulkanIter = std::find_if(variant.targets.begin(), variant.targets.end(), [](const JzShaderTargetData &target) {
        return target.rhiType == JzERHIType::Vulkan;
    });
    if (vulkanIter != variant.targets.end()) {
        return &(*vulkanIter);
    }

    return variant.targets.empty() ? nullptr : &variant.targets.front();
}

Bool JzShader::BuildVariantPipeline(U64 keywordMask, std::shared_ptr<JzRHIPipeline> &outPipeline) const
{
    auto cacheIter = m_compiledVariants.find(keywordMask);
    if (cacheIter != m_compiledVariants.end() && cacheIter->second) {
        outPipeline = cacheIter->second;
        return true;
    }

    if (!JzServiceContainer::Has<JzDevice>()) {
        return false;
    }

    auto &device = JzServiceContainer::Get<JzDevice>();

    const auto *variant = FindVariant(keywordMask);
    if (!variant) {
        return false;
    }

    const auto *target = FindTarget(*variant, device.GetRHIType());
    if (!target) {
        return false;
    }

    JzPipelineDesc pipelineDesc{};
    pipelineDesc.renderState = variant->renderState;
    pipelineDesc.debugName   = m_name + "_" + std::to_string(variant->keywordMask);

    auto layoutIter = m_vertexLayouts.find(variant->vertexLayoutName);
    if (layoutIter != m_vertexLayouts.end()) {
        pipelineDesc.vertexLayout = layoutIter->second;
    }

    if (!BuildPipelineShaderLayout(*target, pipelineDesc.shaderLayout)) {
        return false;
    }

    for (const auto &stageData : target->stages) {
        JzShaderProgramDesc shaderDesc;
        if (!BuildProgramDesc(stageData, shaderDesc)) {
            return false;
        }
        shaderDesc.debugName = pipelineDesc.debugName + "_Stage" + std::to_string(static_cast<I32>(stageData.stage));
        pipelineDesc.shaders.push_back(std::move(shaderDesc));
    }

    if (pipelineDesc.shaders.empty()) {
        return false;
    }

    auto pipeline = device.CreatePipeline(pipelineDesc);
    if (!pipeline) {
        return false;
    }

    outPipeline                     = pipeline;
    m_compiledVariants[keywordMask] = outPipeline;
    return true;
}

Bool JzShader::BuildProgramDesc(const JzShaderStageData &stageData, JzShaderProgramDesc &outDesc) const
{
    auto chunkIter = m_blobChunkTable.find(stageData.chunkId);
    if (chunkIter == m_blobChunkTable.end()) {
        JzRE_LOG_ERROR("JzShader: Missing blob chunk {} for '{}'", stageData.chunkId, m_name);
        return false;
    }

    const auto &chunk  = chunkIter->second;
    const Size  offset = static_cast<Size>(chunk.offset);
    const Size  size   = static_cast<Size>(chunk.size);
    if (offset + size > m_blobData.size()) {
        JzRE_LOG_ERROR("JzShader: Blob chunk {} out of bounds for '{}'", stageData.chunkId, m_name);
        return false;
    }

    outDesc.stage         = stageData.stage;
    outDesc.format        = stageData.format;
    outDesc.entryPoint    = stageData.entryPoint;
    outDesc.reflectionKey = stageData.reflectionKey;
    outDesc.bytecodeOrText.assign(m_blobData.begin() + static_cast<Size>(offset),
                                  m_blobData.begin() + static_cast<Size>(offset + size));

    return true;
}

Bool JzShader::BuildPipelineShaderLayout(const JzShaderTargetData &target, JzShaderLayoutDesc &outLayout) const
{
    outLayout.resources.clear();

    for (const auto &stageData : target.stages) {
        if (stageData.reflectionKey.empty()) {
            continue;
        }

        auto layoutIter = m_reflectionLayouts.find(stageData.reflectionKey);
        if (layoutIter == m_reflectionLayouts.end()) {
            JzRE_LOG_ERROR("JzShader: Missing reflection layout key '{}' for '{}'",
                           stageData.reflectionKey, m_name);
            return false;
        }

        for (const auto &resource : layoutIter->second.resources) {
            if (!MergeLayoutResource(outLayout, resource, stageData.reflectionKey)) {
                return false;
            }
        }
    }

    return true;
}

Bool JzShader::MergeLayoutResource(JzShaderLayoutDesc                &layout,
                                   const JzShaderResourceBindingDesc &resource,
                                   const String                      &layoutKey) const
{
    auto iter = std::find_if(layout.resources.begin(), layout.resources.end(), [&resource](const JzShaderResourceBindingDesc &existing) {
        return existing.set == resource.set && existing.binding == resource.binding && existing.type == resource.type;
    });

    if (iter == layout.resources.end()) {
        layout.resources.push_back(resource);
        return true;
    }

    const Bool sameType       = iter->type == resource.type;
    const Bool sameArraySize  = iter->arraySize == resource.arraySize;
    const Bool compatibleName = iter->name.empty() || resource.name.empty() || iter->name == resource.name;
    if (sameType && sameArraySize && compatibleName) {
        if (iter->name.empty()) {
            iter->name = resource.name;
        }
        return true;
    }

    JzRE_LOG_ERROR(
        "JzShader: Reflection layout conflict key='{}' set={} binding={} existing(type={}, array={}, name='{}') new(type={}, array={}, name='{}')",
        layoutKey,
        resource.set,
        resource.binding,
        static_cast<I32>(iter->type),
        iter->arraySize,
        iter->name,
        static_cast<I32>(resource.type),
        resource.arraySize,
        resource.name);
    return false;
}

JzEShaderResourceType JzShader::ParseShaderResourceType(const String &name)
{
    if (name == "UniformBuffer") return JzEShaderResourceType::UniformBuffer;
    if (name == "StorageBuffer") return JzEShaderResourceType::StorageBuffer;
    if (name == "SampledTexture") return JzEShaderResourceType::SampledTexture;
    if (name == "Sampler") return JzEShaderResourceType::Sampler;
    if (name == "StorageTexture") return JzEShaderResourceType::StorageTexture;
    if (name == "PushConstants") return JzEShaderResourceType::PushConstants;
    return JzEShaderResourceType::UniformBuffer;
}

JzERHIType JzShader::ParseRhiType(const String &name)
{
    if (name == "OpenGL") return JzERHIType::OpenGL;
    if (name == "Vulkan") return JzERHIType::Vulkan;
    if (name == "D3D12") return JzERHIType::D3D12;
    if (name == "Metal") return JzERHIType::Metal;
    if (name == "D3D11") return JzERHIType::D3D11;
    return JzERHIType::Unknown;
}

JzEShaderProgramType JzShader::ParseShaderStage(const String &name)
{
    if (name == "Vertex") return JzEShaderProgramType::Vertex;
    if (name == "Fragment") return JzEShaderProgramType::Fragment;
    if (name == "Geometry") return JzEShaderProgramType::Geometry;
    if (name == "TessellationControl") return JzEShaderProgramType::TessellationControl;
    if (name == "TessellationEvaluation") return JzEShaderProgramType::TessellationEvaluation;
    if (name == "Compute") return JzEShaderProgramType::Compute;
    return JzEShaderProgramType::Vertex;
}

JzEShaderBinaryFormat JzShader::ParseShaderFormat(const String &name)
{
    if (name == "DXIL") return JzEShaderBinaryFormat::DXIL;
    if (name == "SPIRV") return JzEShaderBinaryFormat::SPIRV;
    if (name == "MSL") return JzEShaderBinaryFormat::MSL;
    if (name == "GLSL") return JzEShaderBinaryFormat::GLSL;
    return JzEShaderBinaryFormat::GLSL;
}

} // namespace JzRE
