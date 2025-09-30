/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>
#include "JzRE/RHI/JzGPUShaderProgramObject.h"
#include "JzRE/Resource/JzResource.h"

namespace JzRE {

/**
 * @brief Represents a single shader stage (e.g., vertex, fragment) as a resource.
 */
class JzShader : public JzResource {
public:
    /**
     * @brief Constructor.
     *
     * @param path The file path to the shader source.
     * @param type The type of the shader stage.
     */
    JzShader(const String &path, JzEShaderProgramType type);

    /**
     * @brief Destructor.
     */
    virtual ~JzShader();

    /**
     * @brief Loads the shader source and compiles it.
     *
     * @return Bool True if successful.
     */
    virtual Bool Load() override;

    /**
     * @brief Unloads the shader, releasing GPU memory.
     */
    virtual void Unload() override;

    /**
     * @brief Get the RHI Shader object.
     *
     * @return std::shared_ptr<JzGPUShaderProgramObject>
     */
    std::shared_ptr<JzGPUShaderProgramObject> GetRhiShader() const
    {
        return m_rhiShader;
    }

private:
    String                                    m_path;
    JzEShaderProgramType                      m_type;
    std::shared_ptr<JzGPUShaderProgramObject> m_rhiShader;
};

} // namespace JzRE
