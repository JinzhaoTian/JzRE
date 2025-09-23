/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <vector>
#include <memory>
#include "JzEntityManager.h"
#include "JzComponentPool.h"
#include "JzSystem.h"

namespace JzRE {

/**
 * @brief The World class coordinates all systems and the entity manager.
 */
class JzWorld {
public:
    /**
     * @brief Create a entity
     *
     * @return JzEntity
     */
    JzEntity CreateEntity();

    /**
     * @brief Destroy a entity
     *
     * @param entity
     */
    void DestroyEntity(JzEntity entity);

    /**
     * @brief Registers a new component
     *
     * @tparam T
     */
    template <typename T>
    void RegisterComponent();

    /**
     * @brief Add a component
     *
     * @tparam T
     *
     * @param entity
     * @param component
     *
     * @return T&
     */
    template <typename T>
    T &AddComponent(JzEntity entity, T &&component);

    /**
     * @brief Remove a component
     *
     * @tparam T
     *
     * @param entity
     */
    template <typename T>
    void RemoveComponent(JzEntity entity);

    /**
     * @brief Get the Component object
     *
     * @tparam T
     *
     * @param entity
     *
     * @return T&
     */
    template <typename T>
    T &GetComponent(JzEntity entity);

    /**
     * @brief Get the Component Pool object
     *
     * @tparam T
     * @return std::shared_ptr<JzComponentPool<T>>
     */
    template <typename T>
    std::shared_ptr<JzComponentPool<T>> GetComponentPool();

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
    JzEntityManager                                m_entityManager;
    std::vector<std::shared_ptr<JzIComponentPool>> m_componentPools;
    std::vector<std::shared_ptr<JzSystem>>         m_systems;
};

} // namespace JzRE

#include "JzWorld.inl" // IWYU pragma: keep
