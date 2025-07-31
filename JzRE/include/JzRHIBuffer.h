#pragma once

#include "CommonTypes.h"
#include "JzRHIETypes.h"
#include "JzRHIResource.h"

namespace JzRE {
/**
 * RHI缓冲区接口
 */
class JzRHIBuffer : public JzRHIResource {
public:
    JzRHIBuffer(const JzBufferDesc &desc) :
        JzRHIResource(desc.debugName), desc(desc) { }
    virtual ~JzRHIBuffer() = default;

    virtual void  UpdateData(const void *data, Size size, Size offset = 0) = 0;
    virtual void *MapBuffer()                                              = 0;
    virtual void  UnmapBuffer()                                            = 0;

    JzEBufferType GetType() const
    {
        return desc.type;
    }
    JzEBufferUsage GetUsage() const
    {
        return desc.usage;
    }
    Size GetSize() const
    {
        return desc.size;
    }

protected:
    JzBufferDesc desc;
};
}