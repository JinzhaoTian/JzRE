#pragma once

#include "CommonTypes.h"

namespace JzRE {
class OGLShader {
public:
    OGLShader();
    OGLShader(OGLShader &&other) noexcept;
    ~OGLShader();

    OGLShader &operator=(OGLShader &&other) noexcept;

    Bool LoadFromFile(const String &vertexPath, const String &fragmentPath);
    void Use() const;

    void SetUniform(const String &name, int value) const;
    void SetUniform(const String &name, float value) const;
    void SetUniform(const String &name, const glm::vec3 &vec) const;
    void SetUniform(const String &name, const glm::mat4 &mat) const;

private:
    GLuint programID;

    OGLShader(const OGLShader &) = delete;
    OGLShader &operator=(const OGLShader &) = delete;

    String ReadFile(const String &path) const;
    Bool CompileShader(const String &source, GLenum shaderType, GLuint &shaderID);
};
} // namespace JzRE