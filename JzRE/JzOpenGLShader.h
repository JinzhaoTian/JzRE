#pragma once

#include "CommonTypes.h"
#include "JzRHIETypes.h"
#include "JzRHIShader.h"

namespace JzRE {
/**
 * @brief OpenGL Implementation of RHI Shader
 */
class JzOpenGLShader : public JzRHIShader {
public:
    /**
     * @brief Constructor
     * @param desc The description of the shader
     */
    JzOpenGLShader(const JzShaderDesc &desc);

    /**
     * @brief Destructor
     */
    ~JzOpenGLShader() override;

    /**
     * @brief Get the handle of the shader
     * @return The handle of the shader
     */
    GLuint GetHandle() const;

    /**
     * @brief Check if the shader is compiled
     * @return True if the shader is compiled, false otherwise
     */
    Bool IsCompiled() const;

    /**
     * @brief Get the compile log
     * @return The compile log
     */
    const String &GetCompileLog() const;

private:
    /**
     * @brief Convert shader type to OpenGL shader type
     * @param type The shader type
     * @return The OpenGL shader type
     */
    static GLenum ConvertShaderType(JzEShaderType type);

    /**
     * @brief Compile the shader
     * @return True if the shader is compiled, false otherwise
     */
    Bool CompileShader();

private:
    GLuint m_handle     = 0;
    Bool   m_isCompiled = false;
    String m_compileLog;
};
} // namespace JzRE