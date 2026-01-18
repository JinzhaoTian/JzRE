/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Core/JzRETypes.h"

namespace JzRE {

/**
 * @brief Key for identifying a specific shader variant.
 *
 * A variant is identified by a bitmask of enabled keywords.
 * Each bit position corresponds to a keyword index defined in JzShaderProgram.
 */
struct JzShaderVariantKey {
    U64 keywordMask = 0;

    bool operator==(const JzShaderVariantKey &other) const
    {
        return keywordMask == other.keywordMask;
    }

    bool operator!=(const JzShaderVariantKey &other) const
    {
        return keywordMask != other.keywordMask;
    }

    /**
     * @brief Hash functor for use in unordered containers.
     */
    struct Hash {
        size_t operator()(const JzShaderVariantKey &key) const
        {
            return std::hash<U64>{}(key.keywordMask);
        }
    };
};

/**
 * @brief Builder for constructing variant keys.
 */
class JzShaderVariantKeyBuilder {
public:
    /**
     * @brief Enable a keyword by its index.
     * @param keywordIndex The bit position of the keyword (0-63).
     * @return Reference to this builder for chaining.
     */
    JzShaderVariantKeyBuilder &EnableKeyword(U32 keywordIndex);

    /**
     * @brief Disable a keyword by its index.
     * @param keywordIndex The bit position of the keyword (0-63).
     * @return Reference to this builder for chaining.
     */
    JzShaderVariantKeyBuilder &DisableKeyword(U32 keywordIndex);

    /**
     * @brief Set a keyword's enabled state by its index.
     * @param keywordIndex The bit position of the keyword (0-63).
     * @param enabled Whether the keyword should be enabled.
     * @return Reference to this builder for chaining.
     */
    JzShaderVariantKeyBuilder &SetKeyword(U32 keywordIndex, Bool enabled);

    /**
     * @brief Build the final variant key.
     * @return The constructed JzShaderVariantKey.
     */
    JzShaderVariantKey Build() const;

    /**
     * @brief Reset the builder to default state.
     * @return Reference to this builder for chaining.
     */
    JzShaderVariantKeyBuilder &Reset();

private:
    U64 m_mask = 0;
};

} // namespace JzRE
