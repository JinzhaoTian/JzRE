#pragma once

#include "CommonTypes.h"
#include "JzRHIBuffer.h"
#include "JzRHIVertexArray.h"

namespace JzRE {
/**
 * @brief OpenGL Vertex Array Object Implementation
 */
class JzOpenGLVertexArray : public JzRHIVertexArray {
public:
    JzOpenGLVertexArray(const String &debugName = "");
    ~JzOpenGLVertexArray() override;

    void BindVertexBuffer(std::shared_ptr<JzRHIBuffer> buffer, U32 binding = 0) override;
    void BindIndexBuffer(std::shared_ptr<JzRHIBuffer> buffer) override;
    void SetVertexAttribute(U32 index, U32 size, U32 stride, U32 offset) override;

    GLuint GetHandle() const;

private:
    GLuint                                    handle = 0;
    std::vector<std::shared_ptr<JzRHIBuffer>> vertexBuffers;
    std::shared_ptr<JzRHIBuffer>              indexBuffer;
};
} // namespace JzRE