#pragma once

#include "CommonTypes.h"
#include "JzRHIETypes.h"
#include "JzRHIResource.h"

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

protected:
    JzPipelineDesc desc;
};
}