#pragma once

#include "CommonTypes.h"
#include "JzOpenGLShader.h"
#include "JzRHIETypes.h"
#include "JzRHIPipeline.h"

namespace JzRE {
/**
 * @brief OpenGL Implementation of RHI Pipeline
 */
class JzOpenGLPipeline : public JzRHIPipeline {
public:
    /**
     * @brief Constructor
     *
     * @param desc The description of the pipeline
     */
    JzOpenGLPipeline(const JzPipelineDesc &desc);

    /**
     * @brief Destructor
     */
    ~JzOpenGLPipeline() override;

    /**
     * @brief Get the program handle
     *
     * @return The program handle
     */
    GLuint GetProgram() const;

    /**
     * @brief Check if the pipeline is linked
     *
     * @return True if the pipeline is linked, false otherwise
     */
    Bool IsLinked() const;

    /**
     * @brief Get the link log
     *
     * @return The link log
     */
    const String &GetLinkLog() const;

    /**
     * @brief Set a uniform value
     *
     * @param name The name of the uniform
     * @param value The value to set
     */
    void SetUniform(const String &name, I32 value) override;

    /**
     * @brief Set a uniform value
     *
     * @param name The name of the uniform
     * @param value The value to set
     */
    void SetUniform(const String &name, F32 value) override;

    /**
     * @brief Set a uniform value
     *
     * @param name The name of the uniform
     * @param value The value to set
     */
    void SetUniform(const String &name, const JzVec2 &value) override;

    /**
     * @brief Set a uniform value
     *
     * @param name The name of the uniform
     * @param value The value to set
     */
    void SetUniform(const String &name, const JzVec3 &value) override;

    /**
     * @brief Set a uniform value
     *
     * @param name The name of the uniform
     * @param value The value to set
     */
    void SetUniform(const String &name, const JzVec4 &value) override;

    /**
     * @brief Set a uniform value
     *
     * @param name The name of the uniform
     * @param value The value to set
     */
    void SetUniform(const String &name, const JzMat3 &value) override;

    /**
     * @brief Set a uniform value
     *
     * @param name The name of the uniform
     * @param value The value to set
     */
    void SetUniform(const String &name, const JzMat4 &value) override;

private:
    /**
     * @brief Link the program
     * @return True if the program is linked, false otherwise
     */
    Bool LinkProgram();

    /**
     * @brief Get the uniform location
     * @param name The name of the uniform
     * @return The uniform location
     */
    GLint GetUniformLocation(const String &name);

private:
    GLuint                                       m_program  = 0;
    Bool                                         m_isLinked = false;
    String                                       m_linkLog;
    std::vector<std::shared_ptr<JzOpenGLShader>> m_shaders;
    std::unordered_map<String, GLint>            m_uniformLocations;
};

} // namespace JzRE