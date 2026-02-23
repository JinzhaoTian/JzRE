/**
 * @file    JzShaderRegistry.h
 * @brief   Specialized shader registry with compiler abstraction and dependency tracking
 * @author  Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <filesystem>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Resource/JzAssetHandle.h"
#include "JzRE/Runtime/Resource/JzShaderAsset.h"

namespace JzRE {

// Forward declarations
class JzRHIPipeline;
class JzShaderVariant;
class JzDevice;

/**
 * @brief Abstract shader compiler interface
 *
 * Platform-specific implementations (OpenGL, Vulkan, DirectX) should
 * inherit from this interface.
 */
class IShaderCompiler {
public:
    virtual ~IShaderCompiler() = default;

    /**
     * @brief Compile shader source into a pipeline
     *
     * @param source Shader source data (vertex, fragment, etc.)
     * @param config Compilation configuration
     * @param defines Additional preprocessor defines
     * @param[out] pipeline The resulting compiled pipeline
     * @param[out] log Compilation log (errors/warnings)
     * @return true if compilation succeeded
     */
    virtual Bool Compile(const JzShaderSourceData                 &source,
                         const JzShaderCompileConfig              &config,
                         const std::unordered_map<String, String> &defines,
                         std::shared_ptr<JzRHIPipeline>           &pipeline,
                         String                                   &log) = 0;

    /**
     * @brief Get the compiler name for debugging
     */
    virtual const char *GetName() const = 0;
};

/**
 * @brief OpenGL shader compiler implementation
 */
class JzOpenGLShaderCompiler : public IShaderCompiler {
public:
    explicit JzOpenGLShaderCompiler(JzDevice &device);

    Bool Compile(const JzShaderSourceData                 &source,
                 const JzShaderCompileConfig              &config,
                 const std::unordered_map<String, String> &defines,
                 std::shared_ptr<JzRHIPipeline>           &pipeline,
                 String                                   &log) override;

    const char *GetName() const override
    {
        return "OpenGL";
    }

private:
    JzDevice &m_device;
};

/**
 * @brief Vulkan shader compiler implementation.
 */
class JzVulkanShaderCompiler : public IShaderCompiler {
public:
    explicit JzVulkanShaderCompiler(JzDevice &device);

    Bool Compile(const JzShaderSourceData                 &source,
                 const JzShaderCompileConfig              &config,
                 const std::unordered_map<String, String> &defines,
                 std::shared_ptr<JzRHIPipeline>           &pipeline,
                 String                                   &log) override;

    const char *GetName() const override
    {
        return "Vulkan";
    }

private:
    JzDevice &m_device;
};

/**
 * @brief Specialized shader registry with advanced features
 *
 * Extends basic asset registry functionality with:
 * - Platform-specific compiler abstraction
 * - Include file dependency tracking
 * - Shader variant compilation
 * - Hot reload checking
 * - Precompilation of common shaders
 *
 * @note This class does not inherit from JzAssetRegistry<JzShaderAsset>
 *       but works alongside the JzAssetManager to provide shader-specific
 *       functionality.
 */
class JzShaderRegistry {
public:
    /**
     * @brief Construct shader registry
     * @param device Reference to the RHI device for shader compilation
     */
    explicit JzShaderRegistry(JzDevice &device);

    /**
     * @brief Destructor
     */
    ~JzShaderRegistry();

    // Non-copyable
    JzShaderRegistry(const JzShaderRegistry &)            = delete;
    JzShaderRegistry &operator=(const JzShaderRegistry &) = delete;

    // ==================== Compiler Management ====================

    /**
     * @brief Get the current shader compiler
     */
    IShaderCompiler *GetCompiler() const
    {
        return m_compiler.get();
    }

    /**
     * @brief Set a custom shader compiler
     */
    void SetCompiler(std::unique_ptr<IShaderCompiler> compiler);

    // ==================== Variant Compilation ====================

    /**
     * @brief Compile a shader variant with specified defines
     *
     * @param shaderAsset The base shader asset
     * @param defines Preprocessor defines for this variant
     * @return Compiled shader variant, or nullptr on failure
     */
    std::shared_ptr<JzShaderVariant> CompileVariant(
        JzShaderAsset                            *shaderAsset,
        const std::unordered_map<String, String> &defines);

    // ==================== Dependency Tracking ====================

    /**
     * @brief Register an include file dependency
     *
     * @param shaderPath Path to the shader that has the dependency
     * @param includePath Path to the included file
     */
    void RegisterIncludeDependency(const String &shaderPath, const String &includePath);

    /**
     * @brief Get all shaders that depend on a given include file
     *
     * @param includePath Path to the include file
     * @return Set of shader paths that include this file
     */
    std::unordered_set<String> GetDependentShaders(const String &includePath) const;

    /**
     * @brief Clear all dependency tracking data
     */
    void ClearDependencies();

    // ==================== Hot Reload ====================

    /**
     * @brief Check all tracked shaders for file modifications
     *
     * @return List of shader paths that need reloading
     */
    std::vector<String> CheckForUpdates() const;

    /**
     * @brief Register a shader for hot reload tracking
     *
     * @param shaderPath Path to the shader file
     */
    void TrackShaderForReload(const String &shaderPath);

    /**
     * @brief Stop tracking a shader for hot reload
     *
     * @param shaderPath Path to the shader file
     */
    void UntrackShaderForReload(const String &shaderPath);

    // ==================== Precompilation ====================

    /**
     * @brief Precompile a shader with common variant combinations
     *
     * @param shaderAsset The shader to precompile
     * @param keywords List of keyword names to generate combinations for
     */
    void PrecompileCommonVariants(JzShaderAsset             *shaderAsset,
                                  const std::vector<String> &keywords);

    // ==================== Statistics ====================

    /**
     * @brief Get total number of compiled variants
     */
    [[nodiscard]] Size GetCompiledVariantCount() const
    {
        return m_compiledVariantCount;
    }

    /**
     * @brief Get number of tracked shaders for hot reload
     */
    [[nodiscard]] Size GetTrackedShaderCount() const;

private:
    /**
     * @brief Generate preprocessor defines string from map
     */
    static String GenerateDefinesString(const std::unordered_map<String, String> &defines);

    /**
     * @brief Generate all combinations of keyword defines
     */
    static std::vector<std::unordered_map<String, String>>
    GenerateKeywordCombinations(const std::vector<String> &keywords);

    JzDevice                        &m_device;
    std::unique_ptr<IShaderCompiler> m_compiler;

    // Include dependency tracking: includePath -> set of shaderPaths
    std::unordered_map<String, std::unordered_set<String>> m_includeDependencies;

    // Hot reload tracking: shaderPath -> last modification time
    std::unordered_map<String, std::filesystem::file_time_type> m_shaderModTimes;

    // Statistics
    Size m_compiledVariantCount = 0;

    mutable std::mutex m_mutex;
};

} // namespace JzRE
