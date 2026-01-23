/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>
#include <vector>
#include <entt/entt.hpp>
#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Function/ECS/JzEntity.h"
#include "JzRE/Runtime/Function/ECS/JzSystem.h"

namespace JzRE {

/**
 * @brief The EnTT-based World class that manages entities, components, and systems.
 *
 * This class wraps the entt::registry and provides a higher-level interface
 * for entity and component management. It serves as the central hub for all
 * ECS operations in the EnTT-based implementation.
 */
class JzWorld {
public:
    /**
     * @brief Default constructor.
     */
    JzWorld() = default;

    /**
     * @brief Destructor.
     */
    ~JzWorld() = default;

    // Non-copyable, movable
    JzWorld(const JzWorld &)            = delete;
    JzWorld &operator=(const JzWorld &) = delete;
    JzWorld(JzWorld &&)                 = default;
    JzWorld &operator=(JzWorld &&)      = default;

    // ==================== Entity Management ====================

    /**
     * @brief Creates a new entity.
     *
     * @return The newly created entity.
     */
    JzEntity CreateEntity();

    /**
     * @brief Destroys an entity and all its associated components.
     *
     * @param entity The entity to destroy.
     */
    void DestroyEntity(JzEntity entity);

    /**
     * @brief Checks if an entity is still valid (not destroyed).
     *
     * @param entity The entity to check.
     *
     * @return True if the entity is valid, false otherwise.
     */
    Bool IsValid(JzEntity entity) const;

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
    decltype(auto) AddComponent(JzEntity entity, Args &&...args);

    /**
     * @brief Adds or updates a component on an entity.
     *
     * If the entity already has the component, it will be replaced with the new one.
     * Otherwise, a new component will be added.
     *
     * @tparam T The component type.
     * @tparam Args The argument types for the component constructor.
     *
     * @param entity The entity to add or update the component on.
     * @param args The arguments to forward to the component constructor.
     *
     * @return A reference to the added or updated component.
     */
    template <typename T, typename... Args>
    decltype(auto) AddOrReplaceComponent(JzEntity entity, Args &&...args);

    /**
     * @brief Removes a component from an entity.
     *
     * @tparam T The component type.
     *
     * @param entity The entity to remove the component from.
     */
    template <typename T>
    void RemoveComponent(JzEntity entity);

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
    T &GetComponent(JzEntity entity);

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
    const T &GetComponent(JzEntity entity) const;

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
    T *TryGetComponent(JzEntity entity);

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
    const T *TryGetComponent(JzEntity entity) const;

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
    Bool HasComponent(JzEntity entity) const;

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
     * @tparam T The system type (must derive from JzSystem).
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

    /**
     * @brief Updates only systems in the Logic phase.
     *
     * Logic systems handle game logic, physics, AI, animations, etc.
     * This can run in parallel with GPU work from the previous frame.
     *
     * @param delta The delta time since the last frame.
     */
    void UpdateLogic(F32 delta);

    /**
     * @brief Updates only systems in the PreRender phase.
     *
     * PreRender systems prepare data for rendering: camera matrices,
     * light collection, culling, LOD selection, etc.
     *
     * @param delta The delta time since the last frame.
     */
    void UpdatePreRender(F32 delta);

    /**
     * @brief Updates only systems in the Render phase.
     *
     * Render systems perform actual GPU rendering operations.
     * Must run on the main thread after synchronization.
     *
     * @param delta The delta time since the last frame.
     */
    void UpdateRender(F32 delta);

    /**
     * @brief Updates only systems in a specific phase.
     *
     * Use this for fine-grained control over individual phases.
     *
     * @param phase The specific phase to update.
     * @param delta The delta time since the last frame.
     */
    void UpdatePhase(JzSystemPhase phase, F32 delta);

private:
    entt::registry                         m_registry; ///< The EnTT registry holding all entities and components.
    std::vector<std::shared_ptr<JzSystem>> m_systems;  ///< Registered systems.
};

} // namespace JzRE

#include "JzRE/Runtime/Function/ECS/JzWorld.inl" // IWYU pragma: keep
