#pragma once

#include "CommonTypes.h"
#include "JzRHIBuffer.h"
#include "JzRHIETypes.h"
#include "JzRHIResource.h"

namespace JzRE {
/**
 * RHI顶点数组对象接口
 */
class JzRHIVertexArray : public JzRHIResource {
public:
    JzRHIVertexArray(const String &debugName = "") :
        JzRHIResource(debugName) { }
    virtual ~JzRHIVertexArray() = default;

    virtual void BindVertexBuffer(std::shared_ptr<JzRHIBuffer> buffer, U32 binding = 0) = 0;
    virtual void BindIndexBuffer(std::shared_ptr<JzRHIBuffer> buffer)                   = 0;
    virtual void SetVertexAttribute(U32 index, U32 size, U32 stride, U32 offset)        = 0;
};

} // namespace JzRE