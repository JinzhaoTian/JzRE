#pragma once

#include "CommonTypes.h"
#include "JzRHIETypes.h"
#include "JzRHIResource.h"

namespace JzRE {
/**
 * RHI渲染管线接口
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