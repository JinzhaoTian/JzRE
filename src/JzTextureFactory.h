/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzResourceFactory.h"
#include "JzTexture.h"

namespace JzRE {

/**
 * @brief Texture resource factory
 */
class JzTextureFactory : public JzResourceFactory {
public:
    /**
     * @brief Create a texture resource
     *
     * @param name
     *
     * @return JzResource*
     */
    virtual JzResource *Create(const String &name) override
    {
        // TODO
        return new JzTexture(name);
    }
};

} // namespace JzRE
