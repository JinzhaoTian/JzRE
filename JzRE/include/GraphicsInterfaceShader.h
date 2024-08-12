#pragma once

#include "CommonTypes.h"

namespace JzRE {
class GraphicsInterfaceShader {
public:
    GraphicsInterfaceShader();
    GraphicsInterfaceShader(GraphicsInterfaceShader&& other) noexcept;
    ~GraphicsInterfaceShader();

    GraphicsInterfaceShader& operator=(GraphicsInterfaceShader&& other) noexcept;

    Bool LoadFromFile(const String &vertexPath, const String &fragmentPath);
    void Use() const;

    void SetUniform(const String &name, int value) const;
    void SetUniform(const String &name, float value) const;
    void SetUniform(const String &name, const glm::vec3 &vec) const;
    void SetUniform(const String &name, const glm::mat4 &mat) const;

private:
    GLuint programID;

    GraphicsInterfaceShader(const GraphicsInterfaceShader&) = delete;
    GraphicsInterfaceShader& operator=(const GraphicsInterfaceShader&) = delete;

    String ReadFile(const String &path) const;
    Bool CompileShader(const String &source, GLenum shaderType, GLuint &shaderID);
};
} // namespace JzRE