#include "JzOpenGLVertexArray.h"
#include "JzOpenGLBuffer.h"

JzRE::JzOpenGLVertexArray::JzOpenGLVertexArray(const JzRE::String &debugName) :
    JzRHIVertexArray(debugName)
{
    // Generate OpenGL vertex array object
    glGenVertexArrays(1, &m_handle);

    // If debug name is provided, set OpenGL object label (if supported)
    if (!debugName.empty() && glObjectLabel) {
        glObjectLabel(GL_VERTEX_ARRAY, m_handle, -1, debugName.c_str());
    }
}

JzRE::JzOpenGLVertexArray::~JzOpenGLVertexArray()
{
    // Delete OpenGL vertex array object
    if (m_handle != 0) {
        glDeleteVertexArrays(1, &m_handle);
        m_handle = 0;
    }
}

void JzRE::JzOpenGLVertexArray::BindVertexBuffer(std::shared_ptr<JzRE::JzRHIBuffer> buffer, U32 binding)
{
    if (!buffer) {
        return;
    }

    // Ensure buffer is a vertex buffer type
    if (buffer->GetType() != JzEBufferType::Vertex) {
        return;
    }

    // Get OpenGL buffer handle
    auto  *openglBuffer = static_cast<JzOpenGLBuffer *>(buffer.get());
    GLuint bufferHandle = openglBuffer->GetHandle();

    // Bind current VAO
    glBindVertexArray(m_handle);

    // Bind vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, bufferHandle);

    // Store buffer reference
    if (binding >= m_vertexBuffers.size()) {
        m_vertexBuffers.resize(binding + 1);
    }
    m_vertexBuffers[binding] = buffer;
}

void JzRE::JzOpenGLVertexArray::BindIndexBuffer(std::shared_ptr<JzRE::JzRHIBuffer> buffer)
{
    if (!buffer) {
        return;
    }

    // Ensure buffer is an index buffer type
    if (buffer->GetType() != JzEBufferType::Index) {
        return;
    }

    // Get OpenGL buffer handle
    auto  *openglBuffer = static_cast<JzOpenGLBuffer *>(buffer.get());
    GLuint bufferHandle = openglBuffer->GetHandle();

    // Bind current VAO
    glBindVertexArray(m_handle);

    // Bind index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferHandle);

    // Store buffer reference
    m_indexBuffer = buffer;
}

void JzRE::JzOpenGLVertexArray::SetVertexAttribute(JzRE::U32 index, JzRE::U32 size, JzRE::U32 stride, JzRE::U32 offset)
{
    // Bind current VAO
    glBindVertexArray(m_handle);

    // Enable vertex attribute
    glEnableVertexAttribArray(index);

    // Set vertex attribute pointer
    // Assume data type is GL_FLOAT, which is the most common case
    // In a more complete implementation, the data type may need to be passed as a parameter
    glVertexAttribPointer(
        index,                           // Attribute index
        size,                            // Number of components per vertex (1, 2, 3, or 4)
        GL_FLOAT,                        // Data type
        GL_FALSE,                        // Whether to normalize
        stride,                          // Stride (bytes)
        reinterpret_cast<void *>(offset) // Offset
    );
}

GLuint JzRE::JzOpenGLVertexArray::GetHandle() const
{
    return m_handle;
}