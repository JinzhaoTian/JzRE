/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <entt/entt.hpp>
#include "JzRE/Runtime/Core/JzRETypes.h"

namespace JzRE {

/**
 * @brief Represents an entity in the EnTT-based ECS.
 *
 * This is a type alias to entt::entity for compatibility with the EnTT library.
 * Unlike the original JzEntity (U32), this uses entt's entity type which includes
 * version information for entity recycling.
 */
using JzEntity = entt::entity;

/**
 * @brief Invalid entity constant for the EnTT-based ECS.
 */
constexpr JzEntity INVALID_ENTT_ENTITY = entt::null;

/**
 * @brief Converts a JzEntity to its underlying integer representation.
 *
 * @param entity The entity to convert.
 *
 * @return The underlying integer value.
 */
inline U32 ToEntityId(JzEntity entity)
{
    return static_cast<U32>(entt::to_integral(entity));
}

/**
 * @brief Checks if an entity is valid (not null).
 *
 * @param entity The entity to check.
 *
 * @return True if the entity is valid, false otherwise.
 */
inline Bool IsValidEntity(JzEntity entity)
{
    return entity != entt::null;
}

} // namespace JzRE
