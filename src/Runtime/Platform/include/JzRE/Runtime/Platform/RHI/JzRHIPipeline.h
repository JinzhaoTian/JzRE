/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <vector>
#include <unordered_map>
#include <utility>
#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzVector.h"
#include "JzRE/Runtime/Core/JzMatrix.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUShaderProgramObject.h"
#include "JzRE/Runtime/Platform/RHI/JzShaderParameter.h"

namespace JzRE {

/**
 * @brief Blend mode
 */
enum class JzEBlendMode : U8 {
    None,
    Alpha,
    Additive,
    Multiply
};

/**
 * @brief Depth test function
 */
enum class JzEDepthFunc : U8 {
    Never,
    Less,
    Equal,
    LessEqual,
    Greater,
    NotEqual,
    GreaterEqual,
    Always
};

/**
 * @brief Face culling mode
 */
enum class JzECullMode : U8 {
    None,
    Front,
    Back,
    FrontAndBack
};

/**
 * @brief MSAA level
 */
enum class JzEMSAALevel : U8 {
    None = 0,
    X2   = 2,
    X4   = 4,
    X8   = 8,
    X16  = 16
};

/**
 * @brief Render state description
 */
struct JzRenderState {
    JzEBlendMode blendMode  = JzEBlendMode::None;
    JzEDepthFunc depthFunc  = JzEDepthFunc::Less;
    JzECullMode  cullMode   = JzECullMode::Back;
    Bool         depthTest  = true;
    Bool         depthWrite = true;
    Bool         wireframe  = false;
    JzEMSAALevel msaaLevel  = JzEMSAALevel::None;
};

/**
 * @brief Pipeline description
 */
struct JzPipelineDesc {
    std::vector<JzShaderProgramDesc> shaders;
    JzRenderState                    renderState;
    String                           debugName;
};

/**
 * @brief Interface of RHI Pipeline
 */
class JzRHIPipeline : public JzGPUResource {
public:
    /**
     * @brief Constructor
     * @param desc The description of the pipeline
     */
    JzRHIPipeline(const JzPipelineDesc &desc) :
        JzGPUResource(desc.debugName), desc(desc) { }

    /**
     * @brief Destructor
     */
    virtual ~JzRHIPipeline() = default;

    /**
     * @brief Get the render state
     * @return The render state
     */
    const JzRenderState &GetRenderState() const
    {
        return desc.renderState;
    }

    /**
     * @brief Set a uniform value
     *
     * @param name The name of the uniform
     * @param value The value to set
     */
    void SetUniform(const String &name, I32 value)
    {
        SetParameter(name, value);
    }

    /**
     * @brief Set a uniform value
     *
     * @param name The name of the uniform
     * @param value The value to set
     */
    void SetUniform(const String &name, F32 value)
    {
        SetParameter(name, value);
    }

    /**
     * @brief Set a uniform value
     *
     * @param name The name of the uniform
     * @param value The value to set
     */
    void SetUniform(const String &name, const JzVec2 &value)
    {
        SetParameter(name, value);
    }

    /**
     * @brief Set a uniform value
     *
     * @param name The name of the uniform
     * @param value The value to set
     */
    void SetUniform(const String &name, const JzVec3 &value)
    {
        SetParameter(name, value);
    }

    /**
     * @brief Set a uniform value
     *
     * @param name The name of the uniform
     * @param value The value to set
     */
    void SetUniform(const String &name, const JzVec4 &value)
    {
        SetParameter(name, value);
    }

    /**
     * @brief Set a uniform value
     *
     * @param name The name of the uniform
     * @param value The value to set
     */
    void SetUniform(const String &name, const JzMat3 &value)
    {
        SetParameter(name, value);
    }

    /**
     * @brief Set a uniform value
     *
     * @param name The name of the uniform
     * @param value The value to set
     */
    void SetUniform(const String &name, const JzMat4 &value)
    {
        SetParameter(name, value);
    }

    /**
     * @brief Commit cached uniform values to backend state.
     */
    virtual void CommitParameters() = 0;

    /**
     * @brief Whether any cached parameter changed since last commit.
     */
    Bool HasDirtyParameters() const
    {
        return m_parametersDirty;
    }

    /**
     * @brief Get cached parameter map.
     */
    const std::unordered_map<String, JzShaderParameterValue> &GetParameterCache() const
    {
        return m_parameterCache;
    }

protected:
    template <typename TValue>
    void SetParameter(const String &name, TValue &&value)
    {
        m_parameterCache[name] = std::forward<TValue>(value);
        m_parametersDirty      = true;
    }

    void MarkParametersCommitted()
    {
        m_parametersDirty = false;
    }

    JzPipelineDesc desc;
    std::unordered_map<String, JzShaderParameterValue> m_parameterCache;
    Bool                                                m_parametersDirty = false;
};
} // namespace JzRE
