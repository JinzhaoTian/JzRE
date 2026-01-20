/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Resource/JzResourceFactory.h"
#include "JzRE/Runtime/Resource/JzShaderSource.h"

namespace JzRE {

/**
 * @brief Factory for creating JzShaderSource resources
 */
class JzShaderSourceFactory : public JzResourceFactory {
public:
    /**
     * @brief Create a shader source resource
     *
     * @param name The file path to the shader source
     * @return JzResource* Pointer to the created JzShaderSource
     */
    virtual JzResource *Create(const String &name) override
    {
        return new JzShaderSource(name);
    }
};

} // namespace JzRE
