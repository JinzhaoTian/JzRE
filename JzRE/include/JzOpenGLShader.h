#pragma once

#include "CommonTypes.h"
#include "JzRHIETypes.h"
#include "JzRHIShader.h"

namespace JzRE {
/**
 * @brief OpenGL Shader Implementation
 */
class JzOpenGLShader : public JzRHIShader {
public:
    JzOpenGLShader(const JzShaderDesc &desc);
    ~JzOpenGLShader() override;

    GLuint        GetHandle() const;
    Bool          IsCompiled() const;
    const String &GetCompileLog() const;

private:
    static GLenum ConvertShaderType(JzEShaderType type);
    Bool          CompileShader();

private:
    GLuint handle     = 0;
    Bool   isCompiled = false;
    String compileLog;
};
} // namespace JzRE