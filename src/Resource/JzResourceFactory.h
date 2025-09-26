/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzResource.h"

namespace JzRE {

/**
 * @brief Interface for JzRE resource factory
 */
class JzResourceFactory {
public:
    /**
     * @brief Destructor
     */
    virtual ~JzResourceFactory() = default;

    /**
     * @brief create a resource method
     *
     * @param name
     * @return JzResource*
     */
    virtual JzResource *Create(const String &name) = 0;
};

} // namespace JzRE