/**
 * @file    JzShaderAsset.h
 * @brief   Shader asset for asset management system integration
 * @author  Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <filesystem>
#include <memory>
#include <unordered_map>
#include <vector>

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Resource/JzResource.h"
#include "JzRE/Runtime/Resource/JzShaderProgram.h"
#include "JzRE/Runtime/Resource/JzShaderVariant.h"
#include "JzRE/Runtime/Resource/JzShaderVariantKey.h"

namespace JzRE {

/**
 * @brief Shader type enumeration
 */
enum class JzEShaderStageType {
    Vertex,
    Fragment,
    Geometry,
    TessellationControl,
    TessellationEvaluation,
    Compute
};

/**
 * @brief Shader compile status enumeration
 */
enum class JzEShaderCompileStatus {
    NotLoaded, ///< Shader has not been loaded yet
    Loading,   ///< Shader source is being loaded
    Compiling, ///< Shader is being compiled
    Compiled,  ///< Shader compiled successfully
    Failed     ///< Shader compilation failed
};

/**
 * @brief Shader source container
 */
struct JzShaderSourceData {
    String vertexSource;      ///< Vertex shader source code
    String fragmentSource;    ///< Fragment shader source code
    String geometrySource;    ///< Geometry shader source code (optional)
    String tessControlSource; ///< Tessellation control shader source code (optional)
    String tessEvalSource;    ///< Tessellation evaluation shader source code (optional)
    String computeSource;     ///< Compute shader source code (optional)

    /// Preprocessor define macros
    std::unordered_map<String, String> defines;
};

/**
 * @brief Shader compilation configuration
 */
struct JzShaderCompileConfig {
    Bool                validate         = true;  ///< Validate shader after compilation
    Bool                optimize         = true;  ///< Optimize shader code
    Bool                includeDebugInfo = false; ///< Include debug information
    std::vector<String> includePaths;             ///< Additional include paths
};

/**
 * @brief Shader variant definition
 */
struct JzShaderAssetVariant {
    String                             name;    ///< Variant name
    std::unordered_map<String, String> defines; ///< Preprocessor defines for this variant
    std::shared_ptr<JzShaderVariant>   variant; ///< Compiled variant
};

/**
 * @brief Shader asset for the asset management system.
 *
 * This class represents a shader as a loadable asset, integrating with
 * the JzAssetManager system. It supports:
 * - Loading shader source from files
 * - Shader variant management
 * - Hot reload support
 * - Compile error logging
 *
 * @note This class works with JzShaderProgram internally but exposes
 *       an asset-friendly interface.
 */
class JzShaderAsset : public JzResource {
public:
    /**
     * @brief Construct a shader asset from vertex and fragment shader paths.
     *
     * @param vertexPath Path to the vertex shader file.
     * @param fragmentPath Path to the fragment shader file.
     */
    JzShaderAsset(const String &vertexPath, const String &fragmentPath);

    /**
     * @brief Construct a shader asset from a single base name.
     *
     * This constructor expects files named <baseName>.vert and <baseName>.frag
     * to exist in the same directory.
     *
     * @param baseName Base path/name for the shader (without extension).
     */
    explicit JzShaderAsset(const String &baseName);

    /**
     * @brief Destructor.
     */
    virtual ~JzShaderAsset();

    /**
     * @brief Load the shader source and compile it.
     *
     * @return Bool True if loading and compilation succeeded.
     */
    virtual Bool Load() override;

    /**
     * @brief Unload the shader, releasing resources.
     */
    virtual void Unload() override;

    // ==================== Shader Program Access ====================

    /**
     * @brief Get the internal shader program.
     *
     * @return std::shared_ptr<JzShaderProgram> The shader program.
     */
    std::shared_ptr<JzShaderProgram> GetProgram() const
    {
        return m_program;
    }

    /**
     * @brief Get the default (main) shader variant.
     *
     * @return std::shared_ptr<JzShaderVariant> The main compiled variant.
     */
    std::shared_ptr<JzShaderVariant> GetMainVariant() const
    {
        return m_mainVariant;
    }

    // ==================== Variant Management ====================

    /**
     * @brief Get or compile a shader variant with the specified defines.
     *
     * If the variant is already cached, returns the cached version.
     * Otherwise, compiles a new variant and caches it.
     *
     * @param defines Preprocessor defines for this variant.
     *
     * @return std::shared_ptr<JzShaderVariant> The compiled variant.
     */
    std::shared_ptr<JzShaderVariant> GetVariant(const std::unordered_map<String, String> &defines);

    /**
     * @brief Get or compile a shader variant with the specified variant key.
     *
     * @param key The variant key specifying enabled keywords.
     *
     * @return std::shared_ptr<JzShaderVariant> The compiled variant.
     */
    std::shared_ptr<JzShaderVariant> GetVariant(const JzShaderVariantKey &key);

    /**
     * @brief Get all cached variants.
     *
     * @return const std::vector<JzShaderAssetVariant>& List of cached variants.
     */
    const std::vector<JzShaderAssetVariant> &GetVariants() const
    {
        return m_variants;
    }

    // ==================== Status and Logging ====================

    /**
     * @brief Get the compile status.
     *
     * @return JzEShaderCompileStatus The current compile status.
     */
    JzEShaderCompileStatus GetCompileStatus() const
    {
        return m_compileStatus;
    }

    /**
     * @brief Get the compile log (errors and warnings).
     *
     * @return const String& The compile log.
     */
    const String &GetCompileLog() const
    {
        return m_compileLog;
    }

    /**
     * @brief Check if the shader is compiled and ready to use.
     *
     * @return Bool True if compiled successfully.
     */
    Bool IsCompiled() const
    {
        return m_compileStatus == JzEShaderCompileStatus::Compiled;
    }

    // ==================== Hot Reload Support ====================

    /**
     * @brief Check if the shader needs to be reloaded.
     *
     * Compares file modification times to detect changes.
     *
     * @return Bool True if any source file has been modified.
     */
    Bool NeedsReload() const;

    /**
     * @brief Reload the shader from disk.
     *
     * @return Bool True if reload succeeded.
     */
    Bool Reload();

    /**
     * @brief Get the list of dependent files (includes).
     *
     * @return const std::vector<String>& List of file paths.
     */
    const std::vector<String> &GetDependentFiles() const
    {
        return m_dependentFiles;
    }

    // ==================== Source Access ====================

    /**
     * @brief Get the shader source data.
     *
     * @return const JzShaderSourceData& The source data.
     */
    const JzShaderSourceData &GetSourceData() const
    {
        return m_sourceData;
    }

    /**
     * @brief Get the compile configuration.
     *
     * @return const JzShaderCompileConfig& The compile configuration.
     */
    const JzShaderCompileConfig &GetCompileConfig() const
    {
        return m_compileConfig;
    }

    /**
     * @brief Set the compile configuration.
     *
     * @param config The compile configuration to use.
     */
    void SetCompileConfig(const JzShaderCompileConfig &config)
    {
        m_compileConfig = config;
    }

private:
    /**
     * @brief Load shader source from file.
     */
    Bool LoadSourceFromFile(const String &path, String &outSource);

    /**
     * @brief Process #include directives in shader source.
     */
    String ProcessIncludes(const String &source, const String &basePath, std::vector<String> &outIncludes);

    /**
     * @brief Compile the shader program.
     */
    Bool CompileProgram();

    /**
     * @brief Generate variant name from defines.
     */
    String GenerateVariantName(const std::unordered_map<String, String> &defines) const;

private:
    String m_vertexPath;   ///< Path to vertex shader
    String m_fragmentPath; ///< Path to fragment shader
    String m_geometryPath; ///< Path to geometry shader (optional)

    JzShaderSourceData    m_sourceData;    ///< Shader source code
    JzShaderCompileConfig m_compileConfig; ///< Compile configuration

    std::shared_ptr<JzShaderProgram>  m_program;     ///< Internal shader program
    std::shared_ptr<JzShaderVariant>  m_mainVariant; ///< Main compiled variant
    std::vector<JzShaderAssetVariant> m_variants;    ///< Cached variants

    JzEShaderCompileStatus m_compileStatus = JzEShaderCompileStatus::NotLoaded;
    String                 m_compileLog;

    // Hot reload support
    std::filesystem::file_time_type m_vertexLastModified;
    std::filesystem::file_time_type m_fragmentLastModified;
    std::filesystem::file_time_type m_geometryLastModified;
    std::vector<String>             m_dependentFiles;
    Bool                            m_needsReload = false;
};

} // namespace JzRE
