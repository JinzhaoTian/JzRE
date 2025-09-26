/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzOpenGLShader.h"

JzRE::JzOpenGLShader::JzOpenGLShader(const JzRE::JzShaderDesc &desc) :
    JzRE::JzRHIShader(desc)
{
    CompileShader();
}

JzRE::JzOpenGLShader::~JzOpenGLShader()
{
    if (m_handle != 0) {
        glDeleteShader(m_handle);
        m_handle = 0;
    }
}

GLuint JzRE::JzOpenGLShader::GetHandle() const
{
    return m_handle;
}

JzRE::Bool JzRE::JzOpenGLShader::IsCompiled() const
{
    return m_isCompiled;
}

const JzRE::String &JzRE::JzOpenGLShader::GetCompileLog() const
{
    return m_compileLog;
}

GLenum JzRE::JzOpenGLShader::ConvertShaderType(JzRE::JzEShaderType type)
{
    switch (type) {
        case JzEShaderType::Vertex:
            return GL_VERTEX_SHADER;
        case JzEShaderType::Fragment:
            return GL_FRAGMENT_SHADER;
        case JzEShaderType::Geometry:
            return GL_GEOMETRY_SHADER;
        case JzEShaderType::TessellationControl:
            return GL_TESS_CONTROL_SHADER;
        case JzEShaderType::TessellationEvaluation:
            return GL_TESS_EVALUATION_SHADER;
        case JzEShaderType::Compute:
            return GL_COMPUTE_SHADER;
        default:
            return 0;
    }
}

JzRE::Bool JzRE::JzOpenGLShader::CompileShader()
{
    // Clear previous compilation state
    if (m_handle != 0) {
        glDeleteShader(m_handle);
        m_handle = 0;
    }
    m_isCompiled = false;
    m_compileLog.clear();

    // Convert shader type
    GLenum shaderType = ConvertShaderType(desc.type);
    if (shaderType == 0) {
        m_compileLog = "Unsupported shader type";
        return false;
    }

    // Create shader object
    m_handle = glCreateShader(shaderType);
    if (m_handle == 0) {
        m_compileLog = "Failed to create shader object";
        return false;
    }

    // Set shader source code
    const char *source = desc.source.c_str();
    glShaderSource(m_handle, 1, &source, nullptr);

    // Compile shader
    glCompileShader(m_handle);

    // Check compilation status
    GLint compileStatus;
    glGetShaderiv(m_handle, GL_COMPILE_STATUS, &compileStatus);

    // Get compilation log
    GLint logLength;
    glGetShaderiv(m_handle, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 1) {
        m_compileLog.resize(logLength);
        glGetShaderInfoLog(m_handle, logLength, nullptr, m_compileLog.data());
        // Remove trailing null characters
        m_compileLog.resize(logLength - 1);
    }

    m_isCompiled = (compileStatus == GL_TRUE);

    // If compilation failed, clean up resources
    if (!m_isCompiled) {
        glDeleteShader(m_handle);
        m_handle = 0;
    }

    return m_isCompiled;
}