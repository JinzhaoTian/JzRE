#include "JzOpenGLBuffer.h"

JzRE::JzOpenGLBuffer::JzOpenGLBuffer(const JzRE::JzBufferDesc &desc) :
    JzRE::JzRHIBuffer(desc) { }

JzRE::JzOpenGLBuffer::~JzOpenGLBuffer() { }

void JzRE::JzOpenGLBuffer::UpdateData(const void *data, JzRE::Size size, JzRE::Size offset) { }

void *JzRE::JzOpenGLBuffer::MapBuffer() { }

void JzRE::JzOpenGLBuffer::UnmapBuffer() { }

GLuint JzRE::JzOpenGLBuffer::GetHandle() const
{
    return handle;
}