/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2026 JzRE
 */

#pragma once

#include <filesystem>

#include "JzRE/Runtime/Resource/JzResourceFactory.h"
#include "JzRE/Runtime/Resource/JzShader.h"

namespace JzRE {

/**
 * @brief Factory for cooked shader assets.
 *
 * Accepted input forms:
 * - "shaders/standard" (auto appends .jzshader)
 * - "shaders/standard.jzshader"
 */
class JzShaderFactory : public JzResourceFactory {
public:
    /**
     * @brief Create a cooked shader asset resource.
     */
    JzResource *Create(const String &name) override
    {
        namespace fs = std::filesystem;

        fs::path path(name);
        if (path.extension().empty()) {
            path += ".jzshader";
        }

        return new JzShader(path.string());
    }
};

} // namespace JzRE
