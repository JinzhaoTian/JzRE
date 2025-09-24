/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzResourceFactory.h"
#include "JzModel.h"

namespace JzRE {
/**
 * @brief Model resource factory
 */
class JzModelFactory : public JzResourceFactory {
public:
    /**
     * @brief Create a model resource
     *
     * @param name
     *
     * @return JzResource*
     */
    virtual JzResource *Create(const String &name) override
    {
        return new JzModel(name);
    }
};

} // namespace JzRE
