/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/ECS/JzComponentPool.h"
#include <stdexcept>

namespace JzRE {

template <typename T>
void JzComponentPool<T>::Add(JzEntity entity, T &&component)
{
    if (Has(entity)) {
        throw std::runtime_error("Entity already has this component.");
    }

    if (entity >= m_sparse.size()) {
        m_sparse.resize(entity + 1, -1);
    }

    m_sparse[entity] = m_dense.size();
    m_dense.push_back(entity);
    m_components.emplace_back(std::forward<T>(component));
}

template <typename T>
void JzComponentPool<T>::RemoveEntity(JzEntity entity)
{
    if (!Has(entity)) {
        return;
    }

    // Swap-and-pop
    size_t   index_to_remove = m_sparse[entity];
    JzEntity last_entity     = m_dense.back();

    // Move the last element into the place of the removed element
    m_dense[index_to_remove]      = last_entity;
    m_components[index_to_remove] = std::move(m_components.back());

    // Update the sparse array for the moved entity
    m_sparse[last_entity] = index_to_remove;

    // Invalidate the removed entity's sparse entry
    m_sparse[entity] = -1;

    // Pop the back
    m_dense.pop_back();
    m_components.pop_back();
}

template <typename T>
T &JzComponentPool<T>::Get(JzEntity entity)
{
    if (!Has(entity)) {
        throw std::runtime_error("Entity does not have this component.");
    }
    return m_components[m_sparse[entity]];
}

template <typename T>
Bool JzComponentPool<T>::Has(JzEntity entity) const
{
    return entity < m_sparse.size() && m_sparse[entity] != -1 && m_sparse[entity] < m_dense.size() && m_dense[m_sparse[entity]] == entity;
}

} // namespace JzRE