/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzResourceFactory.h"
#include "JzMaterial.h"

namespace JzRE {

/**
 * @brief Material resource factory
 */
class JzMaterialFactory : public JzResourceFactory {
public:
    /**
     * @brief Create a material resource
     *
     * @param name
     *
     * @return JzResource*
     */
    virtual JzResource *Create(const String &name) override
    {
        // This assumes a constructor JzMaterial(const std::string&) exists.
        return new JzMaterial(name);
    }
};

} // namespace JzRE
