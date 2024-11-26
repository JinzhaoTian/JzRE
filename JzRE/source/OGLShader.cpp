#include "OGLShader.h"

namespace JzRE {
OGLShader::OGLShader() :
    programID(0) {
}

OGLShader::OGLShader(OGLShader &&other) noexcept :
    programID(other.programID) {
    other.programID = 0;
}

OGLShader::~OGLShader() {
    if (programID != 0) {
        glDeleteProgram(programID);
    }
}

OGLShader &OGLShader::operator=(OGLShader &&other) noexcept {
    if (this != &other) {
        if (programID != 0) {
            glDeleteProgram(programID);
        }

        programID = other.programID;
        other.programID = 0;
    }
    return *this;
}

Bool OGLShader::LoadFromFile(const String &vertexPath, const String &fragmentPath) {
    String vertexCode = ReadFile(vertexPath);
    String fragmentCode = ReadFile(fragmentPath);

    GLuint vertexShader, fragmentShader;
    if (!CompileShader(vertexCode, GL_VERTEX_SHADER, vertexShader)
        || !CompileShader(fragmentCode, GL_FRAGMENT_SHADER, fragmentShader)) {
        return false;
    }

    programID = glCreateProgram();
    glAttachShader(programID, vertexShader);
    glAttachShader(programID, fragmentShader);
    glLinkProgram(programID);

    GLint success;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(programID, 512, nullptr, infoLog);
        std::cerr << "Shader program linking failed: " << infoLog << std::endl;
        return false;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return true;
}

void OGLShader::Use() const {
    glUseProgram(programID);
}

Bool OGLShader::CompileShader(const String &source, GLenum shaderType, GLuint &shaderID) {
    shaderID = glCreateShader(shaderType);
    const char *sourceCStr = source.c_str();
    glShaderSource(shaderID, 1, &sourceCStr, nullptr);
    glCompileShader(shaderID);

    GLint success;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shaderID, 512, nullptr, infoLog);
        std::cerr << "Shader compilation failed: " << infoLog << std::endl;
        return false;
    }

    return true;
}

void OGLShader::SetUniform(const String &name, int value) const {
    glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
}

void OGLShader::SetUniform(const String &name, float value) const {
    glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
}

void OGLShader::SetUniform(const String &name, const glm::vec3 &vec) const {
    glUniform3fv(glGetUniformLocation(programID, name.c_str()), 1, glm::value_ptr(vec));
}

void OGLShader::SetUniform(const String &name, const glm::mat4 &mat) const {
    glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

String OGLShader::ReadFile(const String &path) const {
    std::ifstream file(path);

    if (!file.is_open()) {
        std::cerr << "Could not open file: " << path << std::endl;
        return "";
    }

    std::stringstream stream;
    stream << file.rdbuf();
    file.close();

    return stream.str();
}

} // namespace JzRE