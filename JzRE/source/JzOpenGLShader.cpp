#include "JzOpenGLShader.h"

JzRE::JzOpenGLShader::JzOpenGLShader(const JzRE::JzShaderDesc &desc) :
    JzRE::JzRHIShader(desc) { }

JzRE::JzOpenGLShader::~JzOpenGLShader() { }

GLuint JzRE::JzOpenGLShader::GetHandle() const
{
    return handle;
}

JzRE::Bool JzRE::JzOpenGLShader::IsCompiled() const
{
    return isCompiled;
}

const JzRE::String &JzRE::JzOpenGLShader::GetCompileLog() const
{
    return compileLog;
}

GLenum JzRE::JzOpenGLShader::ConvertShaderType(JzRE::JzEShaderType type)
{
    return 0;
}

JzRE::Bool JzRE::JzOpenGLShader::CompileShader()
{
    return false;
}