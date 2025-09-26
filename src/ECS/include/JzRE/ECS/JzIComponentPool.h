/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzEntity.h"

namespace JzRE {

/**
 * @brief Non-template base class for component pools to allow storing them polymorphically.
 */
class JzIComponentPool {
public:
    /**
     * @brief Destructor
     */
    virtual ~JzIComponentPool() = default;

    /**
     * @brief Remove entity
     *
     * @param entity
     */
    virtual void RemoveEntity(JzEntity entity) = 0;
};

} // namespace JzRE
