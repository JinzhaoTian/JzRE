#pragma once

#include "CommonTypes.h"
#include "JzMatrix.h"
#include "JzRHIDescription.h"
#include "JzRHIETypes.h"
#include "JzRHIResource.h"
#include "JzVector.h"

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