/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Core/JzRETypes.h"
#include "JzRE/Core/JzVector.h"
#include "JzRE/Core/JzMatrix.h"
#include "JzRE/RHI/JzRHIDescription.h"
#include "JzRE/RHI/JzRHIETypes.h"
#include "JzRE/RHI/JzRHIResource.h"

namespace JzRE {
/**
 * @brief Interface of RHI Pipeline
 */
class JzRHIPipeline : public JzRHIResource {
public:
    /**
     * @brief Constructor
     * @param desc The description of the pipeline
     */
    JzRHIPipeline(const JzPipelineDesc &desc) :
        JzRHIResource(desc.debugName), desc(desc) { }

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