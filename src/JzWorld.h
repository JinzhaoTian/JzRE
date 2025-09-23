/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <vector>
#include <memory>
#include "JzSystem.h"
#include "JzEntityManager.h"

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
        return m_manager;
    }

private:
    JzEntityManager                        m_manager;
    std::vector<std::shared_ptr<JzSystem>> m_systems;
};

template <typename T>
std::shared_ptr<T> JzWorld::RegisterSystem()
{
    auto system = std::make_shared<T>();
    m_systems.push_back(system);
    return system;
}

} // namespace JzRE
