/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include "JzRE/Runtime/Function/ECS/JzEntity.h"
#include "JzRE/Runtime/Function/ECS/JzIComponentPool.h"
#include "JzRE/Runtime/Function/ECS/JzComponentPool.h"

namespace JzRE {

/**
 * @brief Manages all entities and their components.
 */
class JzEntityManager {
public:
    /**
     * @brief Creates a new entity.
     *
     * @return The new entity's ID.
     */
    JzEntity CreateEntity();

    /**
     * @brief Destroys an entity and all its components.
     *
     * @param entity The entity to destroy.
     */
    void DestroyEntity(JzEntity entity);

    /**
     * @brief Adds a component to an entity.
     *
     * @tparam T The component type.
     * @tparam ...Args The arguments for the component's constructor.
     *
     * @param entity The entity.
     * @param ...args The arguments to construct the component with.
     *
     * @return A reference to the newly added component.
     */
    template <typename T, typename... Args>
    T &AddComponent(JzEntity entity, Args &&...args);

    /**
     * @brief Removes a component from an entity.
     *
     * @tparam T The component type.
     *
     * @param entity The entity.
     */
    template <typename T>
    void RemoveComponent(JzEntity entity);

    /**
     * @brief Gets a component from an entity.
     *
     * @tparam T The component type.
     *
     * @param entity The entity.
     *
     * @return A reference to the component.
     */
    template <typename T>
    T &GetComponent(JzEntity entity);

    /**
     * @brief Checks if an entity has a component.
     *
     * @tparam T The component type.
     *
     * @param entity The entity.
     *
     * @return True if the entity has the component, false otherwise.
     */
    template <typename T>
    Bool HasComponent(JzEntity entity);

    /**
     * @brief Creates a view to iterate over entities with specific components.
     *
     * @tparam ...Components The component types to view.
     *
     * @return A vector of entities that have all the specified components.
     */
    template <typename... Components>
    std::vector<JzEntity> View();

private:
    /**
     * @brief Gets or creates a component pool for a given component type.
     *
     * @tparam T The component type.
     *
     * @return A shared pointer to the component pool.
     */
    template <typename T>
    std::shared_ptr<JzComponentPool<T>> GetPool();

private:
    JzEntity                                                            m_entity_counter = 0;
    std::unordered_map<const char *, std::shared_ptr<JzIComponentPool>> m_component_pools;
};

} // namespace JzRE

#include "JzRE/Runtime/Function/ECS/JzEntityManager.inl" // IWYU pragma: keep