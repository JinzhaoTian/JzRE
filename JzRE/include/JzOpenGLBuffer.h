#pragma once

#include "CommonTypes.h"
#include "JzRHIBuffer.h"
#include "JzRHIETypes.h"

namespace JzRE {
/**
 * OpenGL缓冲区实现
 */
class JzOpenGLBuffer : public JzRHIBuffer {
public:
    JzOpenGLBuffer(const JzBufferDesc &desc);
    ~JzOpenGLBuffer() override;

    void  UpdateData(const void *data, Size size, Size offset = 0) override;
    void *MapBuffer() override;
    void  UnmapBuffer() override;

    GLuint GetHandle() const;

private:
    GLuint handle = 0;
    GLenum target;
    GLenum usage;
};
} // namespace JzRE