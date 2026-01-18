/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <filesystem>
#include <memory>
#include <mutex>
#include <unordered_map>
#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Resource/JzShaderProgram.h"
#include "JzRE/Runtime/Resource/JzShaderVariant.h"
#include "JzRE/Runtime/Resource/JzShaderVariantKey.h"

namespace JzRE {

/**
 * @brief Central manager for shader programs and variants.
 *
 * Responsibilities:
 * - Register and manage shader programs
 * - Compile and cache shader variants on-demand
 * - Provide built-in engine shaders
 * - Handle cross-platform shader compilation (future: SPIRV)
 */
class JzShaderManager {
public:
    JzShaderManager()  = default;
    ~JzShaderManager() = default;

    /**
     * @brief Initialize the shader manager.
     *
     * Registers built-in engine shaders.
     */
    void Initialize();

    /**
     * @brief Shutdown and release all cached shaders.
     */
    void Shutdown();

    // ==================== Shader Program Management ====================

    /**
     * @brief Register a shader program.
     * @param name Unique name for the program.
     * @param program The shader program to register.
     */
    void RegisterShaderProgram(const String &name, std::shared_ptr<JzShaderProgram> program);

    /**
     * @brief Load and register a shader program from files.
     *
     * Loads vertex and fragment shader sources from files and registers
     * the program with the given name.
     *
     * @param name Unique name for the program.
     * @param vertexPath Path to vertex shader file (.vert).
     * @param fragmentPath Path to fragment shader file (.frag).
     * @param geometryPath Optional path to geometry shader file (.geom).
     * @return True if loading succeeded, false otherwise.
     */
    Bool LoadShaderProgram(const String                &name,
                           const std::filesystem::path &vertexPath,
                           const std::filesystem::path &fragmentPath,
                           const std::filesystem::path &geometryPath = {});

    /**
     * @brief Get a registered shader program by name.
     * @param name The program name.
     * @return Shared pointer to the program, nullptr if not found.
     */
    std::shared_ptr<JzShaderProgram> GetShaderProgram(const String &name);

    // ==================== Variant Access ====================

    /**
     * @brief Get or compile a shader variant.
     *
     * If the variant is already cached, returns the cached version.
     * Otherwise, compiles a new variant and caches it.
     *
     * @param programName The shader program name.
     * @param variantKey The variant key specifying enabled keywords.
     * @return Shared pointer to the variant, nullptr on failure.
     */
    std::shared_ptr<JzShaderVariant> GetVariant(const String &programName, const JzShaderVariantKey &variantKey = {});

    // ==================== Built-in Shaders ====================

    /**
     * @brief Get the built-in standard shader.
     * @param variantKey Optional variant key for features like SKINNED, SHADOWS.
     * @return Shared pointer to the standard shader variant.
     */
    std::shared_ptr<JzShaderVariant> GetStandardShader(const JzShaderVariantKey &variantKey = {});

    /**
     * @brief Get the built-in unlit shader.
     * @return Shared pointer to the unlit shader variant.
     */
    std::shared_ptr<JzShaderVariant> GetUnlitShader();

    // ==================== Utility ====================

    /**
     * @brief Invalidate all cached variants.
     *
     * Call this to force recompilation of all variants on next access.
     */
    void InvalidateAllVariants();

    /**
     * @brief Get statistics about cached shaders.
     */
    struct Stats {
        Size loadedPrograms;
        Size compiledVariants;
    };
    Stats GetStats() const;

private:
    /**
     * @brief Compile a variant for a shader program.
     */
    std::shared_ptr<JzShaderVariant> CompileVariant(std::shared_ptr<JzShaderProgram> program,
                                                    const JzShaderVariantKey        &variantKey);

    /**
     * @brief Generate preprocessor defines for a variant.
     */
    String GenerateDefines(std::shared_ptr<JzShaderProgram> program, const JzShaderVariantKey &variantKey);

    /**
     * @brief Generate cache key for variant lookup.
     */
    String MakeVariantCacheKey(const String &programName, const JzShaderVariantKey &variantKey);

    /**
     * @brief Register built-in engine shaders.
     */
    void RegisterBuiltInShaders();

    /**
     * @brief Read file contents as string.
     * @param path Path to file.
     * @return File contents, empty string on failure.
     */
    String ReadShaderFile(const std::filesystem::path &path);

    /**
     * @brief Get the shader directory path.
     * @return Path to shaders directory relative to engine path.
     */
    std::filesystem::path GetShaderDirectory() const;

private:
    std::unordered_map<String, std::shared_ptr<JzShaderProgram>> m_programs;
    std::unordered_map<String, std::shared_ptr<JzShaderVariant>> m_variantCache;

    mutable std::mutex m_mutex;
};

} // namespace JzRE
