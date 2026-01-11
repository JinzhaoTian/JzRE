/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>
#include <vector>
#include <entt/entt.hpp>
#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Function/ECS/EnTT/JzEnttEntity.h"
#include "JzRE/Runtime/Function/ECS/EnTT/JzEnttSystem.h"

namespace JzRE {

/**
 * @brief The EnTT-based World class that manages entities, components, and systems.
 *
 * This class wraps the entt::registry and provides a higher-level interface
 * for entity and component management. It serves as the central hub for all
 * ECS operations in the EnTT-based implementation.
 */
class JzEnttWorld {
public:
    /**
     * @brief Default constructor.
     */
    JzEnttWorld() = default;

    /**
     * @brief Destructor.
     */
    ~JzEnttWorld() = default;

    // Non-copyable, movable
    JzEnttWorld(const JzEnttWorld &)            = delete;
    JzEnttWorld &operator=(const JzEnttWorld &) = delete;
    JzEnttWorld(JzEnttWorld &&)                 = default;
    JzEnttWorld &operator=(JzEnttWorld &&)      = default;

    // ==================== Entity Management ====================

    /**
     * @brief Creates a new entity.
     *
     * @return The newly created entity.
     */
    JzEnttEntity CreateEntity();

    /**
     * @brief Destroys an entity and all its associated components.
     *
     * @param entity The entity to destroy.
     */
    void DestroyEntity(JzEnttEntity entity);

    /**
     * @brief Checks if an entity is still valid (not destroyed).
     *
     * @param entity The entity to check.
     *
     * @return True if the entity is valid, false otherwise.
     */
    Bool IsValid(JzEnttEntity entity) const;

    /**
     * @brief Gets the number of currently alive entities.
     *
     * @return The entity count.
     */
    Size GetEntityCount() const;

    // ==================== Component Management ====================

    /**
     * @brief Adds a component to an entity.
     *
     * @tparam T The component type.
     * @tparam Args The argument types for the component constructor.
     *
     * @param entity The entity to add the component to.
     * @param args The arguments to forward to the component constructor.
     *
     * @return A reference to the newly added component.
     */
    template <typename T, typename... Args>
    T &AddComponent(JzEnttEntity entity, Args &&...args);

    /**
     * @brief Removes a component from an entity.
     *
     * @tparam T The component type.
     *
     * @param entity The entity to remove the component from.
     */
    template <typename T>
    void RemoveComponent(JzEnttEntity entity);

    /**
     * @brief Gets a component from an entity.
     *
     * @tparam T The component type.
     *
     * @param entity The entity to get the component from.
     *
     * @return A reference to the component.
     */
    template <typename T>
    T &GetComponent(JzEnttEntity entity);

    /**
     * @brief Gets a component from an entity (const version).
     *
     * @tparam T The component type.
     *
     * @param entity The entity to get the component from.
     *
     * @return A const reference to the component.
     */
    template <typename T>
    const T &GetComponent(JzEnttEntity entity) const;

    /**
     * @brief Tries to get a component from an entity.
     *
     * @tparam T The component type.
     *
     * @param entity The entity to get the component from.
     *
     * @return A pointer to the component, or nullptr if not found.
     */
    template <typename T>
    T *TryGetComponent(JzEnttEntity entity);

    /**
     * @brief Tries to get a component from an entity (const version).
     *
     * @tparam T The component type.
     *
     * @param entity The entity to get the component from.
     *
     * @return A const pointer to the component, or nullptr if not found.
     */
    template <typename T>
    const T *TryGetComponent(JzEnttEntity entity) const;

    /**
     * @brief Checks if an entity has a specific component.
     *
     * @tparam T The component type.
     *
     * @param entity The entity to check.
     *
     * @return True if the entity has the component, false otherwise.
     */
    template <typename T>
    Bool HasComponent(JzEnttEntity entity) const;

    // ==================== View/Query ====================

    /**
     * @brief Creates a view for iterating over entities with specific components.
     *
     * @tparam Components The component types to query.
     *
     * @return An EnTT view that can be iterated over.
     *
     * @note This is the preferred way to query entities in EnTT as it provides
     *       optimal performance through cache-friendly iteration.
     *
     * @code
     * // Usage example:
     * for (auto [entity, transform, velocity] : world.View<JzTransformComponent, JzVelocityComponent>().each()) {
     *     transform.position += velocity.velocity * deltaTime;
     * }
     * @endcode
     */
    template <typename... Components>
    auto View();

    /**
     * @brief Creates a view for iterating over entities with specific components (const version).
     *
     * @tparam Components The component types to query.
     *
     * @return An EnTT view that can be iterated over.
     */
    template <typename... Components>
    auto View() const;

    // ==================== System Management ====================

    /**
     * @brief Registers a new system.
     *
     * @tparam T The system type (must derive from JzEnttSystem).
     * @tparam Args The argument types for the system constructor.
     *
     * @param args The arguments to forward to the system constructor.
     *
     * @return A shared pointer to the registered system.
     */
    template <typename T, typename... Args>
    std::shared_ptr<T> RegisterSystem(Args &&...args);

    /**
     * @brief Updates all registered systems.
     *
     * @param delta The delta time since the last frame.
     */
    void Update(F32 delta);

    // ==================== Direct Registry Access ====================

    /**
     * @brief Gets direct access to the underlying entt::registry.
     *
     * @return A reference to the registry.
     *
     * @note Use this for advanced EnTT features not exposed through this wrapper.
     */
    entt::registry &GetRegistry()
    {
        return m_registry;
    }

    /**
     * @brief Gets direct access to the underlying entt::registry (const version).
     *
     * @return A const reference to the registry.
     */
    const entt::registry &GetRegistry() const
    {
        return m_registry;
    }

private:
    entt::registry                             m_registry; ///< The EnTT registry holding all entities and components.
    std::vector<std::shared_ptr<JzEnttSystem>> m_systems;  ///< Registered systems.
};

} // namespace JzRE

#include "JzEnttWorld.inl" // IWYU pragma: keep
