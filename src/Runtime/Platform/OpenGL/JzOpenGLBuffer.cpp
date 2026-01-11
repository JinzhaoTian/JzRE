/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzOpenGLBuffer.h"

JzRE::JzOpenGLBuffer::JzOpenGLBuffer(const JzRE::JzGPUBufferObjectDesc &desc) :
    JzRE::JzGPUBufferObject(desc)
{
    // Set OpenGL target based on buffer type
    switch (desc.type) {
        case JzEGPUBufferObjectType::Vertex:
            m_target = GL_ARRAY_BUFFER;
            break;
        case JzEGPUBufferObjectType::Index:
            m_target = GL_ELEMENT_ARRAY_BUFFER;
            break;
        case JzEGPUBufferObjectType::Uniform:
            m_target = GL_UNIFORM_BUFFER;
            break;
        case JzEGPUBufferObjectType::Storage:
            m_target = GL_SHADER_STORAGE_BUFFER;
            break;
        default:
            m_target = GL_ARRAY_BUFFER;
            break;
    }

    // Set OpenGL usage based on buffer usage
    switch (desc.usage) {
        case JzEGPUBufferObjectUsage::StaticDraw:
            m_usage = GL_STATIC_DRAW;
            break;
        case JzEGPUBufferObjectUsage::DynamicDraw:
            m_usage = GL_DYNAMIC_DRAW;
            break;
        case JzEGPUBufferObjectUsage::StreamDraw:
            m_usage = GL_STREAM_DRAW;
            break;
        default:
            m_usage = GL_STATIC_DRAW;
            break;
    }

    // Generate OpenGL buffer object
    glGenBuffers(1, &m_handle);

    // Bind buffer and allocate memory
    glBindBuffer(m_target, m_handle);
    glBufferData(m_target, desc.size, desc.data, m_usage);
    glBindBuffer(m_target, 0);
}

JzRE::JzOpenGLBuffer::~JzOpenGLBuffer()
{
    if (m_handle != 0) {
        glDeleteBuffers(1, &m_handle);
        m_handle = 0;
    }
}

void JzRE::JzOpenGLBuffer::UpdateData(const void *data, JzRE::Size size, JzRE::Size offset)
{
    if (data == nullptr || size == 0) {
        return;
    }

    // Bind buffer
    glBindBuffer(m_target, m_handle);

    // Update buffer data
    if (offset + size <= desc.size) {
        glBufferSubData(m_target, offset, size, data);
    } else {
        // If offset + size exceeds buffer size, reallocate the entire buffer
        glBufferData(m_target, offset + size, nullptr, m_usage);
        glBufferSubData(m_target, offset, size, data);
        desc.size = offset + size;
    }

    // Unbind buffer
    glBindBuffer(m_target, 0);
}

void *JzRE::JzOpenGLBuffer::MapBuffer()
{
    // Bind buffer
    glBindBuffer(m_target, m_handle);

    // Map buffer to memory, allow read and write access
    void *mappedPtr = glMapBuffer(m_target, GL_READ_WRITE);

    // Keep buffer bound until UnmapBuffer is called
    return mappedPtr;
}

void JzRE::JzOpenGLBuffer::UnmapBuffer()
{
    // Unmap buffer
    glUnmapBuffer(m_target);

    // Unbind buffer
    glBindBuffer(m_target, 0);
}

GLuint JzRE::JzOpenGLBuffer::GetHandle() const
{
    return m_handle;
}