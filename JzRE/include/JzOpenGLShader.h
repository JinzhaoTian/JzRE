#pragma once

#include "CommonTypes.h"
#include "JzRHIETypes.h"
#include "JzRHIShader.h"

namespace JzRE {
/**
 * OpenGL着色器实现
 */
class JzOpenGLShader : public JzRHIShader {
public:
    JzOpenGLShader(const JzShaderDesc &desc);
    ~JzOpenGLShader() override;

    GLuint GetHandle() const
    {
        return handle;
    }
    Bool IsCompiled() const
    {
        return isCompiled;
    }
    const String &GetCompileLog() const
    {
        return compileLog;
    }

private:
    GLuint handle     = 0;
    Bool   isCompiled = false;
    String compileLog;

    static GLenum ConvertShaderType(JzEShaderType type);
    Bool          CompileShader();
};
} // namespace JzRE