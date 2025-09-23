/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <vector>
#include "JzEntity.h"
#include "JzIComponentPool.h"

namespace JzRE {

/**
 * @brief A component pool using a sparse set for efficient storage and iteration.
 *
 * @tparam T The component type.
 */
template <typename T>
class JzComponentPool : public JzIComponentPool {
public:
    /**
     * @brief Adds a component for a given entity.
     *
     * @param entity The entity.
     * @param component The component to add.
     *
     * @throw std::runtime_error if the entity already has this component.
     */
    void Add(JzEntity entity, T &&component);

    /**
     * @brief Removes a component from a given entity.
     *
     * @param entity The entity.
     */
    void RemoveEntity(JzEntity entity) override;

    /**
     * @brief Gets the component for a given entity.
     *
     * @param entity The entity.
     *
     * @return A reference to the component.
     *
     * @throw std::runtime_error if the entity does not have this component.
     */
    T &Get(JzEntity entity);

    /**
     * @brief Checks if an entity has a component.
     *
     * @param entity The entity.
     *
     * @return True if the entity has the component, false otherwise.
     */
    Bool Has(JzEntity entity) const;

    /**
     * @brief Get the dense array of entities.
     *
     * @return A const reference to the dense array.
     */
    const std::vector<JzEntity> &GetDense() const
    {
        return m_dense;
    }

private:
    std::vector<T>        m_components; // Tightly packed components
    std::vector<JzEntity> m_dense;      // Tightly packed entity IDs
    std::vector<I32>      m_sparse;     // Maps entity ID to index in dense array
};

} // namespace JzRE

#include "JzComponentPool.inl" // IWYU pragma: keep