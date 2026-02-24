/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#pragma once

#include <filesystem>
#include <memory>
#include <unordered_map>
#include <vector>

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Platform/RHI/JzDevice.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUShaderProgramObject.h"
#include "JzRE/Runtime/Platform/RHI/JzRHIPipeline.h"
#include "JzRE/Runtime/Resource/JzResource.h"
#include "JzRE/Runtime/Resource/JzShaderVariantKey.h"

namespace JzRE {

/**
 * @brief Shader compile/load state.
 */
enum class JzEShaderCompileStatus {
    NotLoaded,
    Loading,
    Compiling,
    Compiled,
    Failed
};

/**
 * @brief One keyword bit definition in cooked manifest.
 */
struct JzShaderKeywordDefinition {
    String name;
    U32    bit = 0;
};

/**
 * @brief One shader stage payload reference for a backend target.
 */
struct JzShaderStageData {
    JzEShaderProgramType  stage   = JzEShaderProgramType::Vertex;
    JzEShaderBinaryFormat format  = JzEShaderBinaryFormat::GLSL;
    U32                   chunkId = 0;
    String                entryPoint{"main"};
    String                reflectionKey;
};

/**
 * @brief Backend-specific stage selection for one variant.
 */
struct JzShaderTargetData {
    JzERHIType                          rhiType = JzERHIType::Unknown;
    std::vector<JzShaderStageData> stages;
};

/**
 * @brief Variant record loaded from cooked manifest.
 */
struct JzShaderVariantData {
    U64                                  keywordMask = 0;
    String                               vertexLayoutName{"default"};
    JzRenderState                        renderState;
    std::vector<JzShaderTargetData> targets;
};

/**
 * @brief Runtime shader asset that loads cooked manifest + blob artifacts.
 */
class JzShader : public JzResource {
public:
    /**
     * @brief Construct from manifest path (".jzshader" or stem path).
     */
    explicit JzShader(const String &shaderPath);

    /**
     * @brief Destructor.
     */
    ~JzShader() override;

    /**
     * @brief Load manifest and blob, then build default variant.
     */
    Bool Load() override;

    /**
     * @brief Release loaded blob and variant cache.
     */
    void Unload() override;

    /**
     * @brief Get the default pipeline variant (keyword mask == 0).
     */
    std::shared_ptr<JzRHIPipeline> GetMainVariant() const
    {
        return m_mainVariant;
    }

    /**
     * @brief Get or build pipeline variant by keyword bitmask.
     */
    std::shared_ptr<JzRHIPipeline> GetVariant(U64 keywordMask);

    /**
     * @brief Get or build pipeline variant by variant key.
     */
    std::shared_ptr<JzRHIPipeline> GetVariant(const JzShaderVariantKey &key)
    {
        return GetVariant(key.keywordMask);
    }

    /**
     * @brief Compatibility overload from defines map to keyword bitmask.
     */
    std::shared_ptr<JzRHIPipeline> GetVariant(const std::unordered_map<String, String> &defines)
    {
        return GetVariant(BuildKeywordMask(defines));
    }

    /**
     * @brief Build backend shader program descriptors for one variant.
     */
    std::vector<JzShaderProgramDesc> GetBackendProgramDesc(JzERHIType rhiType, U64 keywordMask) const;

    /**
     * @brief Compute keyword mask from define map (non-zero => enabled).
     */
    U64 BuildKeywordMask(const std::unordered_map<String, String> &defines) const;

    /**
     * @brief Get loaded keyword table.
     */
    const std::vector<JzShaderKeywordDefinition> &GetKeywords() const
    {
        return m_keywords;
    }

    /**
     * @brief Get all loaded variant records.
     */
    const std::vector<JzShaderVariantData> &GetVariants() const
    {
        return m_variants;
    }

    /**
     * @brief Get reflection layout table keyed by reflectionKey.
     */
    const std::unordered_map<String, JzShaderLayoutDesc> &GetReflectionLayouts() const
    {
        return m_reflectionLayouts;
    }

    /**
     * @brief Current compile/load state.
     */
    JzEShaderCompileStatus GetCompileStatus() const
    {
        return m_compileStatus;
    }

    /**
     * @brief Load/compile log.
     */
    const String &GetCompileLog() const
    {
        return m_compileLog;
    }

    /**
     * @brief Whether shader is loaded and default variant is available.
     */
    Bool IsCompiled() const
    {
        return m_compileStatus == JzEShaderCompileStatus::Compiled;
    }

    /**
     * @brief Check whether cooked files changed on disk.
     */
    Bool NeedsReload() const;

    /**
     * @brief Reload cooked files and clear cached variants.
     */
    Bool Reload();

    /**
     * @brief Manifest + blob paths used by this asset.
     */
    const std::vector<String> &GetDependentFiles() const
    {
        return m_dependentFiles;
    }

private:
    struct JzBlobChunkRecord {
        U32 offset = 0;
        U32 size   = 0;
    };

    Bool LoadManifest();
    Bool LoadBlob();

    const JzShaderVariantData *FindVariant(U64 keywordMask) const;
    const JzShaderTargetData  *FindTarget(const JzShaderVariantData &variant, JzERHIType rhiType) const;

    Bool                         BuildVariantPipeline(U64 keywordMask, std::shared_ptr<JzRHIPipeline> &outPipeline) const;
    Bool                         BuildProgramDesc(const JzShaderStageData &stageData, JzShaderProgramDesc &outDesc) const;
    Bool                         BuildPipelineShaderLayout(const JzShaderTargetData &target, JzShaderLayoutDesc &outLayout) const;
    Bool                         MergeLayoutResource(JzShaderLayoutDesc &layout, const JzShaderResourceBindingDesc &resource, const String &layoutKey) const;
    static JzEShaderResourceType ParseShaderResourceType(const String &name);

    static JzERHIType            ParseRhiType(const String &name);
    static JzEShaderProgramType  ParseShaderStage(const String &name);
    static JzEShaderBinaryFormat ParseShaderFormat(const String &name);

private:
    String m_manifestPath;
    String m_blobPath;

    std::vector<String> m_dependentFiles;

    std::filesystem::file_time_type m_manifestLastModified{};
    std::filesystem::file_time_type m_blobLastModified{};

    std::vector<JzShaderKeywordDefinition> m_keywords;
    std::unordered_map<String, U32>        m_keywordBits;

    std::unordered_map<String, JzShaderLayoutDesc> m_reflectionLayouts;
    std::unordered_map<String, JzVertexLayoutDesc> m_vertexLayouts;
    std::vector<JzShaderVariantData>          m_variants;

    std::vector<U8>                            m_blobData;
    std::unordered_map<U32, JzBlobChunkRecord> m_blobChunkTable;

    mutable std::unordered_map<U64, std::shared_ptr<JzRHIPipeline>> m_compiledVariants;
    std::shared_ptr<JzRHIPipeline>                                  m_mainVariant;

    JzEShaderCompileStatus m_compileStatus = JzEShaderCompileStatus::NotLoaded;
    String                 m_compileLog;
};

} // namespace JzRE
