/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <vector>
#include <memory>
#include "JzEntityManager.h"
#include "JzSystem.h"

namespace JzRE {

/**
 * @brief The World class coordinates all systems and the entity manager.
 */
class JzWorld {
public:
    /**
     * @brief Registers a new system.
     *
     * @tparam T The type of the system to register.
     *
     * @return A shared pointer to the created system.
     */
    template <typename T>
    std::shared_ptr<T> RegisterSystem();

    /**
     * @brief Calls the Update method on all registered systems.
     *
     * @param delta The delta time since the last frame.
     */
    void Update(F32 delta);

    /**
     * @brief Gets a reference to the entity manager.
     *
     * @return A reference to the entity manager.
     */
    JzEntityManager &GetManager()
    {
        return m_entityManager;
    }

private:
    JzEntityManager                        m_entityManager;
    std::vector<std::shared_ptr<JzSystem>> m_systems;
};

} // namespace JzRE

#include "JzWorld.inl" // IWYU pragma: keep
