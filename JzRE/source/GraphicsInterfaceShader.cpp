#include "GraphicsInterfaceShader.h"

namespace JzRE {
GraphcsInterfaceShader::GraphcsInterfaceShader() :
    program(0) {
}

GraphcsInterfaceShader::~GraphcsInterfaceShader() {
    Shutdown();
}

void GraphcsInterfaceShader::Load(const String &vertexPath, const String &fragmentPath) {
}

void GraphcsInterfaceShader::Use() const {
    glUseProgram(program);
}

void GraphcsInterfaceShader::Shutdown() {
    if (program != 0) {
        glDeleteProgram(program);
        program = 0;
    }
}

void GraphcsInterfaceShader::SetUniform(const String &name, int value) const {
    glUniform1i(glGetUniformLocation(program, name.c_str()), value);
}

void GraphcsInterfaceShader::SetUniform(const String &name, float value) const {
    glUniform1f(glGetUniformLocation(program, name.c_str()), value);
}

void GraphcsInterfaceShader::SetUniform(const String &name, const glm::mat4 &mat) const {
    glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

String GraphcsInterfaceShader::ReadFile(const String &path) const {
}

Bool GraphcsInterfaceShader::CompileShader(const String &source, GLenum shaderType, GLuint &shader) const {
}
} // namespace JzRE