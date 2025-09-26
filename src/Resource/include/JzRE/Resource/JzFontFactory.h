/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Resource/JzResourceFactory.h"
#include "JzRE/Resource/JzFont.h"

namespace JzRE {

/**
 * @brief Font resource factory
 */
class JzFontFactory : public JzResourceFactory {
public:
    /**
     * @brief Create a font resource
     *
     * @param name
     *
     * @return JzResource*
     */
    virtual JzResource *Create(const String &name) override
    {
        // A real implementation might need to pass the font size.
        return new JzFont(name, 48); // Defaulting to size 48 for simplicity
    }
};

} // namespace JzRE
