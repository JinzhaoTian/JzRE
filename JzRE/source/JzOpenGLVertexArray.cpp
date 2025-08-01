#include "JzOpenGLVertexArray.h"

JzRE::JzOpenGLVertexArray::JzOpenGLVertexArray(const JzRE::String &debugName) { }

JzRE::JzOpenGLVertexArray::~JzOpenGLVertexArray() { }

void JzRE::JzOpenGLVertexArray::BindVertexBuffer(std::shared_ptr<JzRE::JzRHIBuffer> buffer, U32 binding) { }

void JzRE::JzOpenGLVertexArray::BindIndexBuffer(std::shared_ptr<JzRE::JzRHIBuffer> buffer) { }

void JzRE::JzOpenGLVertexArray::SetVertexAttribute(JzRE::U32 index, JzRE::U32 size, JzRE::U32 stride, JzRE::U32 offset) { }

GLuint JzRE::JzOpenGLVertexArray::GetHandle() const
{
    return handle;
}