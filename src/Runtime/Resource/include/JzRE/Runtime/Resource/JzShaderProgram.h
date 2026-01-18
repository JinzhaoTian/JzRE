/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <vector>
#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Platform/JzRHIPipeline.h"
#include "JzRE/Runtime/Resource/JzShaderVariantKey.h"

namespace JzRE {

/**
 * @brief Definition of a shader keyword for variants.
 *
 * Keywords are used to create different variants of the same shader.
 * Each keyword maps to a preprocessor #define in the shader source.
 */
struct JzShaderKeyword {
    String name;           ///< Keyword name (e.g., "SKINNED", "SHADOWS")
    U32    index;          ///< Bit index in variant key (0-63)
    Bool   defaultEnabled; ///< Whether enabled by default
};

/**
 * @brief Represents a shader program with multiple possible variants.
 *
 * A shader program contains the source code for different shader stages
 * (vertex, fragment, etc.) and a list of keywords that can be used to
 * create different variants.
 */
class JzShaderProgram {
public:
    /**
     * @brief Construct a shader program.
     * @param name Unique name for this shader program.
     */
    explicit JzShaderProgram(const String &name);

    /**
     * @brief Get the program name.
     */
    const String &GetName() const
    {
        return m_name;
    }

    /**
     * @brief Set the vertex shader source.
     */
    void SetVertexSource(const String &source)
    {
        m_vertexSource = source;
    }

    /**
     * @brief Set the fragment shader source.
     */
    void SetFragmentSource(const String &source)
    {
        m_fragmentSource = source;
    }

    /**
     * @brief Set the geometry shader source (optional).
     */
    void SetGeometrySource(const String &source)
    {
        m_geometrySource = source;
    }

    /**
     * @brief Get the vertex shader source.
     */
    const String &GetVertexSource() const
    {
        return m_vertexSource;
    }

    /**
     * @brief Get the fragment shader source.
     */
    const String &GetFragmentSource() const
    {
        return m_fragmentSource;
    }

    /**
     * @brief Get the geometry shader source.
     */
    const String &GetGeometrySource() const
    {
        return m_geometrySource;
    }

    /**
     * @brief Check if this program has a geometry shader.
     */
    Bool HasGeometryShader() const
    {
        return !m_geometrySource.empty();
    }

    /**
     * @brief Add a keyword for variants.
     * @param keyword The keyword definition.
     */
    void AddKeyword(const JzShaderKeyword &keyword);

    /**
     * @brief Get all keywords.
     */
    const std::vector<JzShaderKeyword> &GetKeywords() const
    {
        return m_keywords;
    }

    /**
     * @brief Get keyword index by name.
     * @param name Keyword name.
     * @return Keyword index, or -1 if not found.
     */
    I32 GetKeywordIndex(const String &name) const;

    /**
     * @brief Set the render state for pipelines created from this program.
     */
    void SetRenderState(const JzRenderState &state)
    {
        m_renderState = state;
    }

    /**
     * @brief Get the render state.
     */
    const JzRenderState &GetRenderState() const
    {
        return m_renderState;
    }

    /**
     * @brief Build the default variant key based on keyword defaults.
     */
    JzShaderVariantKey GetDefaultVariantKey() const;

private:
    String m_name;
    String m_vertexSource;
    String m_fragmentSource;
    String m_geometrySource;

    std::vector<JzShaderKeyword> m_keywords;
    JzRenderState                m_renderState;
};

} // namespace JzRE
