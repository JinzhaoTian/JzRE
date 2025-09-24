/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzResourceFactory.h"
#include "JzMesh.h"

namespace JzRE {

/**
 * @brief Mesh resource factory
 */
class JzMeshFactory : public JzResourceFactory {
public:
    /**
     * @brief Create a mesh resource
     *
     * @param name
     *
     * @return JzResource*
     */
    virtual JzResource *Create(const String &name) override
    {
        // This assumes a constructor JzMesh(const std::string&) exists.
        return new JzMesh(name);
    }
};

} // namespace JzRE
