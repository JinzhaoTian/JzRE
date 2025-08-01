#pragma once

#include "CommonTypes.h"
#include "JzOpenGLShader.h"
#include "JzRHIETypes.h"
#include "JzRHIPipeline.h"

namespace JzRE {
/**
 * OpenGL渲染管线实现
 */
class JzOpenGLPipeline : public JzRHIPipeline {
public:
    JzOpenGLPipeline(const JzPipelineDesc &desc);
    ~JzOpenGLPipeline() override;

    GLuint        GetProgram() const;
    Bool          IsLinked() const;
    const String &GetLinkLog() const;

    // Uniform设置接口
    void SetUniform(const String &name, I32 value);
    void SetUniform(const String &name, F32 value);
    void SetUniform(const String &name, const glm::vec2 &value);
    void SetUniform(const String &name, const glm::vec3 &value);
    void SetUniform(const String &name, const glm::vec4 &value);
    void SetUniform(const String &name, const glm::mat3 &value);
    void SetUniform(const String &name, const glm::mat4 &value);

private:
    Bool  LinkProgram();
    GLint GetUniformLocation(const String &name);

private:
    GLuint                                       program  = 0;
    Bool                                         isLinked = false;
    String                                       linkLog;
    std::vector<std::shared_ptr<JzOpenGLShader>> shaders;
    std::unordered_map<String, GLint>            uniformLocations;
};

} // namespace JzRE