#include "JzRHIBuffer.h"

JzRE::JzRHIBuffer::JzRHIBuffer(const JzRE::JzBufferDesc &desc) :
    JzRHIResource(desc.debugName), desc(desc) { }

JzRE::JzEBufferType JzRE::JzRHIBuffer::GetType() const
{
    return desc.type;
}

JzRE::JzEBufferUsage JzRE::JzRHIBuffer::GetUsage() const
{
    return desc.usage;
}

JzRE::Size JzRE::JzRHIBuffer::GetSize() const
{
    return desc.size;
}