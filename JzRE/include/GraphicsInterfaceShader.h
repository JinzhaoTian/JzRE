#pragma once

#include "CommonTypes.h"

namespace JzRE {
class GraphcsInterfaceShader {
public:
    GraphcsInterfaceShader();
    ~GraphcsInterfaceShader();

    void Load(const String &vertexPath, const String &fragmentPath);
    void Use() const;
    void Shutdown();

    void SetUniform(const String &name, int value) const;
    void SetUniform(const String &name, float value) const;
    void SetUniform(const String &name, const glm::mat4 &mat) const;

private:
    GLuint program;

    String ReadFile(const String &path) const;
    Bool CompileShader(const String &source, GLenum shaderType, GLuint &shader) const;
};
} // namespace JzRE