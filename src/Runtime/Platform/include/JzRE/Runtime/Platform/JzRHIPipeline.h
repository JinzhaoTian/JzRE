/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <vector>
#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzVector.h"
#include "JzRE/Runtime/Core/JzMatrix.h"
#include "JzRE/Runtime/Platform/JzGPUShaderProgramObject.h"

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
    virtual void SetUniform(const String &name, I32 value) = 0;

    /**
     * @brief Set a uniform value
     *
     * @param name The name of the uniform
     * @param value The value to set
     */
    virtual void SetUniform(const String &name, F32 value) = 0;

    /**
     * @brief Set a uniform value
     *
     * @param name The name of the uniform
     * @param value The value to set
     */
    virtual void SetUniform(const String &name, const JzVec2 &value) = 0;

    /**
     * @brief Set a uniform value
     *
     * @param name The name of the uniform
     * @param value The value to set
     */
    virtual void SetUniform(const String &name, const JzVec3 &value) = 0;

    /**
     * @brief Set a uniform value
     *
     * @param name The name of the uniform
     * @param value The value to set
     */
    virtual void SetUniform(const String &name, const JzVec4 &value) = 0;

    /**
     * @brief Set a uniform value
     *
     * @param name The name of the uniform
     * @param value The value to set
     */
    virtual void SetUniform(const String &name, const JzMat3 &value) = 0;

    /**
     * @brief Set a uniform value
     *
     * @param name The name of the uniform
     * @param value The value to set
     */
    virtual void SetUniform(const String &name, const JzMat4 &value) = 0;

protected:
    JzPipelineDesc desc;
};
} // namespace JzRE