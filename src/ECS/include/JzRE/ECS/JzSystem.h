/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Core/JzRETypes.h"

namespace JzRE {

// Forward declaration
class JzEntityManager;

/**
 * @brief Abstract base class for all systems.
 */
class JzSystem {
public:
    /**
     * @brief Destructor
     */
    virtual ~JzSystem() = default;

    /**
     * @brief The update method called every frame.
     *
     * @param manager The entity manager.
     * @param delta The delta time since the last frame.
     */
    virtual void Update(JzEntityManager &manager, F32 delta) = 0;
};

} // namespace JzRE
