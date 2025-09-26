/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzResourceFactory.h"
#include "JzShader.h"

namespace JzRE {

/**
 * @brief Shader resource factory
 */
class JzShaderFactory : public JzResourceFactory {
public:
    /**
     * @brief Create a shader resource
     *
     * @param name
     *
     * @return JzResource*
     */
    virtual JzResource *Create(const String &name) override
    {
        // TODO A real implementation might need to know the shader type from the extension or another parameter.
        return new JzShader(name, JzEShaderType::Vertex); // Defaulting to Vertex for simplicity
    }
};

} // namespace JzRE
