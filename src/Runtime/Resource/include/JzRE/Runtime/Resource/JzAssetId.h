/**
 * @file    JzAssetId.h
 * @brief   Asset unique identifier with generation support
 * @author  Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <functional>
#include "JzRE/Runtime/Core/JzRETypes.h"

namespace JzRE {

/**
 * @brief Asset unique identifier with index and generation
 *
 * Layout: [32-bit index][32-bit generation]
 *
 * The generation counter prevents dangling references by ensuring that
 * when a slot is reused, the old handles become invalid.
 *
 * @note This design is inspired by slot map / generational arena patterns
 *       commonly used in game engines for safe resource management.
 */
struct JzAssetId {
    U32 index      = 0xFFFFFFFF; ///< Slot index in the registry
    U32 generation = 0;          ///< Generation counter for validation

    /**
     * @brief Create an invalid asset ID
     */
    static constexpr JzAssetId Invalid()
    {
        return {0xFFFFFFFF, 0};
    }

    /**
     * @brief Check if this ID is valid (not the invalid sentinel)
     */
    [[nodiscard]] Bool IsValid() const
    {
        return index != 0xFFFFFFFF;
    }

    /**
     * @brief Equality comparison
     */
    Bool operator==(const JzAssetId &other) const
    {
        return index == other.index && generation == other.generation;
    }

    /**
     * @brief Inequality comparison
     */
    Bool operator!=(const JzAssetId &other) const
    {
        return !(*this == other);
    }

    /**
     * @brief Hash functor for use with unordered containers
     *
     * Combines index and generation into a single 64-bit hash.
     */
    struct Hash {
        Size operator()(const JzAssetId &id) const
        {
            // Combine index and generation into a 64-bit value for hashing
            U64 combined = (static_cast<U64>(id.generation) << 32) | id.index;
            return std::hash<U64>{}(combined);
        }
    };
};

} // namespace JzRE
