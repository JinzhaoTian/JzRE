/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Resource/JzResource.h"

namespace JzRE {

/**
 * @brief Lightweight shader source resource (text only, no GPU compilation)
 *
 * Unlike JzShader which compiles to GPU, JzShaderSource only holds the
 * source code text. This is used by JzShaderManager for variant compilation.
 */
class JzShaderSource : public JzResource {
public:
    /**
     * @brief Constructor
     * @param path The file path to the shader source
     */
    explicit JzShaderSource(const String &path);

    /**
     * @brief Destructor
     */
    virtual ~JzShaderSource() = default;

    /**
     * @brief Load the shader source from file
     * @return Bool True if successful
     */
    virtual Bool Load() override;

    /**
     * @brief Unload the shader source
     */
    virtual void Unload() override;

    /**
     * @brief Get the shader source code
     * @return const String& The source code
     */
    const String &GetSource() const
    {
        return m_source;
    }

private:
    String m_path;
    String m_source;
};

} // namespace JzRE
