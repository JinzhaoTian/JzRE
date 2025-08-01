#include "JzOpenGLPipeline.h"

JzRE::JzOpenGLPipeline::JzOpenGLPipeline(const JzRE::JzPipelineDesc &desc) :
    JzRE::JzRHIPipeline(desc) { }

JzRE::JzOpenGLPipeline::~JzOpenGLPipeline() { }

GLuint JzRE::JzOpenGLPipeline::GetProgram() const
{
    return program;
}

JzRE::Bool JzRE::JzOpenGLPipeline::IsLinked() const
{
    return isLinked;
}

const JzRE::String &JzRE::JzOpenGLPipeline::GetLinkLog() const
{
    return linkLog;
}

void JzRE::JzOpenGLPipeline::SetUniform(const JzRE::String &name, I32 value) { }

void JzRE::JzOpenGLPipeline::SetUniform(const JzRE::String &name, F32 value) { }

void JzRE::JzOpenGLPipeline::SetUniform(const JzRE::String &name, const glm::vec2 &value) { }

void JzRE::JzOpenGLPipeline::SetUniform(const JzRE::String &name, const glm::vec3 &value) { }

void JzRE::JzOpenGLPipeline::SetUniform(const JzRE::String &name, const glm::vec4 &value) { }

void JzRE::JzOpenGLPipeline::SetUniform(const JzRE::String &name, const glm::mat3 &value) { }

void JzRE::JzOpenGLPipeline::SetUniform(const JzRE::String &name, const glm::mat4 &value) { }

JzRE::Bool JzRE::JzOpenGLPipeline::LinkProgram()
{
    return false;
}

GLint JzRE::JzOpenGLPipeline::GetUniformLocation(const JzRE::String &name) { }