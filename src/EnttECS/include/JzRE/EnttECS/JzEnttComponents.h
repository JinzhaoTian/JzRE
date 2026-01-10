/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

/**
 * @brief This header includes all components for use with the EnTT ECS.
 *
 * The components are shared between the original ECS and the EnTT-based ECS.
 * All component definitions are in JzRE/ECS/JzComponent.h.
 *
 * This allows for:
 * - Code reuse between the two ECS implementations
 * - Easy migration of existing components
 * - Consistent component definitions across the codebase
 */

// Re-export all components from the original ECS module
// These are pure data structures and work with both ECS implementations
#include "JzRE/ECS/JzComponent.h"

namespace JzRE {

// ==================== EnTT-Specific Components ====================

/**
 * @brief Tag component to mark an entity as active.
 *
 * Tag components are empty structs used to categorize entities without
 * storing any data. They are very efficient in EnTT.
 */
struct JzActiveTag { };

/**
 * @brief Tag component to mark an entity as static (non-moving).
 */
struct JzStaticTag { };

/**
 * @brief Tag component to mark an entity for destruction at the end of the frame.
 */
struct JzPendingDestroyTag { };

/**
 * @brief Component storing a human-readable name for an entity.
 */
struct JzNameComponent {
    String name;

    JzNameComponent() = default;

    explicit JzNameComponent(const String &n) :
        name(n) { }

    explicit JzNameComponent(String &&n) :
        name(std::move(n)) { }
};

/**
 * @brief Component storing a unique identifier (UUID) for serialization.
 */
struct JzUUIDComponent {
    U64 uuid;

    JzUUIDComponent() :
        uuid(0) { }

    explicit JzUUIDComponent(U64 id) :
        uuid(id) { }
};

} // namespace JzRE
