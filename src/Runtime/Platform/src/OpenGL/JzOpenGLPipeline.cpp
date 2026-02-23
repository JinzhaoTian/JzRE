/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Platform/OpenGL/JzOpenGLPipeline.h"

#include <type_traits>

JzRE::JzOpenGLPipeline::JzOpenGLPipeline(const JzRE::JzPipelineDesc &desc) :
    JzRE::JzRHIPipeline(desc)
{
    // Create OpenGL program object
    m_program = glCreateProgram();

    // Create and add all shaders
    for (const auto &shaderDesc : desc.shaders) {
        auto shader = std::make_shared<JzOpenGLShader>(shaderDesc);
        if (shader->IsCompiled()) {
            m_shaders.push_back(shader);
            glAttachShader(m_program, shader->GetHandle());
        }
    }

    // Try to link program
    LinkProgram();
}

JzRE::JzOpenGLPipeline::~JzOpenGLPipeline()
{
    if (m_program != 0) {
        // Detach all shaders
        for (const auto &shader : m_shaders) {
            glDetachShader(m_program, shader->GetHandle());
        }
        // Delete program object
        glDeleteProgram(m_program);
    }
}

GLuint JzRE::JzOpenGLPipeline::GetProgram() const
{
    return m_program;
}

JzRE::Bool JzRE::JzOpenGLPipeline::IsLinked() const
{
    return m_isLinked;
}

const JzRE::String &JzRE::JzOpenGLPipeline::GetLinkLog() const
{
    return m_linkLog;
}

void JzRE::JzOpenGLPipeline::CommitParameters()
{
    if (!m_isLinked || !HasDirtyParameters()) {
        return;
    }

    glUseProgram(m_program);

    for (const auto &[name, value] : GetParameterCache()) {
        const GLint location = GetUniformLocation(name);
        if (location == -1) {
            continue;
        }

        std::visit(
            [location](const auto &typedValue) {
                using TValue = std::decay_t<decltype(typedValue)>;
                if constexpr (std::is_same_v<TValue, I32>) {
                    glUniform1i(location, typedValue);
                } else if constexpr (std::is_same_v<TValue, F32>) {
                    glUniform1f(location, typedValue);
                } else if constexpr (std::is_same_v<TValue, JzVec2>) {
                    glUniform2fv(location, 1, typedValue.Data());
                } else if constexpr (std::is_same_v<TValue, JzVec3>) {
                    glUniform3fv(location, 1, typedValue.Data());
                } else if constexpr (std::is_same_v<TValue, JzVec4>) {
                    glUniform4fv(location, 1, typedValue.Data());
                } else if constexpr (std::is_same_v<TValue, JzMat3>) {
                    glUniformMatrix3fv(location, 1, GL_TRUE, typedValue.Data());
                } else if constexpr (std::is_same_v<TValue, JzMat4>) {
                    glUniformMatrix4fv(location, 1, GL_TRUE, typedValue.Data());
                }
            },
            value);
    }

    MarkParametersCommitted();
}

JzRE::Bool JzRE::JzOpenGLPipeline::LinkProgram()
{
    if (m_program == 0) {
        m_linkLog = "Error: Program handle is invalid";
        return false;
    }

    // Link program
    glLinkProgram(m_program);

    // Check link status
    GLint linkStatus;
    glGetProgramiv(m_program, GL_LINK_STATUS, &linkStatus);
    m_isLinked = (linkStatus == GL_TRUE);

    // Get link log
    GLint logLength;
    glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        std::vector<char> log(logLength);
        glGetProgramInfoLog(m_program, logLength, nullptr, log.data());
        m_linkLog = String(log.data());
    }

    // If link failed, add error information
    if (!m_isLinked) {
        if (m_linkLog.empty()) {
            m_linkLog = "Program linking failed with unknown error";
        }
    }

    return m_isLinked;
}

GLint JzRE::JzOpenGLPipeline::GetUniformLocation(const JzRE::String &name)
{
    // If program is not linked, return -1
    if (!m_isLinked) {
        return -1;
    }

    // First check cache
    auto it = m_uniformLocations.find(name);
    if (it != m_uniformLocations.end()) {
        return it->second;
    }

    // Get uniform location
    GLint location = glGetUniformLocation(m_program, name.c_str());

    // Cache result (even if it is -1, cache it to avoid repeated queries for non-existent uniform)
    m_uniformLocations[name] = location;

    return location;
}
