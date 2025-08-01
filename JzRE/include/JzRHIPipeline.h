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
    JzRHIPipeline(const JzPipelineDesc &desc) :
        JzRHIResource(desc.debugName), desc(desc) { }
    virtual ~JzRHIPipeline() = default;

    const JzRenderState &GetRenderState() const
    {
        return desc.renderState;
    }

protected:
    JzPipelineDesc desc;
};
}